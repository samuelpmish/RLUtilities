#pragma once

#include "simulation/Car.h"
#include "simulation/input.h"

#include "linear_algebra/math.h"

#include <list>
#include <vector>

class Slide {

 public:
  Car & car;

  vec3 target;

  bool finished;
  Input controls;

  Slide(Car & c);

  void step(float dt);
  Car simulate();

 private:

  float slide_timer;

};
