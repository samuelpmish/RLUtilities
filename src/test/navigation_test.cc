#include <fstream>
#include <iostream>
#include <Windows.h>

#include "misc/logging.h"

#include "simulation/car.h"
#include "simulation/field.h"

#include "experimental/navigator.h"
#include "experimental/follow_path.h"

#include "rlbot/renderer.h"
#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

#include "rlbot/gamepad.h"

struct waypoint {
  vec3 pos;
  vec3 tangent;
  float time;
};

int main(int argc, char** argv) {

  std::string filename("log.ndjson");

  if (argc == 2) {
    filename = std::string(argv[1]);
  }

  std::ofstream outfile(filename); 

  // establish our connection to the RLBot interface
  Interface::LoadInterface(std::string(DLLNAME));

  int code = Interface::StartMatch();

  Color red{255, 0, 0, 255};
  Color blue{0, 0, 255, 255};

  Renderer r(1);
  Game g;
  Game::set_mode(std::string("soccar"));

  Navigator nav(g.cars[0]);
  nav.debug = true;

  FollowPath follow(g.cars[0]);
  follow.arrival_time = g.time - 5.0;
  follow.arrival_speed = 1200;

  std::vector < waypoint > waypoints = {
    waypoint{vec3{0, 3000, 0}, vec3{-1, 0, 0}, 6.0f},
    waypoint{vec3{0, 2000, 0}, vec3{1, 0, 0}, 4.0f},
    waypoint{vec3{0, 1000, 0}, vec3{-1, 0, 0}, 3.5f},
    waypoint{vec3{0, 0, 0}, vec3{1, 0, 0}, 3.0f},
    waypoint{vec3{2000, 0, 0}, vec3{0, 1, 0}, 4.0f},
    waypoint{vec3{0, 2000, 0}, vec3{1, -1, 0}, 4.0f},
    waypoint{vec3{4096, 1000, 700}, vec3{0, 0, 1}, 4.0f},
    waypoint{vec3{4096, -2000, 1200}, vec3{0, -1, 1}, 4.0f}
  };

  int which = -1;
  float pos_error = 0;
  float vel_error = 0;

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
        output["time"] = g.time;
        output["car"] = to_json(g.cars[0]);

        if (which == -1 || follow.arrival_time - g.time < 0) {

          pos_error = norm(g.cars[0].position - waypoints[which].pos); 
          vel_error = norm(g.cars[0].velocity) - follow.arrival_speed; 

          which++;
          if (which >= waypoints.size()) {
            outfile.close();
            break;
          }
            
          nav.analyze_surroundings(100);

          vec3 destination = waypoints[which].pos;
          vec3 direction = waypoints[which].tangent;

          Curve path = nav.path_to(destination, direction, 150.0f);
          float arrival_time = g.time + waypoints[which].time;
          float arrival_speed = 1000.0f;

          follow.calculate_plan(path, arrival_time, arrival_speed);
          
          output["follow"] = to_json(follow);
          output["path"] = to_json(follow.path);
          outfile << output << std::endl;

        }

        follow.step(g.time_delta);

        float s = follow.path.find_nearest(g.cars[0].position);
        vec2 expectation = follow.plan(follow.arrival_time - g.time);
        output["expected_s"] = expectation[0];
        output["expected_speed"] = expectation[1];
        output["expected_position"] = to_json(follow.path.point_at(expectation[0]));

        output["s"] = s;
        output["follow"] = to_json(follow);
        output["inputs"] = to_json(follow.controls);

        outfile << output << std::endl;

        r.Start();
        r.DrawPolyLine3D(red, follow.path.points);
        r.DrawString2D(red, std::to_string(follow.arrival_time - g.time), vec2{100, 100}, 4, 4);
        r.DrawString2D(red, std::to_string(pos_error), vec2{100, 200}, 4, 4);
        r.DrawString2D(red, std::to_string(vel_error), vec2{100, 300}, 4, 4);
        r.DrawSphere(red, sphere{follow.path.point_at(expectation[0]), 50.0f});
        r.DrawSphere(blue, sphere{g.cars[0].position, 50.0f});
        r.Finish();

        int status = Interface::SetBotInput(follow.controls, 0);
        break;
    }
  }

  return 0;
}
