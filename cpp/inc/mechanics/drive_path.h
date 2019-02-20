#pragma once

#include "simulation/car.h"
#include "simulation/path.h"
#include "simulation/input.h"
#include "simulation/curve.h"
#include "simulation/unroller.h"

#include "mechanics/drive.h"
#include "mechanics/dodge.h"
#include "mechanics/wavedash.h"
#include "mechanics/boostdash.h"

#include "linear_algebra/math.h"

#include <list>
#include <vector>

class DrivePath {

 public:
  Car & car;

  vec3 target;
  std::optional < float > arrival_time;
  std::optional < float > arrival_speed;
  std::optional < vec3 > arrival_tangent;

  Path path;

  bool finished;
  Input controls;

  DrivePath(Car & c);

  void step(float dt);
  bool plan_path(vec3, float, float);
  void recalculate_path(bool force = false);
  float time_to_traverse(const Curve &);

 private:

  Drive drive;
  Dodge dodge;
  Wavedash dash;

};
