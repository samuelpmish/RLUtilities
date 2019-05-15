#pragma once

#include "simulation/car.h"
#include "simulation/input.h"
#include "simulation/curve.h"

#include "linear_algebra/math.h"

#include "misc/graph.h"

#include <array>
#include <vector>

class Navigator {

 public:
  Car & car;

  Navigator(Car & c);

  void analyze_surroundings(float time_budget);
  Curve path_to(vec3, vec3);

  static void init_statics(std::string directory);

 private:

  static float scale;
  static int nx, ntheta, nv;

  static uint32_t to_id(int, int, int, int) const;
  static std::array < int16_t, 4 > from_id(uint32_t) const;

  static std::array < int, 3 > strides;
  static std::vector < float > time_LUT;
  static std::vector < uint32_t > path_LUT;

  static Graph navigation_graph;
  static std::vector < vec3 > navigation_nodes;
  static std::vector < vec3 > navigation_tangents;
  static std::vector < vec3 > navigation_normals;

  int source_id;
  std::vector < int > navigation_paths;

};
