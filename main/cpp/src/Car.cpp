#include "Car.h"

#include "Jump.h"
#include "Dodge.h"
#include "Aerial.h"

const float Car::m = 180.0f;
const float Car::v_max = 2300.0f;
const float Car::w_max = 5.5f;

const vec3 g = {0.0f, 0.0f, -650.0f};

vec3 Car::forward() const { return vec3{o(0, 0), o(1, 0), o(2, 0)}; }
vec3 Car::left() const { return vec3{o(0, 1), o(1, 1), o(2, 1)}; }
vec3 Car::up() const { return vec3{o(0, 2), o(1, 2), o(2, 2)}; }

void Car::jump(const Input& in, float dt) {

  v += g * dt + Jump::speed * up();
  x += v * dt;

  o = dot(axis_rotation(w * dt), o);

  jump_timer = 0.0f;
  jumped = true;
  double_jumped = false;
  enable_jump_acceleration = true;
  on_ground = false;

}

void Car::air_dodge(const Input& in, float dt) {

  // directional dodge
  if (fabs(in.pitch) + fabs(in.roll) + fabs(in.yaw) >= Dodge::input_threshold) {

    float vf = dot(v, forward());
    float s = fabs(vf) / v_max;

    dodge_dir = normalize(vec2{-in.pitch, in.yaw});

    vec3 dodge_torque_local = vec3(cross(vec2{dodge_dir[0] * 224.0f, dodge_dir[1] * 260.0f}));
    dodge_torque = dot(o, dodge_torque_local);

    if (fabs(dodge_dir[0]) < 0.1f) dodge_dir[0] = 0.0f;
    if (fabs(dodge_dir[1]) < 0.1f) dodge_dir[1] = 0.0f;

    bool backward_dodge;

    if (fabs(vf) < 100.0f) {
      backward_dodge = dodge_dir[0] < 0.0f;
    } else {
      backward_dodge = (dodge_dir[0] >= 0.0f) != (vf > 0.0f);
    }

    vec2 dv = 500.0f * dodge_dir;

    if (backward_dodge) {
      dv[0] *= (16.0f / 15.0f) * (1.0f + 1.5f * s);
    }
    dv[1] *= (1.0f + 0.9f * s);

    v += g * dt + vec3(dot(o_dodge, dv));
    x += v * dt;

    w += dodge_torque * dt;
    o = dot(axis_rotation(w * dt), o);

    double_jumped = true;
    dodge_timer = 0.0f;

  // double jump
  } else {

    dodge_torque = vec3{0.0f, 0.0f, 0.0f};

    v += g * dt + Jump::speed * up();
    x += v * dt;

    w += dodge_torque * dt;
    o = dot(axis_rotation(w * dt), o);

    double_jumped = true;
    dodge_timer = 1.01f * Dodge::torque_time;

  }

}

void Car::aerial_control(const Input& in, float dt) {

  const float J = 10.5f; // ?

  // air control torque coefficients
  const vec3 T{-400.0f, -130.0f, 95.0f};

  // air damping torque coefficients
  const vec3 H{-50.0f, -30.0f * (1.0f - fabs(in.pitch)),
               -20.0f * (1.0f - fabs(in.yaw))};

  vec3 rpy{in.roll, in.pitch, in.yaw};

  float thrust = 0.0;

  if (in.boost && boost > 0) {
    v += (Aerial::boost_accel + Aerial::throttle_accel) * forward() * dt;
    boost--;
  } else {
    v += in.throttle * Aerial::throttle_accel * forward() * dt;
  }

  if (in.jump && enable_jump_acceleration) {
    if (jump_timer < Jump::min_duration) {
      v += (0.75f * Jump::acceleration * up() - 510.0f * forward()) * dt;
    } else {
      v += Jump::acceleration * up() * dt;
    }
  }

  if (dodge_timer >= Dodge::z_damping_start && (v[2] < 0.0f || dodge_timer < Dodge::z_damping_end)) {
    v += vec3{0.0f, 0.0f, -v[2] * Dodge::z_damping};
  }

  if (0.0f <= dodge_timer && dodge_timer <= 0.3f) {
    rpy[1] = 0.0f;
  }

  if (0.0f <= dodge_timer && dodge_timer <= Dodge::torque_time) {
    w += dodge_torque * dt;
  } else {
    vec3 w_local = dot(w, o);
    w += dot(o, T * rpy + H * w_local) * (dt / J);
  }

  v += g * dt;

  x += v * dt;
  o = dot(axis_rotation(w * dt), o);

}

float Car::drive_force_forward(const Input& in) {
#if 0
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
          return Aerial::boost_accel + turn_damping;

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
#else
  return 0.0f;
#endif
}

float Car::drive_force_left(const Input& in) {
#if 0
  const float v_f = dot(v, forward());
  const float v_l = dot(v, left());
  const float w_u = dot(w, up());

  return (1380.4531378f * in.steer + 7.8281188f * in.throttle -
          15.0064029f * v_l + 668.1208332f * w_u) *
         (1.0f - exp(-0.001161f * fabs(v_f)));
#else
  return 0.0f;
#endif
}

float Car::drive_torque_up(const Input& in) {
#if 0
  float v_f = dot(v, forward());
  float w_u = dot(w, up());

  return 15.0f * (in.steer * max_curvature(fabs(v_f)) * v_f - w_u);
#else
  return 0.0f;
#endif
}

void Car::driving(const Input& in, float dt) {
#if 0
  // in-plane forces
  vec3 force =
      drive_force_forward(in) * forward() + drive_force_left(in) * left();

  // out-of-plane torque
  vec3 torque = drive_torque_up(in) * up();

  v += force * dt;
  x += v * dt;

  w += torque * dt;
  o = dot(axis_rotation(w * dt), o);
#endif
}

void Car::driving_handbrake(const Input& in, float dt) {
  // TODO
}

void Car::step(Input in, float dt) {

  // driving
  if (on_ground) {

    if (in.jump == 1) {
      std::cout << time << " Jump" << std::endl;
      jump(in, dt);
    } else {
      if (in.slide == 0) {
        driving(in, dt);
      } else {
        driving_handbrake(in, dt);
      }
    }

  // in the air
  } else {
    if (in.jump == 1 && 
        last.jump == 0 && 
        jump_timer < Dodge::timeout && 
        double_jumped == false) {
      std::cout << time << " Air Dodge" << std::endl;
      air_dodge(in, dt);
    } else {
      //std::cout << "Aerial Control" << std::endl;
      aerial_control(in, dt);
    }
  }

  // if the velocities exceed their maximum values, scale them back
  v /= fmaxf(1.0f, norm(v) / v_max);
  w /= fmaxf(1.0f, norm(w) / w_max);

  time += dt;

  if (dodge_timer >= 0.0f) {
    if (dodge_timer >= Dodge::torque_time || on_ground) {
      dodge_timer = -1.0f;
    } else {
      dodge_timer += dt;
    }
  }

  if (jump_timer >= 0.0f) {
    if (on_ground) {
      jump_timer = -1.0f;
    } else {
      jump_timer += dt;
    }
  }

  if ((in.jump == 0) || jump_timer > Jump::max_duration) {
    enable_jump_acceleration = false;
  }

  last = in;
}

obb Car::hitbox() const {
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

  I = m * mat3{{751.0f,    0.0f,    0.0f},
               {  0.0f, 1334.0f,    0.0f},
               {  0.0f,    0.0f, 1836.0f}};
  invI = inv(I);

  jump_timer = -1.0f;
  dodge_timer = -1.0f;
  enable_jump_acceleration = false;

  hitbox_widths = vec3{59.00368881f, 42.09970474f, 18.07953644f};
  hitbox_offset = vec3{13.97565993f, 0.0f, 20.75498772f};

  last = Input();
}

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
void init_car(pybind11::module & m) {
  pybind11::class_<Car>(m, "Car")
    .def(pybind11::init<>())
    .def(pybind11::init<const Car &>())
    .def("step", &Car::step)
    .def("hitbox", &Car::hitbox)
    .def("extrapolate", &Car::extrapolate)
    .def("forward", &Car::forward)
    .def("left", &Car::left)
    .def("up", &Car::up)
    .def_readwrite("time", &Car::time)
    .def_readwrite("id", &Car::id)
    .def_readwrite("team", &Car::team)
    .def_readwrite("pos", &Car::x)
    .def_readwrite("vel", &Car::v)
    .def_readwrite("omega", &Car::w)
    .def_readwrite("theta", &Car::o)
    .def_readwrite("theta_dodge", &Car::o_dodge)
    .def_readwrite("supersonic", &Car::supersonic)
    .def_readwrite("jumped", &Car::jumped)
    .def_readwrite("double_jumped", &Car::double_jumped)
    .def_readwrite("on_ground", &Car::on_ground)
    .def_readwrite("jump_timer", &Car::jump_timer)
    .def_readwrite("dodge_timer", &Car::dodge_timer)
    .def_readwrite("boost", &Car::boost)
    .def_readwrite("last_input", &Car::last);

  pybind11::class_<Input>(m, "Input")
    .def(pybind11::init<>())
    .def_readwrite("steer", &Input::steer)
    .def_readwrite("roll", &Input::roll)
    .def_readwrite("pitch", &Input::pitch)
    .def_readwrite("yaw", &Input::yaw)
    .def_readwrite("throttle", &Input::throttle)
    .def_readwrite("jump", &Input::jump)
    .def_readwrite("boost", &Input::boost)
    .def_readwrite("slide", &Input::slide)
	  .def_readwrite("handbrake", &Input::handbrake);
}
#endif
