#include <iostream>

#include "bot.h"
#include "botmanager.h"
#include "interface.h"
#include "rlbot_generated.h"
#include "platform.h"

#include "misc/gamepad.h"

#include "simulation/game.h"

int main(int argc, char** argv) {

  std::string interface_dll = std::string(DLLNAME);

  // establish our connection to the RLBot interface
  rlbot::Interface::LoadInterface(interface_dll);

  int code = rlbot::Interface::StartMatch(rlbot::MatchSettings());

  std::cout << "error code: " << code << std::endl;

  Game g;

  while (true) {

    // request the latest info from the framework
    UpdateStatus status = g.GetState();

    switch (status) {
      case UpdateStatus::OldData:
        rlbot::platform::SleepMilliseconds(1);
        break;
      case UpdateStatus::InvalidData:
        rlbot::platform::SleepMilliseconds(100);
        break;
      case UpdateStatus::NewData:
        int status = rlbot::Interface::SetBotInput(GamePad::GetOutput().to_controller(), 0);
        break;
    }
  }

  return 0;
}
