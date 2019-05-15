#include "mechanics/wavedash.h"

#include <iostream>

Wavedash::Wavedash(Car & c) : car(c), dodge(c), turn(c) {
	finished = false;
	controls = Input();

	dodge.duration = 0.035f;
	dodge.delay = 1.00f;

	timer = 0.0f;
}

void Wavedash::step(float dt) {

	vec3 direction_opt = normalize(direction.value_or(car.forward()));

	dodge.direction = direction_opt;
	dodge.step(dt);

	turn.target = look_at(vec3(direction_opt) + vec3{ 0.0, 0.0, 0.4f });
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

	timer += dt;

	if (timer > 0.25f && car.on_ground && norm(xy(car.w)) < 0.5f) {
		finished = true;
	}

}

//#include <fstream>
//
//Car Wavedash::simulate() {
//
//  // make a copy of the car's state and get a pointer to it
//  Car car_copy = Car(car);
//  Wavedash copy = Wavedash(car_copy);
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
