#include "simulation/game.h"
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
vec3 vector3_to_vec3(pybind11::object vector3) {
	return vec3{
		vector3.attr("x").cast<float>(),
    vector3.attr("y").cast<float>(),
		vector3.attr("z").cast<float>()
	};
}

mat3 rotator_to_mat3(pybind11::object rotator) {
	return euler_to_rotation(vec3{
		rotator.attr("pitch").cast<float>(),
		rotator.attr("yaw").cast<float>(),
		rotator.attr("roll").cast<float>() 
  });
}

void Game::read_game_information(pybind11::object gametick,
                                 pybind11::object phystick,
                                 pybind11::object fieldinfo) {

  pybind11::object game_info = gametick.attr("game_info"); 
  num_cars = gametick.attr("num_cars").cast<int>();

  if (num_cars > 8) {
    std::cout << "playing with more than 8 cars is not supported by rlutilities\n";
  } else {

    float time_old = time;
  
    time = game_info.attr("seconds_elapsed").cast<float>();
    time_delta = time - time_old;

    int frame_old = frame;
    frame = phystick.attr("ball").attr("state").attr("frame").cast<int>();
    frame_delta = frame - frame_old;

    time_remaining = game_info.attr("game_time_remaining").cast<float>();
    overtime = game_info.attr("is_overtime").cast<bool>();
    round_active = game_info.attr("is_round_active").cast<bool>();
    kickoff_pause = game_info.attr("is_kickoff_pause").cast<bool>();
    match_ended = game_info.attr("is_match_ended").cast<bool>();
    gravity = game_info.attr("world_gravity_z").cast<float>();

    pybind11::list game_cars = gametick.attr("game_cars");
    pybind11::list players = phystick.attr("players");
    for (int i = 0; i < num_cars; i++) {

      pybind11::object game_car = game_cars[i];

      pybind11::object gametick_physics = game_car.attr("physics");
      pybind11::object rigidbody_physics = players[i].attr("state");
      pybind11::object inputs = players[i].attr("input");

      cars[i].position = vector3_to_vec3(gametick_physics.attr("location"));
      cars[i].velocity = vector3_to_vec3(gametick_physics.attr("velocity"));
      cars[i].angular_velocity = vector3_to_vec3(gametick_physics.attr("angular_velocity"));
      cars[i].orientation = rotator_to_mat3(gametick_physics.attr("rotation"));

      // other car data
      cars[i].boost = game_car.attr("boost").cast<int>();
      cars[i].jumped = game_car.attr("jumped").cast<bool>();
      cars[i].double_jumped = game_car.attr("double_jumped").cast<bool>();
      cars[i].on_ground = game_car.attr("has_wheel_contact").cast<bool>();
      cars[i].supersonic = game_car.attr("is_super_sonic").cast<bool>();
      cars[i].team = game_car.attr("team").cast<int>();

      cars[i].time = time;

      cars[i].controls.throttle = inputs.attr("throttle").cast<float>();
      cars[i].controls.steer = inputs.attr("steer").cast<float>();
      cars[i].controls.pitch = inputs.attr("pitch").cast<float>();
      cars[i].controls.yaw = inputs.attr("yaw").cast<float>();
      cars[i].controls.roll = inputs.attr("roll").cast<float>();
      cars[i].controls.jump = inputs.attr("jump").cast<bool>();
      cars[i].controls.boost = inputs.attr("boost").cast<bool>();
      cars[i].controls.handbrake = inputs.attr("handbrake").cast<bool>();

    }

    pybind11::object physics = gametick.attr("game_ball").attr("physics");

    ball.position = vector3_to_vec3(physics.attr("location"));
    ball.velocity = vector3_to_vec3(physics.attr("velocity"));
    ball.angular_velocity = vector3_to_vec3(physics.attr("angular_velocity"));

    ball.time = time;

    int num_boost_pads = fieldinfo.attr("num_boosts").cast<int>();
    pybind11::list boost_pads = fieldinfo.attr("boost_pads");

    int num_game_boosts = gametick.attr("num_boost").cast<int>();
    pybind11::list game_boosts = gametick.attr("game_boosts");

    if (num_boost_pads != num_game_boosts) {
      std::cout << "boost pad info mismatch" << std::endl;
    } else {

      if (num_boost_pads > pads.size()) pads.resize(num_boost_pads);
  
      for (int i = 0; i < num_boost_pads; i++) {
        pybind11::object boost_pad = boost_pads[i];
        pybind11::object game_boost = game_boosts[i];
  
        pads[i].position = vector3_to_vec3(boost_pads[i].attr("location"));
        pads[i].is_full_boost = boost_pad.attr("is_full_boost").cast<bool>();
        pads[i].is_active = game_boost.attr("is_active").cast<bool>();
        pads[i].timer = game_boost.attr("timer").cast<float>();
      }

    }

  }

}

void init_game(pybind11::module & m) {
	pybind11::class_<Game>(m, "Game")
		.def(pybind11::init())
		.def_readwrite("time", &Game::time)
		.def_readwrite("time_delta", &Game::time_delta)
		.def_readwrite("time_remaining", &Game::time_remaining)
		.def_readwrite("frame", &Game::frame)
		.def_readwrite("frame_delta", &Game::frame_delta)
		.def_readwrite("num_cars", &Game::num_cars)
		.def_readwrite("overtime", &Game::overtime)
		.def_readwrite("round_active", &Game::round_active)
		.def_readwrite("kickoff_pause", &Game::kickoff_pause)
		.def_readwrite("match_ended", &Game::match_ended)
		.def_readwrite("ball", &Game::ball)
		.def_readwrite("cars", &Game::cars)
		.def_readonly("pads", &Game::pads)
    .def_readonly_static("map", &Game::map)
    .def_readonly_static("gravity", &Game::gravity)
    .def_readonly_static("frametime", &Game::frametime)
    .def_static("set_mode", &Game::set_mode)
    .def("read_game_information", &Game::read_game_information);
}
