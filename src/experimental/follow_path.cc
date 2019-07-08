#include "experimental/follow_path.h"

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

void FollowPath::step(float dt) {
    
  // these values have been tuned to work best with Drive's implementation
  float steer_reaction_time = 0.25f; 
  float accel_reaction_time = 0.05f;

  float speed = dot(car.velocity, car.forward());
  float T = fmaxf(arrival_time - car.time, 0.008f);

  float s = path.find_nearest(car.position);
  float lookahead = fmaxf(norm(car.velocity), 500.0f) * steer_reaction_time; 

  drive.target = path.point_at(s - lookahead);

  float v = norm(car.velocity);

  vec2 future = plan(T - accel_reaction_time);

  float sf = future[0];
  float vf = future[1];

  float v_avg = (s - sf) / fmin(T, accel_reaction_time);

  // this is the speed that minimizes the combined
  // error in expected position and expected speed
  drive.speed = (2.0f * v_avg + 4.0f * vf - v) / 5.0f;
  //drive.speed = v_avg;

  drive.step(dt);
  controls = drive.controls;

  finished = (T <= 0.0f);

}

#if 0
// evaluates the quadratic function v(t)
// satisfying v(0) == v0, v'(0) = a0, v(T) == vT
float interpolate_quadratic(float v0, float a0, float vT, float t, float T) {
  float tau = t / T;
  float dv = a0 * t;
  return v0 * (1.0f - tau * tau) + 
         dv * (1.0f - tau) + 
         vT * tau * tau;
}
#else
// evaluates the quadratic function v(t)
// satisfying v(0) == v0, v'(0) = 0, v(T) == vT, v'(T) = aT
float interpolate_quadratic(float v0, float aT, float vT, float t, float T) {
  float tau = t / T;
  float dv = aT * t;
  return v0 * (tau - 1.0f) * (tau - 1.0f) * (1.0f + 2.0 * tau) + 
         vT * (3.0f - 2.0f * tau) * tau * tau + 
         dv * (tau - 1.0f) * tau * tau;
}
#endif

float FollowPath::distance_error(float L, float T, float v0, float a0, float vT, int num_steps) {

  float dt = T / (num_steps - 1);
  float s = 0;
  float v = v0;
  float v_prev = v0;

  plan.values.resize(num_steps);
  plan.values[0] = std::pair(0.0f, vec2{s, v});

  for (int i = 1; i < num_steps; i++) {

    float t = i * dt;
    float v_ideal = interpolate_quadratic(v0, a0, vT, t, T);
    float v_limit = path.max_speed_at(s + v_prev * dt);

    v_prev = v;
    v = fminf(v_ideal, v_limit);

    s += 0.5f * (v + v_prev) * dt;

    plan.values[i] = std::pair(t, vec2{s, v});

  }

  return L - s;

}

void FollowPath::calculate_plan(Curve path_, float arrival_time_, float arrival_speed_) {

  path = path_;
  arrival_time = arrival_time_; 
  arrival_speed = arrival_speed_; 

  path.calculate_max_speeds(norm(car.velocity), arrival_speed);

  float dt = 1.0 / 60.0f;
  float s = path.find_nearest(car.position);

  float T = arrival_time - car.time;
  if (T < 0) {
    std::cout << "Warning: invalid arrival time" << std::endl;
  } else {

    float v0 = norm(car.velocity);
    float vf = arrival_speed;
  
    float a = Drive::boost_accel + Drive::throttle_accel(vf);
    float error = distance_error(s, T, vf, a, v0, int(T / dt));
    std::cout << "a = " << a << " calculating error: " << error << std::endl;

  
    float a_old = -Drive::brake_accel;
    float error_old = distance_error(s, T, vf, a_old, v0, int(T / dt));
    std::cout << "a = " << a_old << " calculating error: " << error_old << std::endl;
  
    // try to find the right arrival acceleration
    // using a few iterations of secant method
    for (int i = 0; i < 1 * 16; i++) {
  
      if (fabs(error) < 0.5f) break;
  
      float new_a = (a_old * error - a * error_old) / (error - error_old);
  
      a_old = a;
      a = new_a;
  
      error_old = error;
      error = distance_error(s, T, vf, a, v0, int(T / dt));

      std::cout << "a = " << a << " calculating error: " << error << std::endl;

    }

  }
}
