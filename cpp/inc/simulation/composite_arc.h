#pragma once

#include "simulation/curve.h"

#include "linear_algebra/math.h"

class CompositeArc {
 public:
  vec2 p1, p2;  // start and end of arc-line-arc curve
  vec2 q1, q2;  // points of transition between circles
  vec2 t1, t2;  // tangents at start of arc-line-arc curve
  vec2 n1, n2;  // normals at start of arc-line-arc curve
  vec2 o1, o2;  // centers of turning circles

  float r1, r2;     // radius of each turning circle
  float phi1, phi2; // turning angle for each circle

  float L[5];   // length of each curve segment
  float length; // total arc length of curve

  CompositeArc();

  CompositeArc(const float  _L0,
    const vec2  _p1,
    const vec2  _t1,
    const float _r1,
    const float _L4,
    const vec2  _p2,
    const vec2  _t2,
    const float _r2);

  Curve discretize(int n);

};
