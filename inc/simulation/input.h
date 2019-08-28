#pragma once
#include "controller.h"

struct Input {
  float steer;
  float roll;
  float pitch;
  float yaw;
  float throttle;
  bool jump;
  bool boost;
  bool handbrake;
  bool useItem;

  Input() {
    steer = 0.0;
    roll = 0.0;
    pitch = 0.0;
    yaw = 0.0;
    throttle = 0.0;
    jump = false;
    boost = false;
    handbrake = false;
	  useItem = false;
  }

  rlbot::Controller to_controller()
  {
    rlbot::Controller controller;
    controller.steer = steer;
    controller.roll = roll;
    controller.pitch = pitch;
    controller.yaw = yaw;
    controller.throttle = throttle;
    controller.jump = jump;
    controller.boost = boost;
    controller.handbrake = handbrake;
    controller.useItem = useItem;
    return controller;
  }
};
