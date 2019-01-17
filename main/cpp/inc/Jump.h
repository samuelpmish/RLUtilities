#pragma once

#include "Car.h"
#include "linalg.h"
#include "RL_structs.h"

class Jump {

 public:
  Car & car;

  float duration;

  bool finished;
  Input controls;

  Jump(Car & c);

  void step(float dt);

	Car simulate();

  static const float speed;
  static const float acceleration;
  static const float min_duration;
  static const float max_duration;

 private:

  float timer;
  int counter;

};
