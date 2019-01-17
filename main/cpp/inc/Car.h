#pragma once

#include "linalg.h"
#include "geometry.h"
#include "RL_structs.h"

#define DELTA_T 0.01666

class Car {

 public:
  vec3 x;
  vec3 v;
  vec3 w;
  mat3 o;
  mat2 o_dodge;

  bool supersonic;
  bool jumped;
  bool double_jumped;
  bool on_ground;

  int boost;

  float jump_timer;
  float dodge_timer;
  bool enable_jump_acceleration;

  vec2 dodge_dir;
  vec3 dodge_torque;

  float time;

  vec3 hitbox_widths;
  vec3 hitbox_offset;

  int team;
  int id;

  Input last;

  mat3 I, invI;

  static const float m; 
  static const float v_max;
  static const float w_max;

  Car();

  void step(Input in = Input(), float dt = DELTA_T);
  void extrapolate(float);

  vec3 forward() const;
  vec3 left() const;
  vec3 up() const;

  obb hitbox() const;

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

