#pragma once

#include "pitch.h"

class Ball {
 public:
  static Pitch p;
  vec3 x, v, w;
  float t;
  ray last_bounce;

  float radius;

  __declspec(dllexport) Ball() {
    radius = 91.25f;

    x = vec3{0.0f, 0.0f, 1.1f * radius};
    v = vec3{0.0f, 0.0f, 0.0f};
    w = vec3{0.0f, 0.0f, 0.0f};

    last_bounce = ray();
    t = 0.0f;
  }

  inline sphere Ball::collider() { return sphere{x, radius}; }

  inline ray wall_nearby(float R) {
    if (-3750.0f < x[0] && x[0] < 3750.0f && -4500.0f < x[1] &&
        x[1] < 4500.0f && x[2] < R) {
      return ray{vec3{x[0], x[1], 0.0}, vec3{0.0f, 0.0f, 1.0f}};
    } else {
      if (p.in_contact_with(sphere{x, R})) {
        return p.last_contact_info();
      } else {
        return ray();
      }
    }
  }

  __declspec(dllexport) inline void step(float dt) {
    const float R = radius;        // ball radius
    const float G = -650.0f;       // gravitational acceleration
    const float A = 0.0003f;       // inverse moment of inertia
    const float Y = 2.0f;          // maximum frictional contribution
    const float mu = 0.280f;       // Coulomb friction coefficient
    const float C_R = 0.6f;        // coefficient of restitution
    const float drag = -0.0305f;   // velocity-proportional drag coefficient
    const float w_max = 6.0f;      // maximum angular velocity

    if (norm(v) > 0.0001) {
      vec3 a = drag * v + vec3{0, 0, G};
      vec3 v_pred = v + a * dt;
      vec3 x_pred = x + v_pred * dt;
      vec3 w_pred = w;

      if (p.in_contact_with(sphere{x_pred, radius})) {
        last_bounce = p.last_contact_info();

        vec3 n = last_bounce.direction;

        vec3 v_perp = dot(v_pred, n) * n;
        vec3 v_para = v_pred - v_perp;
        vec3 v_spin = R * cross(n, w_pred);
        vec3 s = v_para + v_spin;

        float ratio = norm(v_perp) / norm(s);

        vec3 delta_v_perp = -(1.0f + C_R) * v_perp;
        vec3 delta_v_para = -fminf(1.0f, Y * ratio) * mu * s;

        w = w_pred + A * R * cross(delta_v_para, n);
        v = v_pred + delta_v_perp + delta_v_para;

        // TODO(SAM)
        x = x + 0.5 * (v + v_pred) * dt;
        // TODO(SAM)

      } else {
        w = w_pred;
        v = v_pred;
        x = x_pred;
      }
    }

    w *= fminf(1.0, w_max / norm(w));

    t += dt;
  }
};
