#pragma once

#include "simulation/car.h"
#include "simulation/input.h"

#include "mechanics/dodge.h"
#include "mechanics/aerial_turn.h"

#include "linear_algebra/math.h"

#include <string>
#include <optional>

class Wavedash {

 public:

  Car & car;

  std::optional < vec2 > direction;

  Input controls;
  bool finished;

  Wavedash(Car & c);

  void step(float dt);

  Car simulate();

 private:

  float timer;

  Dodge dodge;
  AerialTurn turn;

};
