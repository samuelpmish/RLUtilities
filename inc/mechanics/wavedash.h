#pragma once

#include "simulation/car.h"
#include "simulation/input.h"

#include "mechanics/dodge.h"
#include "mechanics/reorient.h"

#include "linear_algebra/math.h"

#include <string>
#include <optional>

class Wavedash {

 public:

  Car & car;

  vec2 direction;

  Input controls;
  bool finished;

  Wavedash(Car & c);

  void step(float dt);

  Car simulate();

 private:

  float timer;

  Dodge dodge;
  Reorient reorient;

};
