#pragma once

#include <linear_algebra/vec.h>
#include <simulation/car.h>

class Model {

public:
  Model();

  explicit Model(std::vector<float> binary);

  vec3 get_controls(mat3 orientation, vec3 angular_velocity);

private:
  vec3 actor(mat3 orientation, vec3 angular_velocity);

  mat<20,12> weight1;
  vec<20> bias1;
  mat<3,20> weight2;
};


class OrientationController {

public:
  Car & car;

  mat3 target;
  float eps_phi;

  bool finished;
  Input controls;

  explicit OrientationController(Car & c);

  void step(float dt);

  Car simulate();

  static void set_model(Model model1);

private:
  static Model model;
};
