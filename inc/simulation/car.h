#pragma once

#include <string>

#include "misc/json.h"
#include "misc/rlu_dll.h"

#include "simulation/input.h"
#include "simulation/geometry.h"

#include "linear_algebra/math.h"


#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#endif


class Car {

 enum class State {
   OnGround=0,
   Jump=1,
   Dodge=2,
   DoubleJump=3,
   Aerial=4,
   Demolished=5
 };

 public:
  vec3 position;
  vec3 velocity;
  vec3 angular_velocity;
  mat3 orientation;
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

  State state;

  vec2 dodge_dir;
  vec3 dodge_torque;

  float time;

  vec3 hitbox_widths;
  vec3 hitbox_offset;

  int team;
  int id;

  Input controls;

  mat3 I, invI;

  RLU_DLL static const float m;
  RLU_DLL static const float v_max;
  RLU_DLL static const float w_max;

  RLU_DLL Car();

  RLU_DLL void step(Input in = Input(), float dt = 0.008333f);
  RLU_DLL void extrapolate(float);

  RLU_DLL void update(Car);

  RLU_DLL vec3 forward() const;
  RLU_DLL vec3 left() const;
  RLU_DLL vec3 up() const;

  RLU_DLL obb hitbox() const;

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

