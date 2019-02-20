#include "simulation/curve.h"

#include "mechanics/drive.h"

Curve::Curve() {
  length = 0.0f;
  points = std::vector < vec3 >();
  tangents = std::vector < vec3 >();
  distances = std::vector < float >();
}

Curve::Curve(const std::vector<vec3> &_points) {
	points = _points;
	calculate_distances();
	calculate_tangents();
}

vec3 Curve::point_at(float s) {
  s = clip(s, 0, distances[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    if (distances[i] >= s && s >= distances[i + 1]) {
      float u = (s - distances[i + 1]) / (distances[i] - distances[i + 1]);
      return lerp(points[i + 1], points[i], u);
    }
  }

  return vec3{ 0.0f, 0.0f, 0.0f };
}

vec3 Curve::tangent_at(float s) {
  s = clip(s, 0, distances[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    if (distances[i] >= s && s >= distances[i + 1]) {
      float u = (s - distances[i + 1]) / (distances[i] - distances[i + 1]);
      return normalize(lerp(tangents[i + 1], tangents[i], u));
    }
  }

  return vec3{ 0.0f, 0.0f, 0.0f };
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

void Curve::calculate_distances() {

  distances = std::vector<float>(points.size(), 0.0f);

  // measure distances from the end of the curve
  for (size_t i = points.size() - 2; i >= 0; i--) {
    distances[i] = distances[i + 1] + norm(points[i + 1] - points[i]);
  }

  length = distances[0];

//  std::ofstream outfile("points.csv");
//  for (auto p : points) {
//    outfile << p << std::endl;
//  }
//  outfile.close();

}

void Curve::calculate_tangents() {

  tangents = std::vector<vec3>(points.size());
  size_t last = tangents.size() - 1;

  tangents[0] = normalize(points[1] - points[0]);
  for (size_t i = 1; i < last; i++) {
    tangents[i] = normalize(points[i + 1] - points[i - 1]);
  }
  tangents[last] = normalize(points[last] - points[last - 1]);

//  outfile.open("tangents.csv");
//  for (auto t : tangents) {
//    outfile << t << std::endl;
//  }
//  outfile.close();

}

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
void init_curve(pybind11::module & m) {
  pybind11::class_<Curve>(m, "Curve")
    .def(pybind11::init< std::vector < vec<3> > >())
    .def("point_at", &Curve::point_at)
    .def("tangent_at", &Curve::tangent_at)
    .def("curvature_at", &Curve::curvature_at)
    .def("find_nearest", &Curve::find_nearest)
    .def("pop_front", &Curve::pop_front)
    .def("calculate_distances", &Curve::calculate_distances)
    .def("calculate_tangents", &Curve::calculate_tangents)
    .def_readwrite("points", &Curve::points)
    .def_readonly("length", &Curve::length);
}
#endif
