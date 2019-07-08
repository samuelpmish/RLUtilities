#pragma once

#include "simulation/car.h"
#include "simulation/input.h"

#include "mechanics/dodge.h"
#include "mechanics/reorient.h"

#include <string>
#include <optional>

class Boostdash {

 public:

  Car & car;

  Input controls;
  bool finished;

  Boostdash(Car & c);

  void step(float dt);

  Car simulate();

 private:

  float timer;
  float turn_up;
  float boost_off;

  Dodge dodge;
  Reorient reorient;

};
