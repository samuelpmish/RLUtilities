#pragma once

#include <vector>

#include "simulation/curve.h"

#include "linear_algebra/math.h"

class Path {

 public:

  std::vector < segment > segments;

  Path();

  void init(int gx, int nx, int ntheta, int nv);

  float minimum_time();

 private:

  static bool initialized;
  static std::vector < std::vector < edge > > neighbors;

  static std::vector < std::vector < std::vector < float > > > times;
  static std::vector < std::vector < std::vector < node > > > parents;

};
