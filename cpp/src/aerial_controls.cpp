#include "linalg.h"

const float T_r = -36.07956616966136f; // torque coefficient for roll
const float T_p = -12.14599781908070f; // torque coefficient for pitch
const float T_y =   8.91962804287785f; // torque coefficient for yaw
const float D_r =  -4.47166302201591f; // drag coefficient for roll
const float D_p = -2.798194258050845f; // drag coefficient for pitch
const float D_y = -1.886491900437232f; // drag coefficient for yaw

vec3 aerial_inputs(
    vec3 omega_start,
    vec3 omega_end,
    mat3x3 theta_start,
    float dt) {

  // net torque in world coordinates
  vec3 tau = (omega_end - omega_start) / dt;

  // net torque in local coordinates
  tau = dot(transpose(theta_start), tau);

  // beginning-step angular velocity, in local coordinates
  vec3 omega_local = dot(transpose(theta_start), omega_start);

  vec3 rhs{
    tau[0] - D_r * omega_local[0],
    tau[1] - D_p * omega_local[1],
    tau[2] - D_y * omega_local[2]
  };

  // user inputs: roll, pitch, yaw
  vec3 u{
    rhs[0] / T_r,
    rhs[1] / (T_p + sgn(rhs[1]) * omega_local[1] * D_p),
    rhs[2] / (T_y - sgn(rhs[2]) * omega_local[2] * D_y)
  };

  // ensure that values are between -1 and +1
  u[0] *= fmin(1.0f, 1.0f / fabs(u[0]));
  u[1] *= fmin(1.0f, 1.0f / fabs(u[1]));
  u[2] *= fmin(1.0f, 1.0f / fabs(u[2]));

  return u;

}

int main() {

  float dt = 0.016601f;
  vec3 omega_start{-1.75055f, -0.349946f, -1.81856f};
  vec3 omega_end{-1.82712f, -0.347064f, -1.85748f};
  mat3x3 theta {
    {-0.015123f,  0.369329f, -0.929176f}, 
    { 0.299064f,  0.888413f,  0.348258f}, 
    { 0.954113f, -0.272616f, -0.123888f}
  };

  vec3 inputs = aerial_inputs( omega_start, omega_end, theta, dt);

  std::cout << inputs << std::endl;

}

