#include <iostream>

#include "rlbot/bot.h"
#include "rlbot/botmanager.h"
#include "rlbot/examplebot.h"
#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

int main(int argc, char** argv) {
  int botIndex = 0;
  int botTeam = 0;
  std::string botName = "";

  std::string interface_dll = std::string(DLLNAME);

  // parse arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);

    if ((arg == "-index") && i + 1 < argc) {
      botIndex = atoi(argv[++i]);
    } else if ((arg == "-team") && i + 1 < argc) {
      botTeam = atoi(argv[++i]);
    } else if ((arg == "-name") && i + 1 < argc) {
      botName = std::string(argv[++i]);
    } else if ((arg == "-dll-path") && i + 1 < argc) {
      interface_dll = std::string(argv[++i]) + "\\" + DLLNAME;
    } else {
      std::cerr << "Bad option: '" << arg << "'" << std::endl;
    }
  }

  // establish our connection to the RLBot interface
  Interface::LoadInterface(interface_dll);

  int code = Interface::StartMatch();

  std::cout << "error code: " << code << std::endl;

  //// wait for everything to be initialized
  //while (!Interface::IsInitialized()) {
  //  Sleep(100);
  //}

  Game g(botIndex, botTeam);

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

        int status = Interface::SetBotInput(controls, botIndex);

        if (count++ % 500 == 0) {
          Game desired = g;
          desired.cars[0].x[2] += 500.0f;
          desired.SetState();
        }

        break;
    }
  }

  return 0;
}
