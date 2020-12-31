#include <fstream>
#include <iostream>

#include <Windows.h>

#include "misc/timer.h"
#include "misc/logging.h"

#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

#include "rlbot/gamepad.h"

int main(int argc, char** argv) {

  std::string filename("log.ndjson");

  if (argc == 2) {
    filename = std::string(argv[1]);
  }

  std::ofstream outfile; 

  std::string interface_dll = std::string(DLLNAME);

  // establish our connection to the RLBot interface
  Interface::LoadInterface(interface_dll);

  int code = Interface::StartMatch();

  timer stopwatch;
  stopwatch.start();
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
        XINPUT_STATE gamepadState;
        XInputGetState(0, &gamepadState);
        Input controls = GamePad::GetOutput();

        if (gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
          if (!outfile.is_open()) {
            std::cout << "opening file" << std::endl;
            outfile.open(filename);
          }

          nlohmann::json output;
          stopwatch.stop();
          output["actual_time"] = stopwatch.elapsed();
          output["time"] = g.time;
          output["car"] = to_json(g.cars[0]);
          output["inputs"] = to_json(controls);
          outfile << output << std::endl;

          std::cout << "logging..." << std::endl;

        } else {
          if (outfile.is_open()) {
            std::cout << "closing file" << std::endl;
            outfile.close();
          }
        }
        int status = Interface::SetBotInput(controls, 0);
        break;
    }
  }

  return 0;
}
