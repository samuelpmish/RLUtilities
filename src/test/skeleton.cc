#include <iostream>

#include "rlbot/bot.h"
#include "rlbot/botmanager.h"
#include "rlbot/examplebot.h"
#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

int main(int argc, char** argv) {

  std::string interface_dll = std::string(DLLNAME);

  // establish our connection to the RLBot interface
  Interface::LoadInterface(interface_dll);

  int code = Interface::StartMatch();

  std::cout << "error code: " << code << std::endl;

  // wait for everything to be initialized
  while (!Interface::IsInitialized()) {
    Sleep(100);
  }

  Game g;

  int count = 0;

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

        Input controls;
        controls.throttle = 1.0;
        controls.steer = sin(g.time);
        controls.roll = 0.0;
        controls.pitch = 0.0;
        controls.yaw = 0.0;
        controls.jump = 0;
        controls.boost = 0;
        controls.handbrake = 0;
        controls.useItem = 0;

        int status = Interface::SetBotInput(controls, 0);

        if (count++ % 500 == 0) {
          Game desired = g;
          desired.cars[0].position[2] += 500.0f;
          desired.SetState();
        }

        break;
    }
  }

  return 0;
}
