#pragma once

#include "linear_algebra/math.h"

float radius_from(const vec2 & p1, const vec2 & n1, const vec2 & p2) {
  const float eps = 0.0001f;
  vec2 dp = p2 - p1;
  float dp_perp = dot(p2 - p1, n1);

  float numerator = dot(dp, dp);
  float denominator = 2.0f * ((fabs(dp_perp) < eps) ? eps : dp_perp);
  return numerator / denominator;
}

