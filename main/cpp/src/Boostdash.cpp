#include "Boostdash.h"

#include <iostream>

Boostdash::Boostdash(Car & c) : car(c), dodge(c), turn(c) {
	finished = false;
	controls = Input();

	dodge.duration = 0.0f;
	dodge.delay = 0.80f;

	boost_off = 0.4f;
	turn_up = 0.2f;

	timer = 0.0f;
}

void Boostdash::step(float dt) {

	vec3 direction = normalize(car.v);
	dodge.direction = direction;
	dodge.step(dt);

	if (timer < turn_up) {
		turn.target = look_at(xy(direction) - vec3{ 0.0, 0.0, 0.6f });
	}
	else {
		turn.target = look_at(xy(direction) + vec3{ 0.0, 0.0, 0.6f });
	}
	turn.step(dt);

	controls.jump = dodge.controls.jump;
	if (controls.jump) {
		controls.roll = dodge.controls.roll;
		controls.pitch = dodge.controls.pitch;
		controls.yaw = dodge.controls.yaw;
	}
	else {
		controls.roll = turn.controls.roll;
		controls.pitch = turn.controls.pitch;
		controls.yaw = turn.controls.yaw;
	}

	controls.boost = (timer <= boost_off) ? 1 : 0;

	timer += dt;

	if (timer > 0.25f && car.on_ground && norm(xy(car.w)) < 0.5f) {
		finished = true;
	}

}

//#include <fstream>
//
//Car Boostdash::simulate() {
//
//  // make a copy of the car's state and get a pointer to it
//  Car car_copy = Car(car);
//  Boostdash copy = Boostdash(car_copy);
//  copy.arrival_time = arrival_time;
//  copy.target_orientation = target_orientation;
//  copy.up = up;
//  copy.angle_threshold = angle_threshold;
//  copy.reorient_distance = reorient_distance;
//  copy.throttle_distance = throttle_distance;
//
//  copy.finished = finished;
//  copy.controls = controls;
//
//  std::ofstream outfile("../../Utilities/analysis/aerial_simulation.csv");
//
//  float dt = 0.01666f;
//  for (float t = dt; t < 5.0f; t += dt) {
//
//    // get the new controls
//    copy.step(dt); 
//
//    // and simulate their effect on the car
//    car_copy.step(copy.controls, dt); 
//
//    outfile << " " << car_copy.time << ", ";
//    outfile << " " << car_copy.x[0] << ", ";
//    outfile << " " << car_copy.x[1] << ", ";
//    outfile << " " << car_copy.x[2] << ", ";
//    outfile << " " << car_copy.v[0] << ", ";
//    outfile << " " << car_copy.v[1] << ", ";
//    outfile << " " << car_copy.v[2] << ", ";
//    outfile << " " << car_copy.w[0] << ", ";
//    outfile << " " << car_copy.w[1] << ", ";
//    outfile << " " << car_copy.w[2] << "\n";
//
//    if (copy.finished) break;
//  }
//
//  outfile.close();
//
//  return car_copy;
//
//}

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_boostdash(pybind11::module & m) {
	pybind11::class_<Boostdash>(m, "Boostdash")
		.def(pybind11::init<Car &>())
		.def_readonly("finished", &Boostdash::finished)
		.def_readonly("controls", &Boostdash::controls)
		.def("step", &Boostdash::step);
	//.def("simulate", &Boostdash::simulate);
}
#endif
