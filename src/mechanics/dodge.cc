#include "mechanics/dodge.h"

#include <optional>
#include <variant>

const float Dodge::timeout = 1.5f;
const float Dodge::input_threshold = 0.5f;

const float Dodge::z_damping = 0.35f;
const float Dodge::z_damping_start = 0.15f;
const float Dodge::z_damping_end = 0.21f;

const float Dodge::torque_time = 0.65f;
const float Dodge::side_torque = 260.0f;
const float Dodge::forward_torque = 224.0f;

Dodge::Dodge(Car & c) : car(c), turn(c) {
	target = {};
	direction = {};

	finished = false;
	controls = Input();

	timer = 0.0f;
}

void Dodge::step(float dt) {

	float timeout = 0.9f;

	controls.jump = 0;
	controls.roll = 0.0f;
	controls.pitch = 0.0f;
	controls.yaw = 0.0f;

	if (duration && timer <= duration.value()) {
		controls.jump = 1;
	}

	float dodge_time;
	if (!duration && !delay) std::cout << "invalid dodge parameters" << std::endl;
	if (!duration &&  delay) dodge_time = delay.value();
	if (duration && !delay) dodge_time = duration.value() + 2.0f * dt;
	if (duration &&  delay) dodge_time = delay.value();

  //if ((timer < dodge_time) && ((timer + dt) >= dodge_time)) {
	if (timer >= dodge_time && !car.double_jumped && !car.on_ground) {

		vec2 direction_local;

		if ((!target && !direction) || (target && direction)) {
			direction_local = vec2{ 0.0f, 0.0f };
		}

		if (target && !direction) {
			direction_local = dot(vec2(normalize(target.value() - car.x)), car.o_dodge);
		}

		if (!target && direction) {
			direction_local = dot(normalize(direction.value()), car.o_dodge);
		}

		if (norm(direction_local) > 0.0f) {

			float vf = dot(car.v, car.forward());
			float s = fabs(vf) / 2300.0f;

			bool backward_dodge;

			if (fabs(vf) < 100.0f) {
				backward_dodge = (direction_local[0] < 0.0f);
			}
			else {
				backward_dodge = (direction_local[0] >= 0.0f) != (vf > 0.0f);
			}

			direction_local[0] /= (backward_dodge) ? (16.0f / 15.0f) * (1.0f + 1.5f * s) : 1.0f;
			direction_local[1] /= (1.0f + 0.9f * s);

			direction_local = normalize(direction_local);

			controls.roll = 0.0f;
			controls.pitch = -direction_local[0];
			controls.yaw = direction_local[1];

		}

		controls.jump = 1;

	}

  if (car.double_jumped) {
    controls.jump = 0;
  }

	if ((timer < dodge_time) && preorientation) {

		turn.target = preorientation.value();
		turn.step(dt);

		controls.roll = turn.controls.roll;
		controls.pitch = turn.controls.pitch;
		controls.yaw = turn.controls.yaw;

	}

	finished = timer > timeout;

	timer += dt;

}

Car Dodge::simulate() {

	// make a copy of the car's state and get a pointer to it
	Car car_copy = Car(car);
	Dodge copy = Dodge(car_copy);
	copy.duration = duration;
	copy.target = target;
	copy.direction = direction;
	copy.finished = finished;
	copy.timer = timer;

	float dt = 0.01666f;
	for (float t = dt; t < 1.5f; t += dt) {

		// get the new controls
		copy.step(dt);

		// and simulate their effect on the car
		car_copy.step(copy.controls, dt);

		if (copy.finished) break;
	}

	return car_copy;

}

nlohmann::json Dodge::to_json() {

  vec3 _target = target.value_or(vec3{NAN, NAN, NAN});
  vec2 _direction = direction.value_or(vec2{NAN, NAN});
  mat3 _preorientation = preorientation.value_or(mat3(NAN));
  float _duration = duration.value_or(NAN);
  float _delay = delay.value_or(NAN);

  nlohmann::json obj;
  obj["type"] = "Dodge";
  obj["target"] = {_target[0], _target[1], _target[2]};
  obj["direction"] = {_direction[0], _direction[1]};
  obj["preorientation"] = {
    {_preorientation(0, 0), _preorientation(0, 1), _preorientation(0, 2)},
    {_preorientation(1, 0), _preorientation(1, 1), _preorientation(1, 2)},
    {_preorientation(2, 0), _preorientation(2, 1), _preorientation(2, 2)}
  };
  obj["duration"] = _duration;
  obj["timer"] = timer;
  obj["finished"] = finished;
  return obj;

}
