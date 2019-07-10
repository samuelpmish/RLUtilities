#include <iostream>

#include "rlbot/bot.h"
#include "rlbot/botmanager.h"
#include "rlbot/examplebot.h"
#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

#include "rlbot/gamepad.h"

int main(int argc, char** argv) {

  std::string interface_dll = std::string(DLLNAME);

  // establish our connection to the RLBot interface
  Interface::LoadInterface(interface_dll);

  int code = Interface::StartMatch();

  std::cout << "error code: " << code << std::endl;

  Game g;

  while (true) {

    // request the latest info from the framework
    UpdateStatus status = g.GetState();

    switch (status) {
      case UpdateStatus::OldData:
        Sleep(1);
        break;
      case UpdateStatus::InvalidData:
        Sleep(100);
        break;
      case UpdateStatus::NewData:
        int status = Interface::SetBotInput(GamePad::GetOutput(), 0);
        break;
    }
  }

  return 0;
}
