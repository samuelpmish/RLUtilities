#pragma once

#include "flatbuffers/flatbuffers.h"
#include "rlbot_generated.h"

#include <array>
#include <optional>

struct DesiredVector3 {
  float x, y, z;
};

struct DesiredRotator {
  float pitch, yaw, roll;
};

class PhysicsState {
 public:
  std::optional<DesiredVector3> location;
  std::optional<DesiredVector3> velocity;
  std::optional<DesiredRotator> rotation;
  std::optional<DesiredVector3> angularVelocity;

  PhysicsState();
};

class BallState {
 public:
  PhysicsState physicsState;

  BallState();
};

class CarState {
 public:
  PhysicsState physicsState;
  std::optional<int> boostAmount;
};

class GameState {
 public:
  BallState ballState;
  std::array<std::optional<CarState>, 8> carStates;

  GameState();
  void BuildAndSend();
};
