#pragma once

#include "simulation/car.h"
#include "simulation/input.h"

#include "mechanics/reorient.h"

#include "linear_algebra/math.h"

class Dodge {

 public:
  Car & car;

  vec2 direction;
  float jump_duration;
  float delay;

  mat3 preorientation;
  mat3 postorientation;

  bool finished;
  Input controls;

  float timer;

  Dodge(Car & c);

  void step(float dt);

  Car simulate();

  static const float timeout;
  static const float input_threshold;

  static const float z_damping;
  static const float z_damping_start;
  static const float z_damping_end;

  static const float torque_time;
  static const float side_torque;
  static const float forward_torque;

 private:

  Reorient reorient;

};
