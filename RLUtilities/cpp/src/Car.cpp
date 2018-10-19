#include "Car.h"

const float g = -650.0f;
const float w_max = 5.5f;
const float boost_accel = 1000.0f;
const float dodge_limit = 0.5f;  // ?

Pitch Car::env;

float max_curvature(float speed) {
  const int n = 6;
  float values[n][2] = {{0.0f, 0.00690f},    {500.0f, 0.00398f},
                        {1000.0f, 0.00235f}, {1500.0f, 0.00138f},
                        {1750.0f, 0.00110f}, {2500.0f, 0.00080f}};

  float input = clamp(speed, 0.0f, 2500.0f);

  for (int i = 0; i < (n - 1); i++) {
    if (values[i][0] <= input && input < values[i + 1][0]) {
      float u = (input - values[i][0]) / (values[i + 1][0] - values[i][0]);
      return lerp(values[i][1], values[i + 1][1], u);
    }
  }

  return -1.0f;
}

float max_speed(float curvature) {
  const int n = 6;
  float values[n][2] = {{0.0f, 0.00690f},    {500.0f, 0.00398f},
                        {1000.0f, 0.00235f}, {1500.0f, 0.00138f},
                        {1750.0f, 0.00110f}, {2500.0f, 0.00080f}};

  float input = clamp(curvature, 0.00080f, 0.00690f);

  for (int i = 0; i < (n - 1); i++) {
    if (values[i][1] <= input && input < values[i + 1][1]) {
      float u = (input - values[i][1]) / (values[i + 1][1] - values[i][1]);
      return clamp(lerp(values[i][0], values[i + 1][0], u), 0.0f, 2300.0f);
    }
  }

  return -1.0f;
}

vec3 Car::forward() { return vec3{o(0, 0), o(1, 0), o(2, 0)}; }
vec3 Car::left() { return vec3{o(0, 1), o(1, 1), o(2, 1)}; }
vec3 Car::up() { return vec3{o(0, 2), o(1, 2), o(2, 2)}; }

void Car::jump(const Input& in, float dt) {
  // TODO
}

void Car::air_dodge(const Input& in, float dt) {
  // TODO
}

void Car::aerial_control(const Input& in, float dt) {
  const float M = 180.0f;  // mass
  const float J = 10.5f;   // moment of inertia
  const float v_max = 2300.0f;
  const float w_max = 5.5f;
  const float boost_force = 178500.0f;
  const float throttle_force = 12000.0f;
  const vec3 g = {0.0f, 0.0f, -651.47f};

  const vec3 rpy{in.roll, in.pitch, in.yaw};

  // air control torque coefficients
  const vec3 T{-400.0f, -130.0f, 95.0f};

  // air damping torque coefficients
  const vec3 H{-50.0f, -30.0f * (1.0f - fabs(in.pitch)),
               -20.0f * (1.0f - fabs(in.yaw))};

  float thrust = 0.0;

  if (in.boost && boost > 0) {
    thrust = (boost_force + throttle_force);
    boost--;
  } else {
    thrust = in.throttle * throttle_force;
  }

  v += (g + (thrust / M) * forward()) * dt;
  x += v * dt;

  vec3 w_local = dot(w, o);

  vec3 old_w = w;
  w += dot(o, T * rpy + H * w_local) * (dt / J);

  mat3 R = axis_rotation(0.5f * (w + old_w) * dt);

  o = dot(axis_rotation(0.5f * (w + old_w) * dt), o);

  // if the velocities exceed their maximum values, scale them back
  v /= fmaxf(1.0f, norm(v) / v_max);
  w /= fmaxf(1.0f, norm(w) / w_max);
}

float Car::drive_force_forward(const Input& in) {
  constexpr float driving_speed = 1450.0f;
  constexpr float braking_force = -3500.0f;
  constexpr float coasting_force = -525.0f;
  constexpr float throttle_threshold = 0.05f;
  constexpr float throttle_force = 1550.0f;
  constexpr float max_speed = 2275.0f;
  constexpr float min_speed = 10.0f;
  constexpr float boost_force = 1500.0f;
  constexpr float steering_torque = 25.75f;
  constexpr float braking_threshold = -0.001f;
  constexpr float supersonic_turn_drag = -98.25;  // ?

  const float v_f = dot(v, forward());
  const float v_l = dot(v, left());
  const float w_u = dot(w, up());

  const float dir = sgn(v_f);
  const float speed = fabs(v_f);

  const float turn_damping =
      (-0.07186693033945346f * fabs(in.steer) +
       -0.05545323728191764f * fabs(w_u) + 0.00062552963716722f * fabs(v_l)) *
      v_f;

  // boosting
  if (in.boost == 1) {
    if (v_f < 0.0f) {
      return -braking_force;

    } else {
      if (v_f < driving_speed) {
        return max_speed - v_f;

      } else {
        if (v_f < max_speed) {
          return boost_accel + turn_damping;

        } else {
          // ?
          return supersonic_turn_drag * fabs(w_u);
          // ?
        }
      }
    }

    // not boosting
  } else {
    // braking
    if ((in.throttle * sgn(v_f) <= braking_threshold) &&
        fabs(v_f) > min_speed) {
      return braking_force * sgn(v_f);

      // not braking
    } else {
      // coasting
      if (fabs(in.throttle) < throttle_threshold && fabs(v_f) > min_speed) {
        return coasting_force * sgn(v_f) + turn_damping;

        // accelerating
      } else {
        if (fabs(v_f) > driving_speed) {
          return turn_damping;

        } else {
          return in.throttle * (throttle_force - fabs(v_f)) + turn_damping;
        }
      }
    }
  }
}

float Car::drive_force_left(const Input& in) {
  const float v_f = dot(v, forward());
  const float v_l = dot(v, left());
  const float w_u = dot(w, up());

  return (1380.4531378f * in.steer + 7.8281188f * in.throttle -
          15.0064029f * v_l + 668.1208332f * w_u) *
         (1.0f - exp(-0.001161f * fabs(v_f)));
}

float Car::drive_torque_up(const Input& in) {
  float v_f = dot(v, forward());
  float w_u = dot(w, up());

  return 15.0f * (in.steer * max_curvature(fabs(v_f)) * v_f - w_u);
}

void Car::driving(const Input& in, float dt) {
  // in-plane forces
  vec3 force =
      drive_force_forward(in) * forward() + drive_force_left(in) * left();

  // out-of-plane torque
  vec3 torque = drive_torque_up(in) * up();

  v += force * dt;
  x += v * dt;

  w += torque * dt;
  o = dot(axis_rotation(w * dt), o);
}

void Car::driving_handbrake(const Input& in, float dt) {
  // TODO
}

void Car::step(Input in, float dt) {
  // driving
  if (on_ground) {
    if (in.jump == 1) {
      // std::cout << "Jump" << std::endl;
      jump(in, dt);

    } else {
      if (in.slide == 0) {
        // std::cout << "Driving" << std::endl;
        driving(in, dt);

      } else {
        // std::cout << "Driving (handbrake)" << std::endl;
        driving_handbrake(in, dt);
      }
    }

    // in the air
  } else {
    if (in.jump == 1 && dodge_timer < dodge_limit && can_dodge) {
      // std::cout << "Air Dodge" << std::endl;
      air_dodge(in, dt);

    } else {
      // std::cout << "Aerial Control" << std::endl;
      aerial_control(in, dt);
    }
  }
}

vec3 Car::pitch_surface_normal() {
  if (env.in_contact_with(sphere{x, 40.0})) {
    return env.last_contact_info().direction;
  } else {
    return vec3{0.0f, 0.0f, 0.0f};
  }
}

obb Car::hitbox() {
  obb box;
  box.orientation = o;
  box.half_width = hitbox_widths;
  box.center = dot(o, hitbox_offset) + x;
  return box;
}

void Car::extrapolate(float dt) { step(last, dt); }

Car::Car() {
  x = vec3{0.0f, 0.0f, 0.0f};
  v = vec3{0.0f, 0.0f, 0.0f};
  w = vec3{0.0f, 0.0f, 0.0f};
  o = eye<3>();

  supersonic = false;
  jumped = false;
  double_jumped = false;
  on_ground = false;

  time = 0.0f;

  can_dodge = false;
  dodge_timer = 0.0f;

  //hitbox_widths = vec3{59.00368881f, 42.09970474f, 18.07953644f};
  hitbox_widths = vec3{70.00368881f, 50.09970474f, 24.07953644f};
  hitbox_offset = vec3{13.97565993f, 0.0f, 20.75498772f};

  last = Input();
}
