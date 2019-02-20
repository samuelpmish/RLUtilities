#include "mechanics/aerial.h"
#include "mechanics/jump.h"

#include "misc/timer.h"

#include <fstream>
#include <iostream>

const float Aerial::max_speed = 2300.0f;
const float Aerial::boost_accel = 1060.0f;
const float Aerial::throttle_accel = 66.66667f;

const float Aerial::boost_per_second = 30.0f; // ???

const vec3 gravity{0.0f, 0.0f, -650.0f};

Aerial::Aerial(Car & c) : car(c), double_jump(c), turn(c) {
  finished = false;
  controls = Input();

  up = vec3{0.0f, 0.0f, 1.0f};
  angle_threshold = 0.3f;
  reorient_distance = 50.0f;
  throttle_distance = 50.0f;

  double_jump.duration = 0.20f;
  double_jump.delay = 0.25f;

  jumping = car.on_ground;
}

void Aerial::step(float dt) {

  const float j_speed = Jump::speed;
  const float j_accel = Jump::acceleration;
  const float j_duration = Jump::max_duration;

  const vec3 gravity{0.0f, 0.0f, -650.0f};

  float T = arrival_time - car.time;

  vec3 xf = car.x + car.v * T + 0.5 * gravity * T * T;
  vec3 vf = car.v + gravity * T;

  bool jumping_prev = jumping;
  if (jumping) {

    // how much of the jump acceleration time is left
    float tau = j_duration - double_jump.timer;

    // impulse from the first jump
    if (double_jump.timer == 0.0f) {
      vf += car.up() * j_speed;
      xf += car.up() * j_speed * T;
    }

    // acceleration from holding jump
    vf += car.up() * j_accel * tau;
    xf += car.up() * j_accel * tau * (T - 0.5f * tau);

    // impulse from the second jump
    vf += car.up() * j_speed;
    xf += car.up() * j_speed * (T - tau);

    double_jump.step(dt);
    controls.jump = double_jump.controls.jump;

    if (double_jump.timer >= double_jump.delay) {
      jumping = false;
    }

  }

  vec3 delta_x = target - xf;

  vec3 direction = normalize(delta_x);

  if (norm(delta_x) > reorient_distance) {
    turn.target = look_at(delta_x, up);
  } else {
    turn.target = target_orientation.value_or(look_at(target - car.x, up));
  }

  turn.step(dt);

  if (jumping_prev && !jumping) { 
    controls.roll = 0.0f;
    controls.pitch = 0.0f;
    controls.yaw = 0.0f;
  } else {
    controls.roll  = turn.controls.roll;
    controls.pitch = turn.controls.pitch;
    controls.yaw   = turn.controls.yaw;
  }

  // only boost/throttle if we're facing the right direction
  if (angle_between(car.forward(), direction) < angle_threshold) {
    if (norm(delta_x) > throttle_distance) {
      controls.boost = 1;
      controls.throttle = 0.0f;
    } else {
      controls.boost = 0;
      controls.throttle = clip(0.5f * throttle_accel * T * T, 0.0f, 1.0f);
    }
  } else {
    controls.boost = 0;
    controls.throttle = 0.0f;
  }

//  // estimate the time required to turn
//  float total_turn_time = turn.time_estimate();
//  float phi = angle_between(car.o, turn.target);
//
//  // the time when we start boosting (coarse estimate!)
//  float tau_1 = total_turn_time * clip(1.0f - angle_threshold / phi, 0.0f, 1.0f);
//
//  // see if the boost acceleration needed to reach the target is achievable
//  float required_accel = 2.0f * norm(delta_x) / ((T - tau_1) * (T - tau_1));
//
//  float ratio = required_accel / boost_accel;
//
//  // the time when we stop boosting
//  float tau_2 = T - (T - tau_1) * sqrt(1.0f - clip(ratio, 0.0f, 1.0f));
//
//  velocity_estimate = vf + boost_accel * (tau_2 - tau_1) * direction;

  finished = (T <= 0.0f);

  //std::cout << T << " " << controls.jump << " " << car.v << " " << velocity_estimate << std::endl;

  if (fabs(T) < 0.05f) {
    //std::cout << T << " " << target << " " << car.x << std::endl;
  }

}

bool Aerial::is_viable() {

  const float j_speed = Jump::speed;
  const float j_accel = Jump::acceleration;
  const float j_duration = Jump::max_duration;

  float T = arrival_time - car.time;

  vec3 xf = car.x + car.v * T + 0.5 * gravity * T * T;
  vec3 vf = car.v + gravity * T;

  if (car.on_ground) {
    vf += car.up() * (2.0f * j_speed + j_accel * j_duration);
    xf += car.up() * (j_speed * (2.0f * T - j_duration) + 
      j_accel * (T * j_duration - 0.5f * j_duration * j_duration));
  }

  vec3 delta_x = target - xf;

  vec3 f = normalize(delta_x);

  // estimate the time required to turn
  turn.target = look_at(f, up);
  float total_turn_time = turn.time_estimate();
  float phi = angle_between(car.o, turn.target);

  // the time when we start boosting (coarse estimate!)
  float tau_1 = total_turn_time * clip(1.0f - angle_threshold / phi, 0.0f, 1.0f);

  // see if the boost acceleration needed to reach the target is achievable
  float required_accel = 2.0f * norm(delta_x) / ((T - tau_1) * (T - tau_1));

  float ratio = required_accel / boost_accel;

  // the time when we stop boosting
  float tau_2 = T - (T - tau_1) * sqrt(1.0f - clip(ratio, 0.0f, 1.0f));

  velocity_estimate = vf + boost_accel * (tau_2 - tau_1) * f;

  boost_estimate = (tau_2 - tau_1) * 30.0f;

  return (norm(velocity_estimate) < 0.90f * max_speed) && // can't exceed max speed
         (boost_estimate < 0.95f * car.boost) &&          // need enough boost
         (abs(ratio) < 0.90f);                            // need enough time

}

Car Aerial::simulate() {

  timer stopwatch;
  stopwatch.start();

  // make a copy of the car's state and get a pointer to it
  Car car_copy = Car(car);
  Aerial copy = Aerial(car_copy);
  copy.target = target;
  copy.arrival_time = arrival_time;
  copy.target_orientation = target_orientation;
  copy.up = up;
  copy.angle_threshold = angle_threshold;
  copy.reorient_distance = reorient_distance;
  copy.throttle_distance = throttle_distance;

  copy.finished = finished;
  copy.controls = controls;

  float dt = 0.01666f;
  for (float t = dt; t < 5.0f; t += dt) {

    // get the new controls
    copy.step(dt); 

    // and simulate their effect on the car
    car_copy.step(copy.controls, dt); 

    //outfile << " " << car_copy.time << ", ";
    //outfile << " " << car_copy.x[0] << ", ";
    //outfile << " " << car_copy.x[1] << ", ";
    //outfile << " " << car_copy.x[2] << ", ";
    //outfile << " " << car_copy.v[0] << ", ";
    //outfile << " " << car_copy.v[1] << ", ";
    //outfile << " " << car_copy.v[2] << ", ";
    //outfile << " " << car_copy.w[0] << ", ";
    //outfile << " " << car_copy.w[1] << ", ";
    //outfile << " " << car_copy.w[2] << ", ";
    //outfile << " " << copy.controls.boost << "\n";

    if (copy.finished) break;
  }

  //outfile.close();

  stopwatch.stop();
  //std::cout << stopwatch.elapsed() << std::endl;

  return car_copy;
}

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_aerial(pybind11::module & m) {
  pybind11::class_<Aerial>(m, "Aerial")
    .def(pybind11::init<Car &>())
    .def_readwrite("arrival_time", &Aerial::arrival_time)
    .def_readwrite("target", &Aerial::target)
    .def_readwrite("target_orientation", &Aerial::target_orientation)
    .def_readwrite("up", &Aerial::up)
    .def_readwrite("angle_threshold", &Aerial::angle_threshold)
    .def_readwrite("reorient_distance", &Aerial::reorient_distance)
    .def_readwrite("throttle_distance", &Aerial::throttle_distance)
    .def_readonly("finished", &Aerial::finished)
    .def_readonly("controls", &Aerial::controls)
    .def_readonly("velocity_estimate", &Aerial::velocity_estimate)
    .def_readonly("boost_estimate", &Aerial::boost_estimate)
    .def_readonly_static("max_speed", &Aerial::max_speed)
    .def_readonly_static("boost_accel", &Aerial::boost_accel)
    .def_readonly_static("throttle_accel", &Aerial::throttle_accel)
    .def_readonly_static("boost_per_second", &Aerial::boost_per_second)
    .def("step", &Aerial::step)
    .def("is_viable", &Aerial::is_viable)
    .def("simulate", &Aerial::simulate);
}
#endif
