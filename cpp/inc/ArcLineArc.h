#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "linalg.h"

class ArcLineArc {
 public:
  vec2 p1, p2;  // start and end of arc-line-arc curve
  vec2 q1, q2;  // points of transition between circles
  vec2 t1, t2;  // tangents at start of arc-line-arc curve
  vec2 n1, n2;  // normals at start of arc-line-arc curve
  vec2 o1, o2;  // centers of turning circles

  float r1, r2;  // radius of turning circles
  float phi1, phi2;  // turning angle for each circle

  float L1, L2, L3;  // length of each curve segment
  float length;      // total arc length of curve

  bool is_valid;

  // I can't believe windows is so ass-backwards that
  // I have to do this, since _USE_MATH_DEFINES still
  // doesn't work.
  const float PI = 3.14159265358979323846;

  ArcLineArc(const vec2 &  _p1,
             const vec2 &  _t1,
             const float & _r1,
             const vec2 &  _p2,
             const vec2 &  _t2,
             const float & _r2) {
    p1 = _p1;
    t1 = _t1;
    n1 = cross(t1);
    r1 = _r1;
    o1 = p1 + r1 * n1;

    p2 = _p2;
    t2 = _t2;
    n2 = cross(t2);
    r2 = _r2;
    o2 = p2 + r2 * n2;

    // set up a coordinate system along the axis
    // connecting the two circle's centers
    vec2 delta_o = o2 - o1;
    vec2 e1 = normalize(delta_o);
    vec2 e2 = - sgn(r1) * cross(e1);

    // figure out if we transition from CW to CCW or vice versa
    // and compute some of the characteristic lengths for the problem
    float sign = - sgn(r1) * sgn(r2);
    float R = fabs(r1) + sign * fabs(r2);
    float L = norm(delta_o);
    float x, y;

    if (((R*R) / (L*L)) < 0.97) {
        float H = sqrt(L*L - R*R);

        // the endpoints of the line segment connecting the circles
        q1 = o1 + fabs(r1) * ((R/L) * e1 + (H/L) * e2);
        q2 = o2 - sign * fabs(r2) * ((R/L) * e1 + (H/L) * e2);

        vec2 pq1 = normalize(q1 - p1);
        phi1 = 2.0f * sgn(dot(pq1, t1)) * asin(fabs(dot(pq1, n1)));
        if (phi1 < 0.0f) phi1 += 2.0f * PI;

        vec2 pq2 = normalize(q2 - p2);
        phi2 = -2.0f * sgn(dot(pq2, t2)) * asin(fabs(dot(pq2, n2)));
        if (phi2 < 0.0f) phi2 += 2.0f * PI;

        L1 = phi1 * fabs(r1);
        L2 = norm(q2 - q1);
        L3 = phi2 * fabs(r2);
        length = L1 + L2 + L3;
        is_valid = true;
    } else {
        q1 = vec2{0, 0};
        q2 = vec2{0, 0};

        phi1 = 0;
        phi2 = 0;

        L1 = -1;
        L2 = -1;
        L3 = -1;
        length = -1;
        is_valid = false;
    }
  }
};

// class ArcLineArc:
//
//    __slots__ = ['o1', 'p1', 'q1', 'r1', 't1', 'phi1',
//                 'o2', 'p2', 'q2', 'r2', 't2', 'phi2',
//                 'l1', 'l2', 'l3', 'length', 'valid', 'points']
//
//    def __init__(self, p1, t1, r1, p2, t2, r2, offset = 0):
//
//        # starting position and directions
//        self.p1 = p1 + offset * t1
//        self.t1 = t1
//        n1 = cross(t1)
//
//        # target position and directions
//        self.p2 = p2 - offset * t2
//        self.t2 = t2
//        n2 = cross(t2)
//
//        # centers and radii of turning circles
//        self.o1 = self.p1 + r1 * n1
//        self.r1 = r1
//        self.o2 = self.p2 + r2 * n2
//        self.r2 = r2
//
//        # set up a coordinate system along the axis
//        # connecting the two circle's centers
//        delta_o = self.o2 - self.o1
//        e1 = normalize(delta_o)
//        e2 = - sgn(r1) * cross(e1)
//
//        # figure out if we transition from CW to CCW or vice versa
//        # and compute some of the characteristic lengths for the problem
//        sign = - sgn(r1) * sgn(r2)
//        R = abs(r1) + sign * abs(r2)
//        L = norm(delta_o)
//
//        if ((R*R) / (L*L)) < 0.97:
//
//            H = math.sqrt(L*L - R*R)
//
//            # the endpoints of the line segment connecting the circles
//            self.q1 = self.o1 + abs(r1) * ((R/L) * e1 + (H/L) * e2)
//            self.q2 = self.o2 - sign * abs(r2) * ((R/L) * e1 + (H/L) * e2)
//
//            pq1 = normalize(self.q1 - self.p1)
//            self.phi1 = 2 * sgn(dot(pq1, t1)) * math.asin(abs(dot(pq1, n1)))
//            if self.phi1 < 0:
//                self.phi1 += 2 * math.pi
//
//            pq2 = normalize(self.q2 - self.p2)
//            self.phi2 = -2 * sgn(dot(pq2, t2)) * math.asin(abs(dot(pq2, n2)))
//            if self.phi2 < 0:
//                self.phi2 += 2 * math.pi
//
//            self.l1 = self.phi1 * abs(r1)
//            self.l2 = norm(self.q2 - self.q1)
//            self.l3 = self.phi2 * abs(r2)
//            self.length = self.l1 + self.l2 + self.l3
//            self.valid = True
//
//        else:
//
//            self.q1 = vec2(0, 0)
//            self.q2 = vec2(0, 0)
//
//            self.phi1 = 0
//            self.phi2 = 0
//
//            self.l1 = -1
//            self.l2 = -1
//            self.l3 = -1
//            self.length = -1
//            self.valid = False
