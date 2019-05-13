#include "mechanics/follow_path.h"

#include "misc/io.h"
#include "misc/timer.h"

#include <math.h>
#include <string>
#include <fstream>
#include <algorithm>

FollowPath::FollowPath(Car & c) : car(c), drive(c), dodge(c), dash(c) {
  finished = false;
  controls = Input();

  path = Curve();

  arrival_time = NAN;
  arrival_speed = NAN;
}

#define printout(x) std::cout << #x ": " << x << std::endl;

void FollowPath::step(float dt) {

  if (path.length < 0.0f) {

    std::cout << "invalid path!" << std::endl;

  } else {
    
    if (path.max_speeds.size() == 0) {
      path.calculate_max_speeds(Drive::max_speed, Drive::max_speed);
    }

    float t_react = 0.3f;

    printout(path.length);

    float speed = dot(car.v, car.forward());

    printout(speed);

    float s = path.find_nearest(car.x);

    printout(s);

    float s_ahead = s - fmaxf(norm(car.v), 500.0f) * t_react;

    drive.target = path.point_at(s_ahead);

    printout(drive.target);
    
    if (isnormal(arrival_time)) {

      float T_min = 0.008f;
      float T = fmaxf(arrival_time - car.time, T_min);

      if (isnormal(arrival_speed)) {

        drive.speed = determine_speed_plan(s, T, dt);

      } else {

        drive.speed = s / T;

      }

      finished = (T <= T_min);

    } else {

      if (isnormal(arrival_speed)) {

        drive.speed = arrival_speed;

      } else {

        drive.speed = Drive::max_throttle_speed;

      }

      finished = (s <= 50.0f);

    }

    drive.speed = fminf(drive.speed, path.max_speed_at(s - 4.0f * speed * dt));

    printout(drive.speed);

    drive.step(dt);
    controls = drive.controls;

    printout(controls.throttle);
    printout(controls.steer);
    std::cout << std::endl;

  }

}

// evaluates the quadratic function v(t)
// satisfying v(0) == v0, v(T) == vT, v'(T) == aT
float interpolate_quadratic(float v0, float vT, float aT, float t, float T) {
  float tau = t / T;
  float dv = aT * T;
  return v0 * (tau - 1.0f) * (tau - 1.0f) +
         dv * (tau - 1.0f) * tau +
         vT * (2.0f - tau) * tau;
}

float FollowPath::distance_error(float s0, float T, float dt, float v0, float vT, float aT) {

  int num_steps = int(T / dt);
  float s = s0;
  float v = v0;
  float v_prev = v0;

  for (int i = 0; i < num_steps; i++) {

    float t = (float(i) / float(num_steps)) * T;
    float v_ideal = interpolate_quadratic(v0, vT, aT, t, T);
    float v_limit = path.max_speed_at(s - v_prev * dt);

    v_prev = v;
    v = fminf(v_ideal, v_limit);

    s -= 0.5f * (v + v_prev) * dt;

  }

  return s;

}

float FollowPath::determine_speed_plan(float s, float T, float dt) {

  float v0 = norm(car.v);
  float vf = arrival_speed;

  float a = Drive::boost_accel + Drive::throttle_accel(vf);
  float error = distance_error(s, T, dt, v0, vf, a);

  float a_old = -Drive::brake_accel;
  float error_old = distance_error(s, T, dt, v0, vf, a_old);

  // try to find the right arrival acceleration
  // using a few iterations of secant method
  for (int i = 0; i < 16; i++) {

    if (fabs(error) < 0.5f) break;

    float new_a = (a_old * error - a * error_old) / (error - error_old);

    a_old = a;
    a = new_a;

    error_old = error;
    error = distance_error(s, T, dt, v0, vf, a);

  }

  expected_error = error;

  expected_speed = interpolate_quadratic(v0, vf, a, drive.reaction_time, T);

  return expected_speed;

}
