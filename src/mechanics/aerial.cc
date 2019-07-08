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

Aerial::Aerial(Car & c) : car(c), dodge(c), reorient(c) {
  finished = false;
  controls = Input();

  up = vec3{0.0f, 0.0f, 1.0f};
  angle_threshold = 0.3f;
  reorient_distance = 50.0f;
  throttle_distance = 50.0f;

  double_jump = true;
  dodge.jump_duration = 0.20f;
  dodge.delay = 0.25f;

  jumping = car.on_ground;
}

void Aerial::step(float dt) {

  const float j_speed = Jump::speed;
  const float j_accel = Jump::acceleration;
  const float j_duration = Jump::max_duration;

  const vec3 gravity{0.0f, 0.0f, -650.0f};

  float T = arrival_time - car.time;

  vec3 xf = car.position + car.velocity * T + 0.5 * gravity * T * T;
  vec3 vf = car.velocity + gravity * T;

  bool jumping_prev = jumping;
  if (jumping) {

    // how much of the jump acceleration time is left
    float tau = j_duration - dodge.timer;

    // impulse from the first jump
    if (dodge.timer == 0.0f) {
      vf += car.up() * j_speed;
      xf += car.up() * j_speed * T;
    }

    // acceleration from holding jump
    vf += car.up() * j_accel * tau;
    xf += car.up() * j_accel * tau * (T - 0.5f * tau);

    // impulse from the second jump
    vf += car.up() * j_speed;
    xf += car.up() * j_speed * (T - tau);

    dodge.step(dt);
    controls.jump = dodge.controls.jump;

    if (dodge.timer >= dodge.delay) {
      jumping = false;
    }

  } else {

    controls.jump = false;
    
  }

  vec3 delta_x = target_position - xf;

  vec3 direction = normalize(delta_x);

  if (norm(delta_x) > reorient_distance) {
    reorient.target_orientation = look_at(delta_x, up);
  } else {
    if (fabs(det(target_orientation) - 1.0f) < 0.01f) {
      reorient.target_orientation = look_at(target_position - car.position, up);
    } else {
      reorient.target_orientation = target_orientation;
    }
  }

  reorient.step(dt);

  if (jumping_prev && !jumping) { 
    controls.roll = 0.0f;
    controls.pitch = 0.0f;
    controls.yaw = 0.0f;
  } else {
    controls.roll  = reorient.controls.roll;
    controls.pitch = reorient.controls.pitch;
    controls.yaw   = reorient.controls.yaw;
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

  finished = (T <= 0.0f);

  //if (fabs(T) < 0.05f) {
  //  std::cout << T << " " << target - car.x << std::endl;
  //}

}

bool Aerial::is_viable() {

  const float j_speed = Jump::speed;
  const float j_accel = Jump::acceleration;
  const float j_duration = Jump::max_duration;

  float T = arrival_time - car.time;

  vec3 xf = car.position + car.velocity * T + 0.5 * gravity * T * T;
  vec3 vf = car.velocity + gravity * T;

  if (car.on_ground) {
    vf += car.up() * (2.0f * j_speed + j_accel * j_duration);
    xf += car.up() * (j_speed * (2.0f * T - j_duration) + 
      j_accel * (T * j_duration - 0.5f * j_duration * j_duration));
  }

  vec3 delta_x = target_position - xf;

  vec3 f = normalize(delta_x);

  // estimate the time required to turn
  reorient.target_orientation = look_at(f, up);
  float total_turn_time = reorient.simulate().time - car.time;
  float phi = angle_between(car.orientation, reorient.target_orientation);

  // the time when we start boosting (coarse estimate!)
  float tau_1 = total_turn_time * clip(1.0f - angle_threshold / phi, 0.0f, 1.0f);

  // see if the boost acceleration needed to reach the target is achievable
  float required_accel = 2.0f * norm(delta_x) / ((T - tau_1) * (T - tau_1));

  float ratio = required_accel / boost_accel;

  // the time when we stop boosting
  float tau_2 = T - (T - tau_1) * sqrt(1.0f - clip(ratio, 0.0f, 1.0f));

  vec3 velocity_estimate = vf + boost_accel * (tau_2 - tau_1) * f;

  float boost_estimate = (tau_2 - tau_1) * 30.0f;

  return (norm(velocity_estimate) < 0.90f * max_speed) && // can't exceed max speed
         (boost_estimate < 0.95f * car.boost) &&          // need enough boost
         (abs(ratio) < 0.90f);                            // need enough time

}

Car Aerial::simulate() {

  // make a copy of the car's state and get a pointer to it
  Car car_copy = Car(car);
  Aerial copy = Aerial(car_copy);
  copy.target_position = target_position;
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

    if (copy.finished) break;
  }

  return car_copy;
}
