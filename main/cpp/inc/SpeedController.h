#pragma once

#include "Car.h"
#include "curve.h"
#include "linalg.h"
#include "RL_structs.h"

class SpeedController {

 public:

  curve * g;
  bool prefer_wavedashes;

  std::optional < float > arrival_time;
  std::optional < float > arrival_speed;

  bool finished;
  
  void step(float dt);
  Car simulate();

 private:

  float vf_old;

};
