#pragma once

#include "misc/interpolation.h"

#include "simulation/car.h"
#include "simulation/input.h"
#include "simulation/curve.h"

#include "mechanics/drive.h"
#include "mechanics/dodge.h"
#include "mechanics/wavedash.h"
#include "mechanics/boostdash.h"

#include "linear_algebra/math.h"

#include <array>
#include <vector>

class FollowPath {

 public:
  Car & car;

  float arrival_time;
  float arrival_speed;

  Curve path;

  bool finished;
  Input controls;

  Interpolation< vec2 > plan;

  FollowPath(Car & c);

  void calculate_plan(Curve, float, float);
  void step(float dt);

 private:

  Drive drive;
  Dodge dodge;
  Wavedash dash;

  // this function simulates the execution of a 
  // specific speed plan, and returns how far away
  // from the the car is from its destination, 
  // at the arrival time
  float distance_error(float s, float T, float v0, float vT, float aT, int num_steps);

};
