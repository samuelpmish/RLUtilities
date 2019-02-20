#pragma once

#include <vector>

#include "linear_algebra/math.h"

class Curve {
 public:
  float length;
  std::vector<vec3> points;
  std::vector<vec3> tangents;
  std::vector<float> distances;
  std::vector<float> curvatures;

  Curve();
  explicit Curve(const std::vector<vec3> &_points);

  vec3 point_at(float s);
  vec3 tangent_at(float s);
  float curvature_at(float s);

  float find_nearest(const vec3 &c);

  float time_lower_bound();

  void pop_front();
  void calculate_distances();
  void calculate_tangents();

};
