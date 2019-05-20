#pragma once

#include "simulation/car.h"
#include "simulation/input.h"

#include "mechanics/dodge.h"
#include "mechanics/aerial_turn.h"

#include "linear_algebra/math.h"

#include <string>
#include <optional>

class Aerial {

 public:

  Car & car;

  float arrival_time;
  vec3 target;
  std::optional < mat3 > target_orientation;
  vec3 up;
  float angle_threshold;
  float reorient_distance;
  float throttle_distance;

  Input controls;
  bool finished;
  float boost_estimate;
  vec3 velocity_estimate;

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

  Dodge double_jump;
  AerialTurn turn;

};
