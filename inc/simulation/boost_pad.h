#pragma once

#include "linear_algebra/math.h"

enum class BoostPadState {
  Available,
  Unavailable
};

enum class BoostPadType {
  Partial,
  Full
};

struct BoostPad {
  vec3 position;
  BoostPadType type;
  BoostPadState state;
  float timer;
  uint16_t actor_id;
};