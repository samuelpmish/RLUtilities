#pragma once

#include "simulation/car.h"
#include "simulation/input.h"
#include "simulation/curve.h"
#include "simulation/unroller.h"

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

  float arrival_accel;
  float expected_error;
  float expected_speed;

  FollowPath(Car & c);

  void step(float dt);

 private:

  Drive drive;
  Dodge dodge;
  Wavedash dash;

  // this function simulates the execution of a 
  // specific speed plan, and returns how far away
  // from the the car is from its destination, 
  // at the arrival time
  float distance_error(float s, float T, float dt, float v0, float vT, float aT);

  // this function calls distance_error() repeatedly
  // to find a speed plan that completes the path in
  // in the appropriate amount of time, while also 
  // satisfying the arrival speed constraint
  float determine_speed_plan(float s, float T, float dt);

};
