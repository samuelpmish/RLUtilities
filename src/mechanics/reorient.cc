#include "mechanics/reorient.h"

#include "misc/timer.h"

#include <fstream>

const float Reorient::scale = 10.5f;
const vec3 Reorient::angular_acceleration = 
    vec3{-400.0f, -130.0f, 95.0f} / Reorient::scale;

const vec3 Reorient::angular_damping = 
    vec3{-50.0f, -30.0f, -20.0f} / Reorient::scale;

const vec3 e[] = {
 {1.0f, 0.0f, 0.0f},
 {0.0f, 1.0f, 0.0f},
 {0.0f, 0.0f, 1.0f}
};

Reorient::Reorient(Car & c) : car(c) {
  target_orientation = eye<3>();
	eps_phi = 0.10f;
	eps_omega = 0.15f;
  horizon_time = 0.05f;

  alpha = vec3{0.0f, 0.0f, 0.0f};

  finished = false;
  controls = Input();
}

// This matrix is used to relate the angular
// velocity to the time rate of the components
// in the axis-angle representation of the
// orientation error, phi. It is defined
// such that we can write dphi_dt in the
// following way:
//
//    vec3 dphi_dt = dot(Z(phi), omega)
//
mat3 Reorient::Z(const vec3 & q) {

  float norm_q = norm(q);

  // for small enough values, use the taylor expansion
  if (norm_q < 0.2f) {

    return mat3{
      {
        1.0f - (q[1]*q[1] + q[2]*q[2]) / 12.0f,
        (q[0]*q[1] / 12.0f) + q[2] / 2.0f,
        (q[0]*q[2] / 12.0f) - q[1] / 2.0f
      },{
        (q[1]*q[0] / 12.0f) - q[2] / 2.0f,
        1.0f - (q[0]*q[0] + q[2]*q[2]) / 12.0f,
        (q[1]*q[2] / 12.0f) + q[0] / 2.0f
      },{
        (q[2]*q[0] / 12.0f) + q[1] / 2.0f,
        (q[2]*q[1] / 12.0f) - q[0] / 2.0f,
        1.0f - (q[0]*q[0] + q[1]*q[1]) / 12.0f
      }
    };

  // otherwise, use the real thing
  } else {

    float qq = norm_q * norm_q;
    float c = 0.5f * norm_q * cos(0.5f * norm_q) / sin(0.5f * norm_q);

    return mat3{
      {
        (q[0]*q[0] + c * (q[1]*q[1] + q[2]*q[2])) / qq,
        ((1.0f - c) * q[0]*q[1] / qq) + q[2] / 2.0f,
        ((1.0f - c) * q[0]*q[2] / qq) - q[1] / 2.0f
      },{
        ((1.0f - c) * q[1] * q[0] / qq) - q[2] / 2.0f,
        (q[1]*q[1] + c * (q[0]*q[0] + q[2]*q[2])) / qq,
        ((1.0f - c) * q[1] * q[2] / qq) + q[0] / 2.0f
      },{
        ((1.0f - c) * q[2] * q[0] / qq) + q[1] / 2.0f,
        ((1.0f - c) * q[2] * q[1] / qq) - q[0] / 2.0f,
        (q[2]*q[2] + c * (q[0]*q[0] + q[1]*q[1])) / qq
      }
    };

  }

}

// This function provides a guideline for when 
// control swiching should take place. 
vec3 Reorient::G(const vec3 & q, const vec3 & dq_dt) {
  const vec3 T = angular_acceleration;
  const vec3 D = angular_damping;

  float G_roll = -sgn(dq_dt[0]) * (
      (fabs(dq_dt[0]) / D[0]) + 
      (T[0] / (D[0] * D[0])) * log(T[0] / (T[0] + D[0] * fabs(dq_dt[0])))
  );

  float G_pitch = -sgn(dq_dt[1]) * dq_dt[1] * dq_dt[1] / (2.0f * T[1]);
  float G_yaw = sgn(dq_dt[2]) * dq_dt[2] * dq_dt[2] / (2.0f * T[2]);

  return vec3{G_roll, G_pitch, G_yaw};
}

// the error between the predicted state and the precomputed return trajectories
vec3 Reorient::f(const vec3 & alpha_local, const float dt) {
  vec3 alpha_world = dot(theta, alpha_local);
  vec3 omega_pred = omega + alpha_world * dt;
  vec3 phi_pred = phi + dot(Z0, omega + alpha_world * (0.5f * dt)) * dt;
  vec3 dphi_dt_pred = dot(Z0, omega_pred);
  return -phi_pred - G(phi_pred, dphi_dt_pred);
}

// Let g(x) be the continuous piecewise linear function
// that interpolates the points:
// (-1.0, values[0]), (0.0, values[1]), (1.0, values[2])
// 
// solve_pwl() determines a value of x in [-1, 1] 
// such that || g(x) - y || is minimized.
float solve_pwl(float y, const vec3 & values) {
  float min_value = fminf(fminf(values[0], values[1]), values[2]);
  float max_value = fmaxf(fmaxf(values[0], values[1]), values[2]);
  float clipped_y = clip(y, min_value, max_value);

  // if the clipped value can be found in the interval [-1, 0]
  if ((fminf(values[0], values[1]) <= clipped_y) &&
      (clipped_y <= fmaxf(values[0], values[1]))) {
    if (fabs(values[1] - values[0]) > 0.0001f) {
      return (clipped_y - values[1]) / (values[1] - values[0]);
    } else {
      return -0.5f;
    }

  // if the clipped value can be found in the interval [0, 1]
  } else {
    if (fabs(values[2] - values[1]) > 0.0001f) {
      return (clipped_y - values[1]) / (values[2] - values[1]);
    } else {
      return 0.5f;
    }
  }
}

vec3 Reorient::find_controls_for(const vec3 & ideal_alpha) {
  const vec3 w = omega_local;
  const vec3 T = angular_acceleration;
  const vec3 D = angular_damping;

  vec3 rpy, alpha_values;

  // Note: these controls are calculated differently,
  // since Rocket League never disables roll damping.
  alpha_values = vec3{-T[0] + D[0] * w[0], D[0] * w[0], T[0] + D[0] * w[0]};
  rpy[0] = solve_pwl(ideal_alpha[0], alpha_values);

  alpha_values = vec3{-T[1], D[1] * w[1], T[1]};
  rpy[1] = solve_pwl(ideal_alpha[1], alpha_values);

  alpha_values = vec3{-T[2], D[2] * w[2], T[2]};
  rpy[2] = solve_pwl(ideal_alpha[2], alpha_values);

  return rpy;
}

void Reorient::step(float dt) {

  omega = dot(transpose(target_orientation), car.angular_velocity);
  theta = dot(transpose(target_orientation), car.orientation);
  omega_local = dot(omega, theta);
  phi = rotation_to_axis(theta);

  finished = (norm(phi) < eps_phi) && (norm(omega) < eps_omega);

  if (finished) {

    controls.roll  = 0.0f;
    controls.pitch = 0.0f;
    controls.yaw   = 0.0f;

  } else {

    Z0 = Z(phi);
    dphi_dt = dot(Z0, omega);

    horizon_time = fmaxf(0.03f, 4.0f * dt);

    // Apply a few Newton iterations to find
    // local angular accelerations that try not to overshoot
    // the guideline trajectories defined by Reorient::G().
    // 
    // This helps to ensure monotonic convergence to the
    // desired orientation, when possible.
    int n_iter = 5;
    float eps = 0.001f;
    for (int i = 0; i < n_iter; i++) {
      vec3 f0 = f(alpha, horizon_time);

      mat3 J;
      for (int j = 0; j < 3; j++) {
        vec3 df_j = (f0 - f(alpha + eps * e[j], horizon_time)) / eps;
        J(0, j) = df_j[0]; J(1, j) = df_j[1]; J(2, j) = df_j[2];
        J(j,j) += 0.00001f;
      }

      vec3 delta_alpha = dot(inv(J), f0);

      alpha += delta_alpha;

      if (norm(delta_alpha) < 1.0f) break;
    }

    vec3 rpy = find_controls_for(alpha); 

    //rpy[0] *= clip(0.5f * ((fabs(phi[0]) / eps_phi) + (fabs(omega[0]) / eps_omega)), 0.0f, 1.0f);
    //rpy[1] *= clip(0.5f * ((fabs(phi[1]) / eps_phi) + (fabs(omega[1]) / eps_omega)), 0.0f, 1.0f);
    //rpy[2] *= clip(0.5f * ((fabs(phi[2]) / eps_phi) + (fabs(omega[2]) / eps_omega)), 0.0f, 1.0f);

    controls.roll  = rpy[0];
    controls.pitch = rpy[1];
    controls.yaw   = rpy[2];

  }

}

Car Reorient::simulate() {

  Car car_copy = Car(car);
  Reorient copy = Reorient(car_copy);
  copy.target_orientation = target_orientation;
  copy.eps_phi = eps_phi;
  copy.eps_omega = eps_omega;
  copy.horizon_time = horizon_time;

  float dt = 0.01666f;
  for (float t = dt; t < 2.0f; t += dt) {

    // get the new controls
    copy.step(dt); 

    // and simulate their effect on the car
    car_copy.step(copy.controls, dt); 

    if (copy.finished) break;
  }

  return car_copy;
}
