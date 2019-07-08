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
  g.set_mode("soccar");

  Color red{255, 0, 0, 255};
  Color green{0, 255, 0, 255};
  Color blue{0, 0, 255, 255};
  Color white{255, 255, 255, 255};
  Renderer r(0);

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
        r.Start();
        r.DrawSphere(Color::red(), sphere{g.ball.position, 100.0f});
        r.DrawOBB(blue, g.cars[0].hitbox());
        r.DrawString2D(green, std::string("2D text"), vec2{100, 100}, 4, 4);
        r.DrawString3D(green, std::string("3D text"), g.ball.position + vec3{0, 0, 200}, 4, 4);
        std::vector < vec3 > polyline;
        Ball b(g.ball);
        for (int i = 0; i < 240; i++) {
          b.step(0.008333);
          b.step(0.008333);
          polyline.push_back(b.position);
        }
        r.DrawPolyLine3D(white, polyline);
        r.Finish();
        int status = Interface::SetBotInput(GamePad::GetOutput(), botIndex);
        break;
    }
  }

  return 0;
}
