#pragma once

#include "simulation/car.h"
#include "simulation/input.h"

#include "linear_algebra/math.h"

class Reorient {

 public:
  Car & car;

  mat3 target_orientation;
  float eps_phi;
  float eps_omega;
  float horizon_time;

  bool finished;
  Input controls;

  vec3 alpha;

  Reorient(Car & c);

  void step(float dt);

	Car simulate();

  static const float scale;
  static const vec3 angular_acceleration;
  static const vec3 angular_damping;

 private:

  vec3 phi;
  vec3 dphi_dt;
  vec3 omega;
  vec3 omega_local;
  mat3 Z0;
  mat3 theta;

  mat3 target_orientation_prev;

  mat3 Z(const vec3 & q);
  vec3 G(const vec3 & q, const vec3 & dq_dt);
  vec3 f(const vec3 & alpha, const float dt);

  vec3 find_controls_for(const vec3 & ideal_alpha);

};
