#pragma once

#include <vector>

#include "linear_algebra/math.h"

class Bezier {
 public:
  size_t num_points;
  std::vector<vec3> points;

  explicit Bezier(const std::vector<vec3>& points_) {
    points = points_;
    num_points = points.size();
  }

  vec3 operator()(float tau) {
    float t = clip(tau, 0.0f, 1.0f);
    float s = 1.0f - t;

    switch (num_points) {
    case 2:
      return s * points[0] + t * points[1];
      break;

    case 3:
      return s * s * points[0] +
        2 * s * t * points[1] +
        t * t * points[2];
      break;

    case 4:
      return s * s * s * points[0] +
        3 * s * s * t * points[1] +
        3 * s * t * t * points[2] + t * t * t * points[3];
      break;
    }

    return vec3{ 0.0f, 0.0f, 0.0f };
  }

  vec3 tangent(float tau) {
    float t = clip(tau, 0.0f, 1.0f);
    float s = 1.0f - t;
    switch (num_points) {
    case 2:
      return points[1] - points[0];
      break;
    case 3:
      return 2 * s * (points[1] - points[0]) +
        2 * t * (points[2] - points[1]);
      break;
    case 4:
      return 3 * s * s * (points[1] - points[0]) +
        6 * s * t * (points[2] - points[1]) +
        3 * t * t * (points[3] - points[2]);
      break;
    }
    return vec3{ 0.0f, 0.0f, 0.0f };
  }

  float length(float t0 = 0, float t1 = 1) {
    float qrule[4][2] = {
      {0.069432, 0.173927},
      {0.330009, 0.326073},
      {0.669991, 0.326073},
      {0.930568, 0.173927}
    };
    float L = 0.0;
    for (int i = 0; i < 4; i++) {
      float t = t0 + qrule[i][0] * (t1 - t0);
      float wt = qrule[i][1] * (t1 - t0);
      L += norm(tangent(t)) * wt;
    }
    return L;
  }

  float t_nearest(const vec3& p) {
    float t = 0.5f;
    for (int i = 0; i < 6; i++) {
      vec3 r = operator()(t) - p;
      vec3 dr = tangent(t);
      if (fabsf(dot(r, normalize(dr))) < 10.0f) {
        break;
      }
      t -= dot(r, dr) / dot(dr, dr);
    }
    return t;
  }
};
