#pragma once

#include "simulation/car.h"
#include "simulation/input.h"

#include "mechanics/dodge.h"
#include "mechanics/reorient.h"

#include "linear_algebra/math.h"

#include <string>
#include <optional>

class Aerial {

 public:

  Car & car;

  // required
  float arrival_time;
  vec3 target_position;

  // optional arguments to tweak behavior
  vec3 up;
  bool double_jump; 
  mat3 target_orientation;

  // parameters that govern the controller
  float angle_threshold;
  float reorient_distance;
  float throttle_distance;
  
  Input controls;
  bool finished;

  Aerial(Car & c);

  void step(float dt);

  bool is_viable();

  Car simulate();

  static const float max_speed;
  static const float boost_accel;
  static const float throttle_accel;
  static const float boost_per_second;

 private:

  bool jumping;

  Dodge dodge;
  Reorient reorient;

};
