#include <fstream>
#include <iostream>
#include <Windows.h>

#include "misc/random.h"
#include "misc/logging.h"

#include "simulation/car.h"
#include "simulation/field.h"

#include "experimental/navigator.h"
#include "experimental/follow_path.h"

#include "rlbot/renderer.h"
#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

#include "rlbot/gamepad.h"

int main(int argc, char** argv) {

  std::string state = "setup";

  std::string filename("backboard.ndjson");

  if (argc == 2) {
    filename = std::string(argv[1]);
  }

  std::ofstream outfile(filename); 

  // establish our connection to the RLBot interface
  std::cout << "Loading Interface...";
  Interface::LoadInterface(std::string(DLLNAME));
  std::cout << "done" << std::endl;

  int code = Interface::StartMatch();
  std::cout << "StartMatch: " << code << std::endl;

  Color red{255, 0, 0, 255};
  Color blue{0, 0, 255, 255};

  Renderer r(1);
  Game g;
  Game::set_mode(std::string("soccar"));

  Navigator nav(g.cars[0]);
  nav.debug = true;

  FollowPath follow(g.cars[0]);

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

        nlohmann::json output;

        if (state == "setup") {

          g.ball.position = vec3{0, -3500.0, 100.0f}; 
          g.ball.velocity = vec3{random::real(-300.0f, 300.0f), 2500.0, 1500.0f}; 

          float theta = random::real(0.0f, 0.0f);
          vec3 f = vec3{sin(theta), cos(theta), 0.0f};

          g.cars[0].position = vec3{random::real(-3000.0f, -1000.0f), 2000.0, 17.0f}; 
          g.cars[0].orientation = look_at(f);
          g.cars[0].velocity = f * 1000.0f;
          g.cars[0].angular_velocity = vec3{0.0f, 0.0f, 0.0f};

          std::cout << g.SetState() << std::endl;
          
          nav.analyze_surroundings(100);

          Ball prediction = g.ball;
          float vy_old = g.ball.velocity[1];

          for (int i = 0; i < 600; i++) {
            prediction.step(1.0 / 120.0);

            if (fabs(prediction.velocity[1] - vy_old) > 400) {
              std::cout << "found a bounce at " << prediction.position << std::endl;

              vec3 direction = vec3{1.0, 0, 0.0};
              vec3 destination = prediction.position - 75.0f * direction;
              Curve path = nav.path_to(destination, direction, 150.0f);

              float arrival_time = prediction.time + 0.03f;
              float arrival_speed = 1600.0;
              follow.calculate_plan(path, arrival_time, arrival_speed);

              output["follow"] = to_json(follow);
              output["path"] = to_json(follow.path);

              break; 
            }
            vy_old = prediction.velocity[1];
          }

          state = "running";

        }

        output["time"] = g.time;
        output["car"] = to_json(g.cars[0]);

        follow.step(g.time_delta);

        float s = follow.path.find_nearest(g.cars[0].position);
        vec2 expectation = follow.plan(follow.arrival_time - g.time);
        output["expected_s"] = expectation[0];
        output["expected_speed"] = expectation[1];
        output["expected_position"] = to_json(follow.path.point_at(expectation[0]));

        output["s"] = s;
        output["inputs"] = to_json(follow.controls);

        outfile << output << std::endl;

        r.Start();
        r.DrawPolyLine3D(red, follow.path.points);
        r.DrawString2D(red, std::to_string(follow.arrival_time - g.time), vec2{100, 100}, 4, 4);
        r.DrawSphere(red, sphere{follow.path.point_at(expectation[0]), 50.0f});
        r.DrawSphere(blue, sphere{g.cars[0].position, 50.0f});
        r.Finish();

        int status = Interface::SetBotInput(follow.controls, 0);

        if (follow.arrival_time - g.time < -1.5) {
          state = "setup";
        }

        break;
    }
  }

  return 0;
}
