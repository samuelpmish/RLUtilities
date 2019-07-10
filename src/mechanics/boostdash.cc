#include "mechanics/boostdash.h"

#include <iostream>

Boostdash::Boostdash(Car & c) : car(c), dodge(c), reorient(c) {
	finished = false;
	controls = Input();

	dodge.jump_duration = 0.0f;
	dodge.delay = 0.80f;

	boost_off = 0.4f;
	turn_up = 0.2f;

	timer = 0.0f;
}

void Boostdash::step(float dt) {

	vec2 direction = normalize(xy(car.velocity));
	dodge.direction = direction;
	dodge.step(dt);

	if (timer < turn_up) {
		reorient.target_orientation = look_at(vec3(direction) - vec3{0.0, 0.0, 0.6f});
	}
	else {
		reorient.target_orientation = look_at(vec3(direction) + vec3{0.0, 0.0, 0.6f});
	}
	reorient.step(dt);

	controls.jump = dodge.controls.jump;
	if (controls.jump) {
		controls.roll = dodge.controls.roll;
		controls.pitch = dodge.controls.pitch;
		controls.yaw = dodge.controls.yaw;
	}
	else {
		controls.roll = reorient.controls.roll;
		controls.pitch = reorient.controls.pitch;
		controls.yaw = reorient.controls.yaw;
	}

	controls.boost = (timer <= boost_off) ? 1 : 0;

	timer += dt;

	if (timer > 0.25f && car.on_ground && norm(xy(car.angular_velocity)) < 0.5f) {
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
//  rereorient car_copy;
//
//}
