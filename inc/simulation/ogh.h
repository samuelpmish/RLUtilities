#pragma once

#include "linear_algebra/math.h"

// this only implements the simplest version of the OGH curve
//
// the user is expected to check that a0 and a1 are 
// greater than zero to ensure its validity
class OGH {

 public:

  // OGH coefficients
  float a0, a1;

  // endpoint positions, tangents, and normals
  vec3 P0, P1, V0, V1;

  OGH(vec3 _P0, vec3 _V0, vec3 _P1, vec3 _V1);

  vec3 evaluate(float t);

  vec3 tangent(float t);

  vec3 acceleration(float t);

};
