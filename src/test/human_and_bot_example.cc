#include <iostream>

#include "mechanics/drive.h"

#include "bot.h"
#include "botmanager.h"
#include "interface.h"
#include "rlbot_generated.h"

#include "misc/gamepad.h"
#include "simulation/game.h"

int main(int argc, char** argv) {

  std::string interface_dll = std::string(DLLNAME);

  // establish our connection to the RLBot interface
  rlbot::Interface::LoadInterface(interface_dll);

  int num_players = 2;
  rlbot::MatchSettings match_settings;
  // TODO setup match settings for two players
  int code = rlbot::Interface::StartMatch(match_settings);

  std::cout << "error code: " << code << std::endl;

  Game g;

  Drive drive_toward_player(g.cars[1]);

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
        // human is player 0
        int status1 = rlbot::Interface::SetBotInput(GamePad::GetOutput().to_controller(), 0);

        drive_toward_player.target = g.cars[0].position;
        drive_toward_player.speed = 500.0f;
        drive_toward_player.step(1.0f / 120.0f);

        // bot is player 1
        int status2 = rlbot::Interface::SetBotInput(drive_toward_player.controls.to_controller(), 1);
        break;
    }
  }

  return 0;
}
