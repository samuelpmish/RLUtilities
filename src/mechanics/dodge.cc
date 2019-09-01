#include "mechanics/dodge.h"

#include <optional>
#include <variant>

const float Dodge::timeout = 1.25f;
const float Dodge::input_threshold = 0.5f;

const float Dodge::z_damping = 0.35f;
const float Dodge::z_damping_start = 0.15f;
const float Dodge::z_damping_end = 0.21f;

const float Dodge::torque_time = 0.65f;
const float Dodge::side_torque = 260.0f;
const float Dodge::forward_torque = 224.0f;

Dodge::Dodge(Car & c) : car(c), reorient(c) {

	direction = vec2{0.0f, 0.0f};
	jump_duration = 0.15f;

	preorientation = mat3(0.0f);

	finished = false;
	controls = Input();

	delay = 0.0f;
	timer = 0.0f;
}

void Dodge::step(float dt) {

	float timeout = 0.9f;

	controls.jump = 0;
	controls.roll = 0.0f;
	controls.pitch = 0.0f;
	controls.yaw = 0.0f;

	if (timer < jump_duration) {
		controls.jump = 1;
	}

	if (timer >= delay && timer > jump_duration + 2.0f * dt && !car.double_jumped && !car.on_ground) {

		if (norm(direction) > 0.0f) {

			vec3 f = car.forward();
			mat2 orientation_xy = rotation(atan2(f[1], f[0]));
			vec2 direction_local = dot(normalize(direction), orientation_xy);

			float vf = dot(car.velocity, car.forward());
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

	if ((timer < delay) && fabs(det(preorientation) - 1.0f) < 0.01f) {

		reorient.target_orientation = preorientation;
		reorient.step(dt);

		controls.roll = reorient.controls.roll;
		controls.pitch = reorient.controls.pitch;
		controls.yaw = reorient.controls.yaw;

	}

	finished = timer > timeout;

	timer += dt;

}

Car Dodge::simulate() {

	// make a copy of the car's state and get a pointer to it
	Car car_copy = Car(car);
	Dodge copy = Dodge(car_copy);
	copy.direction = direction;
	copy.jump_duration = jump_duration;
	copy.delay = delay;
	copy.preorientation = preorientation;
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

