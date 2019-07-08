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
  float values[n][2] = {{   0.0f, 1600.0f},
                        {1400.0f,  160.0f},
                        {1410.0f,    0.0f}};

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
  float values[n][2] = {{   0.0f, 0.00690f},
                        { 500.0f, 0.00398f},
                        {1000.0f, 0.00235f},
                        {1500.0f, 0.00138f},
                        {1750.0f, 0.00110f},
                        {2300.0f, 0.00088f}};

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
  float values[n][2] = {{0.00088f, 2300.0f},
                        {0.00110f, 1750.0f},
                        {0.00138f, 1500.0f},
                        {0.00235f, 1000.0f},
                        {0.00398f,  500.0f},
                        {0.00690f,    0.0f}};

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

  if (norm(car.position - target) < 100.0f) {
    finished = true;
  }

}

void Drive::steer_controller(float dt) {

#if 1

	// first, transform everything into local coordinates    
	vec3 p = dot(target - car.position, car.orientation);

  // if the point is not in the same plane as the car
  // perform an unrolling transformation so it is
	if (p[0] > 15.0f) {

    const float r = 250.0f;

    // get the surface normal at the point of interest
    vec3 n = Field::collide(sphere{target, 100.0f}).direction;

    if ((norm(n) > 0.0f) && dot(n, car.up()) < 0.95f) {

	    vec3 n_local = dot(n, car.orientation);

      // define a basis for a coordinate system that
      // is more natural for the unrolling transformation
	    vec3 e2 = vec3{0.0f, 0.0f, 1.0f};
	    vec3 e3 = normalize(cross(e2, n_local));
	    vec3 e1 = normalize(cross(e2, e3));

      // the angle between the car's up direction and the destination surface normal
	    float phi_max = atan2(dot(n_local, -e1), n_local[2]);

      // the maximum height of the curved section
	    float h = r * (1.0f - cos(phi_max));

  	  float phi = (p[2] >= h) ? phi_max : acos(1.0f - clip(p[2] / r, 0.0f, 1.0f));
  	  float l = (p[2] >= h) ? (p[2] - h) / sin(phi) : 0.0f;
  
      p += (r * (phi - sin(phi)) + l * (1.0f - cos(phi))) * e1;
      p[2] = 0.0f;

    } 

  }

  float angle = atan2(p[1], p[0]);
  float omega = dot(car.angular_velocity, car.up());

  controls.steer = clip(3.4f * angle + 0.235f * omega, -1.0f, 1.0f);

#else

  // time horizon
  float delta_t = 0.08f;

  // characteristic time of the angular acceleration
  float tau = 0.0625f;

  float speed = fmax(200.0f, norm(car.v));
  float w_max = turning_curvature(v) * v;
  float w_now = dot(car.w, car.up());
  float w_future = 1.0;

  // we want to find the right value of "steer" such that
  // w(t + delta_t) == w_target
  //
  // kinematically, we approximate this by
  // w(t + delta_t) ~= w(t) + alpha * delta_t
  //
  // we model alpha in the following way
  // alpha ~= (w_max * steer - w(t)) / tau
  //
  // substitute in
  // w(t + delta_t) == w(t) + (w_max * steer - w(t)) * r
  // (where r = (delta_t / tau))
  //
  // solve for steering value
  // steer == (w(t + delta_t) + (r - 1) * w(t)) / (r * w_max) 

  controls.steer = (w_future + (r - 1.0f) * w_now) / (r * w_max);
  controls.steer = clip(controls.steer, -1.0f, 1.0f);

#endif

}

void Drive::speed_controller(float dt) {

  float vf = dot(car.velocity, car.forward());

  // the average acceleration we would like to produce
  float acceleration = (speed - vf) / reaction_time;

  // values used to determine when it is appropriate to select
  // either braking, coasting, throttle, or boost
  float brake_coast_transition = -(0.45f * brake_accel + 0.55f * coasting_accel);
  float coasting_throttle_transition = -0.5f * coasting_accel;
  float throttle_boost_transition = 1.0f * throttle_accel(vf) + 0.5f * boost_accel;

  // if the car is on a ramp or wall, we don't want to allow the
  // car to coast-- it will slide down the wall. So, redefine the
  // transition accelerations to prevent coasting from occuring.
  if (car.up()[2] < 0.7f) {
    brake_coast_transition = coasting_throttle_transition = -0.5f * brake_accel;
  }

  // apply brakes when the desired acceleration is negative and large enough
  if (acceleration <= brake_coast_transition) {

    controls.throttle = -1.0f;
    controls.boost = 0;

  // let the car coast when the acceleration is negative and small
  } else if ((brake_coast_transition < acceleration) &&
    (acceleration < coasting_throttle_transition)) {

    controls.throttle = 0.0f;
    controls.boost = 0;

  // for small positive accelerations, use throttle only
  } else if ((coasting_throttle_transition <= acceleration) &&
    (acceleration <= throttle_boost_transition)) {

    controls.throttle = clip(acceleration / throttle_accel(vf), 0.02f, 1.0f);
    controls.boost = 0;

  // if the desired acceleration is big enough, use boost
  } else if (throttle_boost_transition < acceleration) {

    controls.throttle = 1.0f;
    controls.boost = 1;

  }

}
