#include "simulation/composite_arc.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include "mechanics/drive.h"

CompositeArc::CompositeArc() {
  p1 = p2 = { 0.0f, 0.0f };
  q1 = q2 = { 0.0f, 0.0f };
  t1 = t2 = { 0.0f, 0.0f };
  n1 = n2 = { 0.0f, 0.0f };
  o1 = o2 = { 0.0f, 0.0f };

  r1 = r2 = 0.0f;
  phi1 = phi2 = 0.0f;

  L[0] = L[1] = L[2] = L[3] = L[4] = 0.0f;
  length = 0.0f;
}

CompositeArc::CompositeArc(const float _L0,
                           const vec2  _p1,
                           const vec2  _t1,
                           const float _r1,
                           const float _L4,
                           const vec2  _p2,
                           const vec2  _t2,
                           const float _r2) {

  // I can't believe windows is so ass-backwards that
  // I have to do this, since _USE_MATH_DEFINES still
  // doesn't work.
  const float PI = 3.14159265358979323846f;

  p1 = _p1 + _L0 * _t1;
  t1 = _t1;
  n1 = cross(t1);
  r1 = _r1;
  o1 = p1 + r1 * n1;

  p2 = _p2 - _L4 * _t2;
  t2 = _t2;
  n2 = cross(t2);
  r2 = _r2;
  o2 = p2 + r2 * n2;

  vec2 delta_o = o2 - o1;

  // figure out if we transition from CW to CCW or vice versa
  // and compute some of the characteristic lengths for the problem
  float sign = -sgn(r1) * sgn(r2);
  float R = fabs(r1) + sign * fabs(r2);
  float o1o2 = norm(delta_o);
  float beta = 0.97f;

  // if the two turning circles are too big, then a smooth path
  // isn't possible, so we shrink the radii of those circles just
  // enough to make it feasible
  if (((R*R) / (o1o2*o1o2)) > beta) {
    vec2 delta_p = p2 - p1;
    vec2 delta_n = r2 * n2 - r1 * n1;

    float a = beta * dot(delta_n, delta_n) - R * R;
    float b = 2.0f * beta * dot(delta_n, delta_p);
    float c = beta * dot(delta_p, delta_p);

    float alpha = (-b - sqrt(b*b - 4.0f*a*c)) / (2.0f*a);

    // scale the radii by alpha, and update the relevant quantities
    r1 *= alpha;
    r2 *= alpha;
    R *= alpha;

    o1 = p1 + r1 * n1;
    o2 = p2 + r2 * n2;

    delta_o = o2 - o1;
    o1o2 = norm(delta_o);
  }

  // set up a coordinate system along the axis
  // connecting the two circle's centers
  vec2 e1 = normalize(delta_o);
  vec2 e2 = -sgn(r1) * cross(e1);

  float H = sqrt(o1o2*o1o2 - R*R);

  // the endpoints of the line segment connecting the circles
  q1 = o1 + fabs(r1) * ((R / o1o2) * e1 + (H / o1o2) * e2);
  q2 = o2 - sign * fabs(r2) * ((R / o1o2) * e1 + (H / o1o2) * e2);

  vec2 pq1 = normalize(q1 - p1);
  phi1 = 2.0f * sgn(dot(pq1, t1)) * asin(fabs(dot(pq1, n1)));
  if (phi1 < 0.0f) phi1 += 2.0f * PI;

  vec2 pq2 = normalize(q2 - p2);
  phi2 = -2.0f * sgn(dot(pq2, t2)) * asin(fabs(dot(pq2, n2)));
  if (phi2 < 0.0f) phi2 += 2.0f * PI;

  L[0] = _L0;
  L[1] = phi1 * fabs(r1);
  L[2] = norm(q2 - q1);
  L[3] = phi2 * fabs(r2);
  L[4] = _L4;
  length = L[0] + L[1] + L[2] + L[3] + L[4];

}

Curve CompositeArc::discretize(int n) {

  vec2 r;
  mat2 Q;
  Curve c;

  float ds = length / n;

  int segments[5];
  int capacity = 1;
  for (int i = 0; i < 5; i++) {
    segments[i] = int(ceil(L[i] / ds));
    capacity += segments[i];
  }

  c.length = length;
  c.points = std::vector < vec3 >(capacity);
  c.distances = std::vector < float >(capacity);
  c.curvatures = std::vector < float >(capacity);

  int id = 0;
  float s = length;

  vec2 m1 = p1 - L[0] * t1;
  vec2 m2 = p2 + L[4] * t2;

  ds = L[0] / segments[0];
  for (int i = 0; i < segments[0]; i++) {
    c.points[id] = vec3(lerp(m1, p1, float(i) / segments[0]));
    c.curvatures[id] = 0.0f;
    c.distances[id] = s;
    id++;
    s -= ds;
  }


  ds = L[1] / segments[1];
  r = p1 - o1;
  Q = rotation(sgn(r1) * phi1 / segments[1]);
  for (int i = 0; i < segments[1]; i++) {
    c.points[id] = vec3(o1 + r);
    c.curvatures[id] = ((i == 0) ? 0.25f : 0.5f) / r1;
    c.distances[id] = s;
    id++;
    s -= ds;
    r = dot(Q, r);
  }


  ds = L[2] / segments[2];
  for (int i = 0; i < segments[2]; i++) {
    c.points[id] = vec3(lerp(q1, q2, float(i) / segments[2]));
    c.curvatures[id] = ((i == 0) ? 0.25f / r1 : 0.0f);
    c.distances[id] = s;
    id++;
    s -= ds;
  }


  ds = L[3] / segments[3];
  r = q2 - o2;
  Q = rotation(sgn(r2) * phi2 / segments[3]);
  for (int i = 0; i < segments[3]; i++) {
    c.points[id] = vec3(o2 + r);
    c.curvatures[id] = ((i == 0) ? 0.25f : 0.5f) / r2;
    c.distances[id] = s;
    id++;
    s -= ds;
    r = dot(Q, r);
  }


  ds = L[4] / segments[4];
  for (int i = 0; i <= segments[4]; i++) {
    c.points[id] = vec3(lerp(p2, m2, float(i) / segments[4]));
    c.curvatures[id] = ((i == 0) ? 0.25f / r2 : 0.0f);
    c.distances[id] = s;
    id++;
    s -= ds;
  }

  c.calculate_tangents();

  return c;

}
