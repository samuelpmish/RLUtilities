#pragma once

#include <string>
#include <vector>
#include <variant>

#include "simulation/car.h"
#include "messages/custom.h"
#include "messages/match_config.h"

struct Ready {
  std::string name;
  int id;
  int team;
  int multiplicity;
  Loadout loadout;
};

struct SetCarState{
  uint16_t id;
  vec3 position;
  vec3 velocity;
  rotator euler_angles;
  vec3 angular_velocity;
  uint8_t boost;
};

struct SetBallState{
  vec3 position;
  vec3 velocity;
  rotator euler_angles;
  vec3 angular_velocity;
};

struct Polyline3D {
  Color color;
  std::vector < vec3 > points;
};

struct Text2D {
  Color color;
  vec2 position;
  float scale;
  std::string text;
};

struct Draw {
  uint8_t id;
  std::vector < Polyline3D > lines;
  std::vector < Text2D > text;
};

struct Clear {
  uint8_t id;
};


namespace Outgoing{

  using Message = std::variant <
    Ready,
    Input,
    SetCarState,
    SetBallState,
    MatchConfig,
    Draw,
    Clear,
    Custom
  >;

  std::string serialize(const std::vector < Message > & messages);

}