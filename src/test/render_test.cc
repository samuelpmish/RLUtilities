#include <iostream>
#include <Windows.h>

#include "rlbot/gamepad.h"
#include "rlbot/renderer.h"
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

  Game g(botIndex, botTeam);

  Color red{255, 0, 0, 255};
  Color blue{0, 0, 255, 255};

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

        Renderer r;
        for (int i = 0; i < 300; i++) {
          r.DrawLine3D(Color{min(i, 255), 0, 0, 255}, vec3{-2000.0f, i * 10.0f, 300.0f}, vec3{2000.0f, i * 10.0f, 300.0f});
        }
        r.Finish();

        int status = Interface::SetBotInput(GamePad::GetOutput(), botIndex);
        break;
    }
  }

  return 0;
}
