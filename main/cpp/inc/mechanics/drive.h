#pragma once

#include "simulation/car.h"
#include "simulation/input.h"
#include "simulation/curve.h"
#include "simulation/unroller.h"

#include "mechanics/Dodge.h"
#include "mechanics/Wavedash.h"
#include "mechanics/Boostdash.h"

#include "linear_algebra/math.h"

#include <list>
#include <vector>

class Drive {

 public:
  Car & car;

  vec3 target;
  std::optional < float > arrival_time;
  std::optional < float > arrival_speed;
  std::optional < vec3 > arrival_tangent;

  Curve path;

  float kp;
  float ki;
  float kd;

  bool finished;
  Input controls;

  Drive(Car & c);

  std::vector < vec3 > debug;

  void step(float dt);
  bool plan_path(vec3, float, float);
  void recalculate_path(bool force = false);

  static const float max_speed;
  static const float max_throttle_speed;
  static const float boost_accel;
  static const float brake_accel;
  static const float coasting_accel;

  static float throttle_accel(float speed);
  static float max_turning_curvature(float speed);
  static float max_turning_speed(float curvature);

 private:

  Unroller transform;

  vec3 prev_target;
  float prev_speed;
  vec3 prev_tangent;

  float T; // time remaining 
  float s;  // the car's location along the curve
  float vf; // the car's current forward velocity

  std::list< float > error_history;

  void steer_controller(float);
  void speed_controller(float);

};
