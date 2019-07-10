#include "mechanics/wavedash.h"

#include <iostream>

Wavedash::Wavedash(Car & c) : car(c), dodge(c), reorient(c) {
	finished = false;
	controls = Input();

	dodge.jump_duration = 0.035f;
	dodge.delay = 1.00f;

	timer = 0.0f;
}

void Wavedash::step(float dt) {

  direction = normalize(direction);

	dodge.direction = direction;
	dodge.step(dt);

	reorient.target_orientation = look_at(vec3(direction) + vec3{ 0.0, 0.0, 0.4f });
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

	timer += dt;

	if (timer > 0.25f && car.on_ground && norm(xy(car.angular_velocity)) < 0.5f) {
		finished = true;
	}

}

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
//  float dt = 0.01666f;
//  for (float t = dt; t < 5.0f; t += dt) {
//
//    // get the new controls
//    copy.step(dt); 
//
//    // and simulate their effect on the car
//    car_copy.step(copy.controls, dt); 
//
//    if (copy.finished) break;
//  }
//
//  outfile.close();
//
//  rereorient car_copy;
//
//}
