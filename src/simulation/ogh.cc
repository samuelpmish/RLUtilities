#include "simulation/ogh.h"

OGH::OGH(vec3 _P0, vec3 _V0, vec3 _P1, vec3 _V1) {

  P0 = _P0;
  V0 = normalize(_V0);

  P1 = _P1;
  V1 = normalize(_V1);

  vec3 dP = P1 - P0;
  float V0dotV1 = dot(V0, V1);
  float denom = 4.0f - V0dotV1 * V0dotV1;

  a0 = (6.0f * dot(dP, V0) - 3.0f * dot(dP, V1) * V0dotV1) / denom;
  a1 = (6.0f * dot(dP, V1) - 3.0f * dot(dP, V0) * V0dotV1) / denom;

}

vec3 OGH::evaluate(float t) {
  
  return ((2.0f * t + 1.0f) * (t - 1.0f) * (t - 1.0f)) * P0 +
         ((t - 1.0f) * (t - 1.0f) * t) * a0 * V0 +
         ((3.0f - 2.0f * t) * t * t) * P1 +
         ((t - 1.0f) * t * t) * a1 * V1;

}

vec3 OGH::tangent(float t) {

  return (6.0f * t * t - 6.0f * t) * P0 + 
         (1.0f - 4.0f * t + 3.0f * t * t) * a0 * V0 +
         (6.0f * t - 6.0f * t * t) * P1 + 
         (3.0f * t * t - 2.0f * t) * a1 * V1;

}

vec3 OGH::acceleration(float t) {

  return (12.0f * t - 6.0f) * P0 + 
         ( 6.0f * t - 4.0f) * a0 * V0 - 
         (12.0f * t - 6.0f) * P1 + 
         ( 6.0f * t - 2.0f) * a1 * V1;

}
