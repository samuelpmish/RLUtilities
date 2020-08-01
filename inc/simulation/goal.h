#pragma once

#include "linear_algebra/math.h"

// for Dropshot
enum class GoalState {
  Unknown,
  Closed,
  Damaged,
  Open 
};

class Goal {
 public:
  GoalState state;
  vec3 position;
  vec3 direction;
  float width;
  float height;
  uint8_t team;
  uint16_t actor_id;
};