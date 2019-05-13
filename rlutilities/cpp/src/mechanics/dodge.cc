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

#if 0
std::string Dodge::to_json() {

  return nlohmann::json{
    {"x", {x[0], x[1], x[2]}},
    {"v", {v[0], v[1], v[2]}},
    {"w", {w[0], w[1], w[2]}},
    {"o", {o(0, 0), o(0, 1), o(0, 2),
           o(1, 0), o(1, 1), o(1, 2), 
           o(2, 0), o(2, 1), o(2, 2)}},
    {"supersonic", supersonic},
    {"jumped", jumped},
    {"double_jumped", double_jumped},
    {"on_ground", on_ground},
    {"boost_left", boost},
    {"jump_timer", jump_timer},
    {"dodge_timer", dodge_timer},
    {"dodge_dir", {dodge_dir[0], dodge_dir[1]}},
    {"time", time},

    {"steer", controls.steer},
    {"roll", controls.roll},
    {"pitch", controls.pitch},
    {"yaw", controls.yaw},
    {"throttle", controls.throttle},
    {"jump", controls.jump},
    {"boost", controls.boost},
    {"handbrake", controls.handbrake}
  }.dump();

}
#endif
