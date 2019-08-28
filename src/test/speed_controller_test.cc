#include <iostream>
#include <Windows.h>

#include "simulation/car.h"
#include "simulation/field.h"
#include "simulation/game.h"

#include "experimental/navigator.h"
#include "experimental/follow_path.h"

#include "interface.h"
#include "rlbot_generated.h"
#include "platform.h"

#include "misc/rlurenderer.h"
#include "misc/gamepad.h"


struct waypoint {
  vec3 pos;
  vec3 tangent;
  float time;
};

int main(int argc, char** argv) {

  // establish our connection to the RLBot interface
  rlbot::Interface::LoadInterface(std::string(DLLNAME));

  int code = rlbot::Interface::StartMatch(rlbot::MatchSettings());

  rlbot::Color red{255, 0, 0, 255};

  Game g;
  Game::set_mode(std::string("soccar"));

  Navigator nav(g.cars[0]);
  nav.debug = true;

  FollowPath follow(g.cars[0]);

  std::vector < waypoint > waypoints = {
    waypoint{vec3{0, 3000, 0}, vec3{-1, 0, 0}, 5.0f},
    waypoint{vec3{0, 2000, 0}, vec3{1, 0, 0}, 5.0f},
    waypoint{vec3{0, 1000, 0}, vec3{-1, 0, 0}, 5.0f},
    waypoint{vec3{0, 0, 0}, vec3{1, 0, 0}, 5.0f},
    waypoint{vec3{2000, 0, 0}, vec3{0, 1, 0}, 10.0f},
    waypoint{vec3{0, 2000, 0}, vec3{1, -1, 0}, 15.0f},
    waypoint{vec3{4096, 1000, 700}, vec3{0, 0, 1}, 20.0f},
    waypoint{vec3{4096, -2000, 1200}, vec3{0, -1, 1}, 25.0f}
  };

  int which = -1;

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
        if (which == -1 || norm(g.cars[0].position - waypoints[which].pos) < 50.0f) {
          which++;
          if (which >= waypoints.size()) break;
          nav.analyze_surroundings(100);
          follow.path = nav.path_to(waypoints[which].pos, waypoints[which].tangent, 500.0f);

          follow.arrival_time = g.time + 6.0;
          follow.arrival_speed = 1500;

        }

        follow.step(g.time_delta);

        RLURenderer r("test");
        r.DrawPolyLine3D(red, follow.path.points);
        r.DrawString2D(red, std::to_string(follow.arrival_time - g.time), vec2{100, 100}, 4, 4);

        int status = rlbot::Interface::SetBotInput(follow.controls.to_controller(), 0);
        break;
    }
  }

  return 0;
}
