#include "mechanics/drive.h"

#include "simulation/field.h"
#include "simulation/circles.h"
#include "simulation/composite_arc.h"

#include <cmath>
#include <numeric>

const float Drive::max_speed = 2300.0f;
const float Drive::max_throttle_speed = 1410.0f;
const float Drive::boost_accel = 991.667f;
const float Drive::brake_accel = 3500.0f;
const float Drive::coasting_accel = 525.0f;

float Drive::throttle_accel(float v) {
  const int n = 3;
  float values[n][2] = { {   0.0f, 1600.0f},
                         {1400.0f,  160.0f},
                         {1410.0f,    0.0f} };

  float input = clip(fabs(v), 0.0f, 1410.0f);

  for (int i = 0; i < (n - 1); i++) {
    if (values[i][0] <= input && input < values[i + 1][0]) {
      float u = (input - values[i][0]) / (values[i + 1][0] - values[i][0]);
      return lerp(values[i][1], values[i + 1][1], u);
    }
  }

  return -1.0f;
}

float Drive::max_turning_curvature(float v) {
  const int n = 6;
  float values[n][2] = { {   0.0f, 0.00690f},
                         { 500.0f, 0.00398f},
                         {1000.0f, 0.00235f},
                         {1500.0f, 0.00138f},
                         {1750.0f, 0.00110f},
                         {2300.0f, 0.00088f} };

  float input = clip(fabs(v), 0.0f, 2300.0f);

  for (int i = 0; i < (n - 1); i++) {
    if (values[i][0] <= input && input < values[i + 1][0]) {
      float u = (input - values[i][0]) / (values[i + 1][0] - values[i][0]);
      return lerp(values[i][1], values[i + 1][1], u);
    }
  }

  return -1.0f;
}

float Drive::max_turning_speed(float curvature) {
  const int n = 6;
  float values[n][2] = { {0.00088f, 2300.0f},
              {0.00110f, 1750.0f},
              {0.00138f, 1500.0f},
              {0.00235f, 1000.0f},
              {0.00398f,  500.0f},
              {0.00690f,    0.0f} };

  float input = clip(fabs(curvature), values[0][0], values[n - 1][0]);

  for (int i = 0; i < (n - 1); i++) {
    if (values[i][0] <= input && input <= values[i + 1][0]) {
      float u = (input - values[i][0]) / (values[i + 1][0] - values[i][0]);
      return clip(lerp(values[i][1], values[i + 1][1], u), 0.0f, 2300.0f);
    }
  }

  return -1.0f;
}

Drive::Drive(Car & c) : car(c) {

  target = { NAN, NAN, NAN };
  speed = 1400.0f;

  reaction_time = 0.04f;

  controls = Input();
  finished = false;

}

void Drive::step(float dt) {

  steer_controller(dt);

  speed_controller(dt);

  if (norm(car.x - target) < 100.0f) {
    finished = true;
  }

}

void Drive::steer_controller(float dt) {

  // that same point, but in local coordinates
  vec3 target_local = dot(target - car.x, car.o);

  // angle between car's forward direction and hare_local
  float angle = atan2(target_local[1], target_local[0]);

  controls.steer = clip(3.0f * angle, -1.0f, 1.0f);

}

void Drive::speed_controller(float dt) {

  float vf = dot(car.v, car.forward());

  float acceleration = (speed - vf) / reaction_time;

  float brake_coast_transition = -(0.45f * brake_accel + 0.55f * coasting_accel);
  float coasting_throttle_transition = -0.5f * coasting_accel;
  float throttle_boost_transition = 1.0f * throttle_accel(vf) + 0.5f * boost_accel;

  // apply brakes when the desired acceleration is negative and large enough
  if (acceleration <= brake_coast_transition) {

    //std::cout << "braking" << std::endl;
    controls.throttle = -1.0f;
    controls.boost = 0;

  // let the car coast when the acceleration is negative and small
  } else if ((brake_coast_transition < acceleration) &&
    (acceleration <= coasting_throttle_transition)) {

    //std::cout << "coasting" << std::endl;
    controls.throttle = 0.0f;
    controls.boost = 0;

    // for small positive accelerations, use throttle only
  } else if ((coasting_throttle_transition < acceleration) &&
    (acceleration <= throttle_boost_transition)) {

    //std::cout << "throttling" << std::endl;
    controls.throttle = clip(acceleration / throttle_accel(vf), 0.02f, 1.0f);
    controls.boost = 0;

    // if the desired acceleration is big enough, use boost
  } else if (throttle_boost_transition < acceleration) {

    //std::cout << "boosting" << std::endl;
    controls.throttle = 1.0f;
    controls.boost = 1;

  }

}
