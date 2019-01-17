#pragma once

#include "Car.h"
#include "Dodge.h"
#include "AerialTurn.h"
#include "linalg.h"
#include "RL_structs.h"

#include <string>
#include <optional>

class Wavedash {

 public:

  Car & car;

  std::optional < vec3 > direction;

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
