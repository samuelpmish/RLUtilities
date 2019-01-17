#include "curve.h"

#include "Drive.h"

Curve::Curve() {
  length = 0.0f;
  points = std::vector < vec3 >();
  tangents = std::vector < vec3 >();
  distances = std::vector < float >();
}

Curve::Curve(const CompositeArc &path) {

  float radians_per_arc_segment = 0.1f;
  float length_per_straight_segment = 200.0f;

  if (path.L0 > 0) {
    const vec2 p0 = path.p1 - path.L0 * path.t1;
    size_t n0 = ceil(path.L0 / length_per_straight_segment);
    for (size_t i = 0; i < n0; i++) {
      points.push_back(lerp(p0, path.p1, float(i) / float(n0)));
    }
  }

  size_t n1 = ceil(path.phi1 / radians_per_arc_segment);
  mat2 Q = rotation(sgn(path.r1) * path.phi1 / n1);
  vec2 delta = path.p1 - path.o1;
  for (size_t i = 0; i <= n1; i++) {
     points.push_back(vec3(path.o1 + delta));
     delta = dot(Q, delta);
  }

  if (path.L2 > 0) {
    size_t n2 = ceil(path.L2 / length_per_straight_segment);
    for (size_t i = 1; i < n2; i++) {
      points.push_back(lerp(path.q1, path.q2, float(i) / float(n2)));
    }
  }

  size_t n3 = ceil(path.phi2 / radians_per_arc_segment);
  Q = rotation(sgn(path.r2) * path.phi2 / n3);
  delta = path.q2 - path.o2;
  for (size_t i = 0; i <= n3; i++) {
     points.push_back(vec3(path.o2 + delta));
     delta = dot(Q, delta);
  }

  if (path.L4 > 0) {
    vec2 p3 = path.p2 + path.L4 * path.t2;
    size_t n4 = ceil(path.L4 / length_per_straight_segment);
    for (size_t i = 1; i <= n4; i++) {
      points.push_back(lerp(path.p2, p3, float(i) / float(n4)));
    }
  }

  calculate_distances_and_tangents();
}

Curve::Curve(const std::vector<vec3> &_points) {
  points = _points;
  calculate_distances_and_tangents();
}

vec3 Curve::point_at(float s) {
  s = clip(s, 0, distances[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    if (distances[i] >= s && s >= distances[i + 1]) {
      float u = (s - distances[i + 1]) / (distances[i] - distances[i + 1]);
      return lerp(points[i + 1], points[i], u);
    }
  }

  return vec3{0.0f, 0.0f, 0.0f};
}

vec3 Curve::tangent_at(float s) {
  s = clip(s, 0, distances[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    if (distances[i] >= s && s >= distances[i + 1]) {
      float u = (s - distances[i + 1]) / (distances[i] - distances[i + 1]);
      return normalize(lerp(tangents[i + 1], tangents[i], u));
    }
  }

  return vec3{0.0f, 0.0f, 0.0f};
}

float Curve::curvature_at(float s) {
  s = clip(s, 0, distances[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    if (distances[i] >= s && s >= distances[i + 1]) {
      float delta_theta = angle_between(tangents[i + 1], tangents[i]);
      float delta_s = distances[i] - distances[i + 1];
      std::cout << delta_theta << " " << delta_s << std::endl;
      return delta_theta / delta_s;
    }
  }

  return 0.0f;
}

float Curve::find_nearest(const vec3 &c) {
  float s = length;
  float min_distance = norm(c - points[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    vec3 a = points[i];
    vec3 b = points[i + 1];

    float alpha = clip(dot(b - a, c - a) / dot(b - a, b - a), 0.0f, 1.0f);

    float distance = norm(c - (a + alpha * (b - a)));

    if (distance < min_distance) {
      min_distance = distance;
      s = lerp(distances[i], distances[i + 1], alpha);
    }
  }

  return s;
}

void Curve::pop_front() {
  if (points.size() > 0) {
    points.erase(points.begin());
    tangents.erase(tangents.begin());
    distances.erase(distances.begin());
    length = distances[0];
  }
}

#include <fstream>

void Curve::calculate_distances_and_tangents() {
  distances = std::vector<float>(points.size(), 0.0f);
  tangents = std::vector<vec3>(points.size());
  size_t last = points.size() - 1;

  // measure distances from the end of the curve
  for (int i = last - 1; i >= 0; i--) {
    distances[i] = distances[i + 1] + norm(points[i + 1] - points[i]);
  }

  length = distances[0];

  tangents[0] = normalize(points[1] - points[0]);
  for (int i = 1; i < last; i++) {
    tangents[i] = normalize(points[i + 1] - points[i - 1]);
  }
  tangents[last] = normalize(points[last] - points[last - 1]);

  std::ofstream outfile ("points.csv");
  for (auto p : points) {
    outfile << p << std::endl;  
  }
  outfile.close();
  outfile.open("tangents.csv");
  for (auto t : tangents) {
    outfile << t << std::endl;  
  }
  outfile.close();

}

float Curve::time_lower_bound() {
  float time = 0.0f;

  for (int i = 0; i < tangents.size() - 1; i++) {
    float dtheta = acos(dot(tangents[i],tangents[i+1]));
    float ds = (distances[i] - distances[i+1]);
    float kappa = dtheta / ds;
    time += ds / Drive::max_turning_speed(kappa);
  }

  return time;
}

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
void init_curve(pybind11::module & m) {
  pybind11::class_<Curve>(m, "Curve")
    .def(pybind11::init< std::vector < vec<3> > >())
    .def(pybind11::init< const CompositeArc & >())
    .def("point_at", &Curve::point_at)
    .def("tangent_at", &Curve::tangent_at)
    .def("curvature_at", &Curve::curvature_at)
    .def("find_nearest", &Curve::find_nearest)
    .def("pop_front", &Curve::pop_front)
    .def("recalculate", &Curve::calculate_distances_and_tangents)
    .def_readwrite("points", &Curve::points)
    .def_readonly("length", &Curve::length);
}
#endif
