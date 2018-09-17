#pragma once

#include <vector>

#include "linalg.h"
#include "ArcLineArc.h"

class Curve {
 public:
  float length;
  std::vector<vec3> points;
  std::vector<vec3> tangents;
  std::vector<float> distances;

  explicit Curve(const ArcLineArc &path, const float offset) {
    size_t n = 32;

    if (offset > 0) {
      points.push_back(vec3(path.p1 - offset * path.t1));
    }

    mat2 Q = rotation(sgn(path.r1) * path.phi1 / n);
    vec2 delta = path.p1 - path.o1;
    for (size_t i = 0; i < n; i++) {
       points.push_back(vec3(path.o1 + delta));
       delta = dot(Q, delta);
    }

    Q = rotation(sgn(path.r2) * path.phi2 / n);
    delta = path.q2 - path.o2;
    for (size_t i = 0; i < n; i++) {
       points.push_back(vec3(path.o2 + delta));
       delta = dot(Q, delta);
    }

    if (offset > 0) {
      points.push_back(vec3(path.p2 + offset * path.t2));
    }

    calculate_distances_and_tangents();
  }

  explicit Curve(const std::vector<vec3> &_points) {
    points = _points;
    calculate_distances_and_tangents();
  }

  vec3 point_at(float s) {
    s = clamp(s, 0, distances[0]);

    for (int i = 0; i < (points.size() - 1); i++) {
      if (distances[i] >= s && s >= distances[i + 1]) {
        float u = (s - distances[i + 1]) / (distances[i] - distances[i + 1]);
        return lerp(points[i + 1], points[i], u);
      }
    }

    return vec3{0.0f, 0.0f, 0.0f};
  }

  vec3 tangent_at(float s) {
    s = clamp(s, 0, distances[0]);

    for (int i = 0; i < (points.size() - 1); i++) {
      if (distances[i] >= s && s >= distances[i + 1]) {
        float u = (s - distances[i + 1]) / (distances[i] - distances[i + 1]);
        return normalize(lerp(tangents[i + 1], tangents[i], u));
      }
    }

    return vec3{0.0f, 0.0f, 0.0f};
  }

  float find_nearest(const vec3 &c) {
    float s = length;
    float min_distance = norm(c - points[0]);

    for (int i = 0; i < (points.size() - 1); i++) {
      vec3 a = points[i];
      vec3 b = points[i + 1];

      float alpha = clamp(dot(b - a, c - a) / dot(b - a, b - a), 0.0f, 1.0f);

      float distance = norm(c - (a + alpha * (b - a)));

      if (distance < min_distance) {
        min_distance = distance;
        s = lerp(distances[i], distances[i + 1], alpha);
      }
    }

    return s;
  }

  void pop_front() {
    if (points.size() > 0) {
      points.erase(points.begin());
      tangents.erase(tangents.begin());
      distances.erase(distances.begin());
      length = distances[0];
    }
  }

  void calculate_distances_and_tangents() {
    distances = std::vector<float>(points.size(), 0.0f);
    tangents = std::vector<vec3>(points.size());
    int last = points.size() - 1;

    // measure distances from the end of the curve
    for (int i = last - 1; i >= 0; i--) {
      distances[i] = distances[i + 1] + norm(points[i + 1] - points[i]);
    }

    length = distances[0];

    tangents[0] = normalize(points[1] - points[0]);
    for (int i = 1; i < last - 1; i++) {
      tangents[i] = normalize(points[i + 1] - points[i - 1]);
    }
    tangents[points.size() - 1] =
        normalize(points[points.size() - 1] - points[points.size() - 2]);
  }
};
