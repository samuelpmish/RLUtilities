#pragma once

#include "simulation/car.h"
#include "simulation/geometry.h"

#include "linear_algebra/math.h"

#include <pybind11/pybind11.h>


class Ball {
 public:
  vec3 x;
  vec3 v;
  vec3 w;
  float t;

  static const float m; // mass

  static const float drag; // viscous damping
  static const float mu; // coefficient of friction
  static const float restitution; // coefficient of restitution

  static const float v_max; // maximum velocity
  static const float w_max; // maximum angular velocity

  static const float soccar_radius;
  static const float hoops_radius;
  static const float dropshot_radius;
  static const float soccar_collision_radius;
  static const float hoops_collision_radius;
  static const float dropshot_collision_radius;

  static float radius;
  static float collision_radius;
  static float I; // moment of inertia

  Ball();

  sphere hitbox();
  void step(float dt);
  void step(float dt, const Car & c);

  void read_game_ball(pybind11::object game_ball);
};
