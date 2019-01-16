#include "AerialTurn.h"

AerialTurn::AerialTurn(Car & c, const mat3 & t) :
  car(c) {
  finished = false;
  target = t;
  controls = Input();
}

float AerialTurn::q(float x) {
  return 1.0f - (1.0f / (1.0f + 500.0f * x * x));
}

float AerialTurn::r(float delta, float v) {
  return delta - 0.5f * sgn(v) * v * v / ALPHA_MAX;
}

float AerialTurn::bangbang(float delta, float v, float dt) {
  float ri = r(delta, v);
  float alpha = sgn(ri) * ALPHA_MAX;
  float rf = r(delta - v * dt, v + alpha * dt);

  // use a single step of secant method to improve
  // the acceleration when residual changes sign
  if (ri * rf < 0.0f) {
		alpha *= (2.0f * (ri / (ri - rf)) - 1.0f);
	}

  return alpha;
}

float solve_PWL(float a, float b, float c) {
  float xp = (fabs(a+b) > 10e-6) ? c/(a+b) : -1.0f;
  float xm = (fabs(a-b) > 10e-6) ? c/(a-b) :  1.0f;

  if ((xm <= 0) && (0 <= xp)) {
    if (fabs(xp) < fabs(xm)) {
      return clip(xp,  0.0f, 1.0f);
    } else {
      return clip(xm, -1.0f, 0.0f);
    }
  } else {
      if (0 <= xp) return clip(xp,  0.0f, 1.0f);
      if (xm <= 0) return clip(xm, -1.0f, 0.0f);
  }

  return 0;
}

vec3 aerial_rpy(const vec3 & w0, const vec3 & w1, const mat3 & theta, float dt) {

  // car's moment of inertia (spherical symmetry)
  float J = 10.5;

  // aerial control torque coefficients
  vec3 T = vec3{-400.0f, -130.0f, 95.0f};

  // aerial damping torque coefficients
  vec3 H = vec3{-50.0f, -30.0f, -20.0f};

  // get angular velocities in local coordinates
  vec3 w0_local = dot(w0, theta);
  vec3 w1_local = dot(w1, theta);

	vec3 rpy;

	for (int i = 0; i < 3; i++) {
		float a = T[i] * dt / J;
		float b = (i == 0) ? 0 : -w0_local[i] * H[i] * dt / J;
		float c = w1_local[i] - (1 + H[i] * dt / J) * w0_local[i];
		rpy[i] = solve_PWL(a, b, c);
	}

	return rpy;
}

void AerialTurn::step(float dt) {
  mat3 relative_rotation = dot(transpose(car.o), target);
  vec3 geodesic_local = rotation_to_axis(relative_rotation);

  // figure out the axis of minimal rotation to target
  vec3 geodesic_world = dot(car.o, geodesic_local);

  // get the angular acceleration
  vec3 alpha = vec3{
      bangbang(geodesic_world[0], car.w[0], dt),
      bangbang(geodesic_world[1], car.w[1], dt),
      bangbang(geodesic_world[2], car.w[2], dt)
  };

  // reduce the corrections for when the solution is nearly converged
	alpha[0] *= q(abs(geodesic_world[0]) + abs(car.w[0]));
	alpha[1] *= q(abs(geodesic_world[1]) + abs(car.w[1]));
	alpha[2] *= q(abs(geodesic_world[2]) + abs(car.w[2]));

  // set the desired next angular velocity
  vec3 omega_next = car.w + alpha * dt;

  // determine the controls that produce that angular velocity
  vec3 roll_pitch_yaw = aerial_rpy(car.w, omega_next, car.o, dt);

  controls.roll  = roll_pitch_yaw[0];
  controls.pitch = roll_pitch_yaw[1];
  controls.yaw   = roll_pitch_yaw[2];

	float eps_omega = 0.01f;
	float eps_theta = 0.04f;

  if ((norm(car.w) < eps_omega && norm(geodesic_world) < eps_theta) || car.on_ground) {
    finished = true;
	}
}

float AerialTurn::time_to_complete() {
  Car car_copy = Car(car);
  AerialTurn copy = AerialTurn(car_copy, target);

  // make a copy of the car's state and get a pointer to it
  float t = 0.0f;
  float dt = 0.01666f;
  for (int i = 0; i < 120; i++) {
    t += dt;
    copy.step(dt); // get the new controls
    copy.car.step(copy.controls, dt); // and simulate their effect on the car
    if (copy.finished) break;
  }

  return t;
}
