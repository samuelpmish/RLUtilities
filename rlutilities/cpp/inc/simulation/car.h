#pragma once

#include "simulation/input.h"
#include "simulation/geometry.h"

#include "linear_algebra/math.h"

#include <string>

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#endif

class Car {

 public:
  vec3 x;
  vec3 v;
  vec3 w;
  mat3 o;
  mat2 o_dodge;

  vec4 q; // quaternion
  vec3 rotator;

  bool supersonic;
  bool jumped;
  bool double_jumped;
  bool on_ground;

  int boost;

  float jump_timer;
  float dodge_timer;
  float boost_timer;
  bool enable_jump_acceleration;

  vec2 dodge_dir;
  vec3 dodge_torque;

  float time;

  vec3 hitbox_widths;
  vec3 hitbox_offset;

  int team;
  int id;

  Input controls;

  mat3 I, invI;

  static const float m;
  static const float v_max;
  static const float w_max;

  Car();

  void step(Input in = Input(), float dt = 0.008333f);
  void extrapolate(float);

  std::string to_json();

  void update(Car);

  vec3 forward() const;
  vec3 left() const;
  vec3 up() const;

  obb hitbox() const;

  #ifdef GENERATE_PYTHON_BINDINGS
  void read_game_car(pybind11::object game_car);
  #endif

 private:

  void jump(const Input& in, float dt);
  void air_dodge(const Input& in, float dt);
  void aerial_control(const Input& in, float dt);

  float drive_force_left(const Input& in);
  float drive_force_forward(const Input& in);
  float drive_torque_up(const Input& in);
  void driving(const Input& in, float dt);

  void driving_handbrake(const Input& in, float dt);

  void check_collision();

};

