#pragma once

#include <windows.h>
#include <xinput.h>

#include "simulation/input.h"

namespace GamePad {

inline Input GetOutput() {
  XINPUT_STATE gamepadState;

  Input controls;
  XInputGetState(0, &gamepadState);

  controls.steer = gamepadState.Gamepad.sThumbLX / float(INT16_MAX);
  controls.throttle = gamepadState.Gamepad.bRightTrigger / (float)UINT8_MAX -
                      gamepadState.Gamepad.bLeftTrigger / (float)UINT8_MAX;

  controls.jump = (gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_A) > 0;
  controls.boost = (gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_B) > 0;
  controls.handbrake = (gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_X) > 0;
  controls.useItem = (gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) > 0;

  if ((gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) > 0) {
    controls.roll = gamepadState.Gamepad.sThumbLX / float(INT16_MAX);
  } else {
    controls.yaw = gamepadState.Gamepad.sThumbLX / float(INT16_MAX);
  }

  controls.pitch = -(gamepadState.Gamepad.sThumbLY / float(INT16_MAX));

  return controls;
}

};  // namespace GamePad
