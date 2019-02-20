#pragma once

#include <vector>

#include "simulation/curve.h"

#include "linear_algebra/math.h"

class Path {

 struct node {
   int16_t x, y, v, theta;
 };

 struct edge {
   float time;
   uint32_t type;
   int16_t x, y, v, theta;
 };

 struct segment {
   Curve c;
   int type;
 };

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
