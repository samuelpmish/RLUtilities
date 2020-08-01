#pragma once

#include <string>

#include "simulation/input.h"
#include "simulation/geometry.h"

#include "misc/color.h"
#include "linear_algebra/math.h"

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#endif

struct Loadout {

  Color primary;
  Color secondary;

  uint16_t car_id;
  uint8_t car_paint_id;
  uint16_t decal_id;
  uint8_t decal_paint_id;
  uint16_t wheels_id;
  uint8_t wheels_paint_id;
  uint16_t boost_id;
  uint8_t boost_paint_id;
  uint16_t antenna_id;
  uint8_t antenna_paint_id;
  uint16_t hat_id;
  uint8_t hat_paint_id;
  uint16_t custom_finish_id;
  uint16_t paint_finish_id;
  uint16_t engine_audio_id;
  uint16_t trails_id;
  uint8_t trails_paint_id;
  uint16_t goal_explosion_id;
  uint8_t goal_explosion_paint_id;

  Loadout() {
    car_id = 23;
    car_paint_id = 12;
    decal_id = 0;
    decal_paint_id = 0;
    wheels_id = 1565;
    wheels_paint_id = 12;
    boost_id = 35;
    boost_paint_id = 7;
    antenna_id = 0;
    antenna_paint_id = 0;
    hat_id = 0;
    hat_paint_id = 0;
    engine_audio_id = 0;
    custom_finish_id = 1681;
    paint_finish_id = 1681;
    trails_id = 3220;
    trails_paint_id = 2;
    goal_explosion_id = 3018;
    goal_explosion_paint_id = 0;
  }

  static Loadout Stocktane(int team) {
    Loadout loadout;
    if (team == 0) {
      loadout.primary = Color{0, 240, 80, 255};
      loadout.secondary = Color{0, 180, 40, 255};
    } else {
      loadout.primary = Color{255, 128, 0, 255};
      loadout.secondary = Color{200, 100, 0, 255};
    }
    return loadout;
  }

};

enum class CarBody {
  Octane,
  Dominus,
  Plank,
  Breakout,
  Hybrid
};

/// What the player's car is currently doing
enum class CarState : uint8_t {
  OnGround,
  Jumped,
  DoubleJumped,
  Dodged,
  InAir,
  Demolished
};

class Car {

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

  vec2 dodge_dir;
  vec3 dodge_torque;

  int frame;
  float time;

  CarBody body;
  CarState state;
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

  void update(Car);

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

