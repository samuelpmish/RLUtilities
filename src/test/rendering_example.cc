#include <iostream>
#include <Windows.h>

#include "rlbot/gamepad.h"
#include "rlbot/renderer.h"
#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

int main(int argc, char** argv) {

  std::string interface_dll = std::string(DLLNAME);

  // establish our connection to the RLBot interface
  Interface::LoadInterface(interface_dll);

  int code = Interface::StartMatch();

  std::cout << "error code: " << code << std::endl;

  Game g;
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
        int status = Interface::SetBotInput(GamePad::GetOutput(), 0);
        break;
    }
  }

  return 0;
}
