#include "Aerial.h"

#include <iostream>

const float Aerial::boost_accel = 1060.0f;
const float Aerial::throttle_accel = 66.66667f;

Aerial::Aerial(Car & c) : car(c), double_jump(c), turn(c) {

	finished = false;
	controls = Input();

	up = vec3{ 0.0f, 0.0f, 1.0f };
	angle_threshold = 0.3f;
	reorient_distance = 25.0f;
	throttle_distance = 50.0f;

	double_jump.duration = 0.20f;
	double_jump.delay = 0.25f;

	jumping = car.on_ground;
}

void Aerial::step(float dt) {

	const vec3 gravity{ 0.0f, 0.0f, -650.0f };

	float T = arrival_time - car.time;

	vec3 delta_x = target - (car.x + car.v * T + 0.5f * gravity * T * T);

	vec3 direction = normalize(delta_x);

	if (norm(delta_x) > reorient_distance) {
		turn.target = look_at(delta_x, up);
	}
	else {
		turn.target = target_orientation.value_or(look_at(target - car.x, up));
	}

	turn.step(dt);
	controls = turn.controls;

	if (jumping) {

		double_jump.step(dt);
		controls.jump = double_jump.controls.jump;

		if (double_jump.timer >= double_jump.delay) {
			controls.roll = 0.0f;
			controls.pitch = 0.0f;
			controls.yaw = 0.0f;
			jumping = false;
		}

	}

	// only boost/throttle if we're facing the right direction
	if (angle_between(car.forward(), direction) < angle_threshold) {
		if (norm(delta_x) > throttle_distance) {
			controls.boost = 1;
			controls.throttle = 0.0f;
		}
		else {
			controls.boost = 0;
			controls.throttle = clip(0.5f * throttle_accel * T * T, 0.0f, 1.0f);
		}
	}
	else {
		controls.boost = 0;
		controls.throttle = 0.0f;
	}

	finished = (T <= 0.0f);

	if (fabs(T) < 0.05f) {
		std::cout << T << " " << target << " " << car.x << std::endl;
	}

}

#include <fstream>
#include "timer.h"

Car Aerial::simulate() {

	timer stopwatch;
	stopwatch.start();

	// make a copy of the car's state and get a pointer to it
	Car car_copy = Car(car);
	Aerial copy = Aerial(car_copy);
	copy.target = target;
	copy.arrival_time = arrival_time;
	copy.target_orientation = target_orientation;
	copy.up = up;
	copy.angle_threshold = angle_threshold;
	copy.reorient_distance = reorient_distance;
	copy.throttle_distance = throttle_distance;

	copy.finished = finished;
	copy.controls = controls;

	float dt = 0.05000f;
	for (float t = dt; t < 5.0f; t += dt) {

		// get the new controls
		copy.step(dt);

		// and simulate their effect on the car
		car_copy.step(copy.controls, dt);

		//outfile << " " << car_copy.time << ", ";
		//outfile << " " << car_copy.x[0] << ", ";
		//outfile << " " << car_copy.x[1] << ", ";
		//outfile << " " << car_copy.x[2] << ", ";
		//outfile << " " << car_copy.v[0] << ", ";
		//outfile << " " << car_copy.v[1] << ", ";
		//outfile << " " << car_copy.v[2] << ", ";
		//outfile << " " << car_copy.w[0] << ", ";
		//outfile << " " << car_copy.w[1] << ", ";
		//outfile << " " << car_copy.w[2] << ", ";
		//outfile << " " << copy.controls.boost << "\n";

		if (copy.finished) break;
	}

	//outfile.close();

	stopwatch.stop();
	std::cout << stopwatch.elapsed() << std::endl;

	return car_copy;
}

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_aerial(pybind11::module & m) {
	pybind11::class_<Aerial>(m, "Aerial")
		.def(pybind11::init<Car &>())
		.def_readwrite("arrival_time", &Aerial::arrival_time)
		.def_readwrite("target", &Aerial::target)
		.def_readwrite("target_orientation", &Aerial::target_orientation)
		.def_readwrite("up", &Aerial::up)
		.def_readwrite("angle_threshold", &Aerial::angle_threshold)
		.def_readwrite("reorient_distance", &Aerial::reorient_distance)
		.def_readwrite("throttle_distance", &Aerial::throttle_distance)
		.def_readonly("finished", &Aerial::finished)
		.def_readonly("controls", &Aerial::controls)
		.def_readonly_static("boost_accel", &Aerial::boost_accel)
		.def_readonly_static("throttle_accel", &Aerial::throttle_accel)
		.def("step", &Aerial::step)
		.def("simulate", &Aerial::simulate);
}
#endif
