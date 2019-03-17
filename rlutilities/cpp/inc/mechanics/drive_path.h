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

class DrivePath {

 public:
  Car & car;

  vec3 target;
  float arrival_time;
  float arrival_speed;
  vec3 arrival_tangent;

  Curve path;

  bool finished;
  Input controls;

  float arrival_accel;
  float expected_error;
  float expected_speed;

  DrivePath(Car & c);

  void step(float dt);
  void recalculate_path();
  float accel_buffer();

  static float scale;
  static int nx, ntheta, nv;

  static uint32_t to_id(int, int, int, int);
  static std::array < int16_t, 4 > from_id(uint32_t);

  static void read_files(std::string directory);

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

  static std::array < int, 3 > strides;
  static std::vector < float > time_LUT;
  static std::vector < uint32_t > path_LUT;

};
