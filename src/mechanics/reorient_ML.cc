#include <iostream>
#include <memory>

#include <mechanics/reorient_ML.h>

Model ReorientML::model = Model();

void ReorientML::set_model(Model model1) {
  model = model1;
}

ReorientML::ReorientML(Car &c) : car(c) {
  target_orientation = eye<3>();
  eps_phi = 0.10f;

  finished = false;
  controls = Input();
}

void ReorientML::step(float dt) {
  vec3 omega = dot(transpose(car.orientation), car.angular_velocity);
  mat3 theta = dot(transpose(car.orientation), target_orientation);

  finished = angle_between(car.orientation, target_orientation) < eps_phi;

  vec3 output = model.get_controls(theta, omega);

  controls.roll = output[0];
  controls.pitch = output[1];
  controls.yaw = output[2];
}

Car ReorientML::simulate() {

  Car car_copy = Car(car);
  auto copy = ReorientML(car_copy);
  copy.target_orientation = target_orientation;
  copy.eps_phi = eps_phi;

  float dt = 0.01666f;
  for (int t = 0; t < 120; t++) {

    // get the new controls
    copy.step(dt);

    // and simulate their effect on the car
    car_copy.step(copy.controls, dt);

    if (copy.finished) break;
  }

  return car_copy;
}

Model::Model() = default;

Model::Model(std::vector<float> binary) {
  weight1 = *reinterpret_cast<mat<20,12>*>(&binary[0]);
  bias1 = *reinterpret_cast<vec<20>*>(&binary[240]);
  weight2 = *reinterpret_cast<mat<3,20>*>(&binary[260]);
}

vec3 Model::get_controls(const mat3 orientation, const vec3 angular_velocity) {
  auto result = vec3(0);

  for (const float x_flip: {-1.f, 1.f}) {
    for (const float y_flip: {-1.f, 1.f}) {
      auto flip = vec3({y_flip, x_flip, x_flip * y_flip});
      auto flip_mat = mat3({
                             {1.f, x_flip * y_flip, x_flip},
                             {x_flip * y_flip, 1.f, y_flip},
                             {x_flip, y_flip, 1.f}
                           });
      auto new_orientation = flip_mat * orientation;
      auto new_angular_velocity = flip * angular_velocity;
      result += flip * actor(new_orientation, new_angular_velocity);
    }
  }

  return clamp(result / 4.f, -1.f, 1.f);
}

vec3 Model::actor(const mat3 orientation, const vec3 angular_velocity) {
  auto flat = vec<12>();
  *reinterpret_cast<mat3*>(&flat[0]) = transpose(orientation);
  *reinterpret_cast<vec3*>(&flat[9]) = angular_velocity;

  auto temp = dot(weight1, flat) + bias1;
  return dot(weight2, relu(temp));
}
