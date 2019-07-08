#include <iostream>

#include "mechanics/drive.h"

#include "rlbot/bot.h"
#include "rlbot/botmanager.h"
#include "rlbot/examplebot.h"
#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

#include "rlbot/gamepad.h"

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

  int num_players = 2;
  int code = Interface::StartMatch(num_players);

  std::cout << "error code: " << code << std::endl;

  Game g(botIndex, botTeam);

  Drive drive_toward_player(g.cars[1]);

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
        // human is player 0
        int status1 = Interface::SetBotInput(GamePad::GetOutput(), 0);

        drive_toward_player.target = g.cars[0].position;
        drive_toward_player.speed = 500.0f;
        drive_toward_player.step(1.0f / 120.0f);

        // bot is player 1
        int status2 = Interface::SetBotInput(drive_toward_player.controls, 1);
        break;
    }
  }

  return 0;
}
