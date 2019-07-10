#include <fstream>
#include <iostream>

#include <Windows.h>

#include "misc/timer.h"
#include "misc/logging.h"

#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

#include "rlbot/gamepad.h"

int main(int argc, char** argv) {

  std::ofstream outfile; 

  std::string interface_dll = std::string(DLLNAME);

  // establish our connection to the RLBot interface
  Interface::LoadInterface(interface_dll);

  int code = Interface::StartMatch();

  std::string state = "setup";

  float time;
  int speed = 200;

  timer stopwatch;
  stopwatch.start();
  Game g;

  Drive d(g.cars[0]);

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

        if (state == "setup") {

          d.speed = speed;

          g.cars[0].position = vec3{3000.0f, 3000.0f, 17.01f};
          g.cars[0].velocity = vec3{0.0f, -d.speed, 0.0f};
          g.cars[0].angular_velocity = vec3{0.0f, 0.0f, 0.0f};
          g.cars[0].orientation = look_at(vec3{0.0f, -1.0f, 0.0f});

          g.SetState();

          time = g.time;

          state = "running";

          outfile.open(std::string("steering_") + std::to_string(speed) + std::string(".ndjson"));

        } 

        float T = g.time - time;

        d.step(g.time_delta);
        Input controls = d.controls;

        if (T < 0.5f) {
          controls.steer = 0.0f;
        }

        if (0.5f < T && T < 2.0f) {
          controls.steer = -1.0f;
        }

        if (2.0 < T && T < 3.5) {
          controls.steer =  1.0f;
        }

        nlohmann::json output;
        stopwatch.stop();
        output["time"] = g.time;
        output["car"] = to_json(g.cars[0]);
        output["inputs"] = to_json(controls);
        outfile << output << std::endl;

        int status = Interface::SetBotInput(controls, 0);

        if (T > 3.5) {
          state = "setup";
          outfile.close();

          speed += 50;
          if (speed > 2300) exit(0);
        }

        break;
    }
  }

  return 0;
}
