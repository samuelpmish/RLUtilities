#pragma once

#include <vector>
#include <string>

#include "linear_algebra/math.h"

struct ControlPoint {
  vec3 p, t, n;
};

class Curve {
 public:
  float length;
  std::vector<vec3> points;
  std::vector<vec3> tangents;
  std::vector<float> distances;
  std::vector<float> curvatures;
  std::vector<float> max_speeds;

  Curve();
  explicit Curve(const std::vector<vec3> & _points);

  explicit Curve(std::vector < ControlPoint >);
  explicit Curve(std::vector < ControlPoint > info,
      vec3 dx0, vec3 dt0, vec3 dx1, vec3 dt1, vec3 start, vec3 end);

  vec3 point_at(float s);
  vec3 tangent_at(float s);
  float curvature_at(float s);
  float max_speed_at(float s);

  float find_nearest(const vec3 &c);

  void pop_front();
  void calculate_distances();
  void calculate_tangents();
  float calculate_max_speeds(float v0, float vf);

  void write_to_file(std::string prefix);

};
