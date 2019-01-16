#pragma once

#include "Car.h"
#include "linalg.h"
#include "RL_structs.h"

class AerialTurn {

 public:
  Car & car;
  mat3 target;
  bool finished;
  Input controls;

  AerialTurn(Car & c, const mat3 & t);

  void step(float dt);

	float time_to_complete();

 private:
  float q(float x);
  float r(float delta, float v);
  float bangbang(float delta, float v, float dt);

  const float ALPHA_MAX = 9.0f;
};
