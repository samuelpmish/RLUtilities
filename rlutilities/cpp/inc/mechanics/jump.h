#pragma once

#include "simulation/car.h"
#include "simulation/input.h"

#include "linear_algebra/math.h"

#include <string>

class Jump {

 public:
  Car & car;

  float duration;

  bool finished;
  Input controls;

  Jump(Car & c);

  void step(float dt);

  Car simulate();

  std::string to_json();

  static const float speed;
  static const float acceleration;
  static const float min_duration;
  static const float max_duration;

 private:

  float timer;
  int counter;

};
