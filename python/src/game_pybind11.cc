#include "simulation/game.h"
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

vec3 vector3_to_vec3(const pybind11::object& vector3) {
	return vec3{
		vector3.attr("x").cast<float>(),
        vector3.attr("y").cast<float>(),
		vector3.attr("z").cast<float>()
	};
}

mat3 rotator_to_mat3(const pybind11::object& rotator) {
	return euler_to_rotation(vec3{
		rotator.attr("pitch").cast<float>(),
		rotator.attr("yaw").cast<float>(),
		rotator.attr("roll").cast<float>() 
  });
}

void Game::read_field_info(const pybind11::object& field_info) {
    int num_boost_pads = field_info.attr("num_boosts").cast<int>();
    pybind11::list boost_pads = field_info.attr("boost_pads");
    pads.resize(num_boost_pads);

    for (int i = 0; i < num_boost_pads; i++) {
        pybind11::object boost_pad = boost_pads[i];
        pads[i].position = vector3_to_vec3(boost_pad.attr("location"));
        if (boost_pad.attr("is_full_boost").cast<bool>()) {
            pads[i].type = BoostPadType::Full;
        } else {
            pads[i].type = BoostPadType::Partial;
        }
    }

    int num_goals = field_info.attr("num_goals").cast<int>();
    pybind11::list field_info_goals = field_info.attr("goals");
    goals.resize(num_goals);

    for (int i = 0; i < num_goals; ++i) {
        pybind11::object goal = field_info_goals[i];
        goals[i].position = vector3_to_vec3(goal.attr("location"));
        goals[i].direction = vector3_to_vec3(goal.attr("direction"));
        goals[i].team = goal.attr("team_num").cast<int>();
        goals[i].width = goal.attr("width").cast<float>();
        goals[i].height = goal.attr("height").cast<float>();
        goals[i].state = GoalState::Unknown;
    }
}

void Game::read_packet(const pybind11::object& packet) {
    // game info
    pybind11::object game_info = packet.attr("game_info");

    float current_time = game_info.attr("seconds_elapsed").cast<float>();
    time_delta = current_time - time;
    time = current_time;

    time_remaining = game_info.attr("game_time_remaining").cast<float>();
    float gravity_z = game_info.attr("world_gravity_z").cast<float>();
    gravity = {0.0f, 0.0f, gravity_z};

    if (game_info.attr("is_match_ended").cast<bool>()) {
        state = GameState::Ended;
    } else {
        if (game_info.attr("is_round_active").cast<bool>()) {
            if (game_info.attr("is_kickoff_pause").cast<bool>()) {
                state = GameState::Kickoff;
            } else {
                state = GameState::Active;
            }
        } else {
            state = GameState::Inactive;
        }
    }

    // cars
    int num_cars = packet.attr("num_cars").cast<int>();
    pybind11::list game_cars = packet.attr("game_cars");
    if (num_cars != cars.size()) {
        cars.resize(num_cars);
    }

    for (int i = 0; i < num_cars; i++) {
        pybind11::object game_car = game_cars[i];

        pybind11::object car_physics = game_car.attr("physics");
        cars[i].position = vector3_to_vec3(car_physics.attr("location"));
        cars[i].velocity = vector3_to_vec3(car_physics.attr("velocity"));
        cars[i].angular_velocity = vector3_to_vec3(car_physics.attr("angular_velocity"));
        cars[i].orientation = rotator_to_mat3(car_physics.attr("rotation"));

        cars[i].boost = game_car.attr("boost").cast<int>();
        cars[i].jumped = game_car.attr("jumped").cast<bool>();
        cars[i].double_jumped = game_car.attr("double_jumped").cast<bool>();
        cars[i].on_ground = game_car.attr("has_wheel_contact").cast<bool>();
        cars[i].supersonic = game_car.attr("is_super_sonic").cast<bool>();
        cars[i].demolished = game_car.attr("is_demolished").cast<bool>();
        cars[i].team = game_car.attr("team").cast<int>();

        cars[i].time = time;
        cars[i].id = i;

        cars[i].hitbox_widths[0] = game_car.attr("hitbox").attr("length").cast<float>() * 0.5f;
        cars[i].hitbox_widths[1] = game_car.attr("hitbox").attr("width").cast<float>() * 0.5f;
        cars[i].hitbox_widths[2] = game_car.attr("hitbox").attr("height").cast<float>() * 0.5f;

        cars[i].hitbox_offset = vector3_to_vec3(game_car.attr("hitbox_offset"));
    }

    // ball
    pybind11::object ball_physics = packet.attr("game_ball").attr("physics");
    ball.position = vector3_to_vec3(ball_physics.attr("location"));
    ball.velocity = vector3_to_vec3(ball_physics.attr("velocity"));
    ball.angular_velocity = vector3_to_vec3(ball_physics.attr("angular_velocity"));
    ball.time = time;

    // boost pads
    int num_game_boosts = packet.attr("num_boost").cast<int>();
    pybind11::list game_boosts = packet.attr("game_boosts");

    if (pads.size() != num_game_boosts) {
        std::cout << "Boost pad info mismatch. Make sure to call read_field_info before read_packet!" << std::endl;
    } else {
        for (int i = 0; i < num_game_boosts; i++) {
            pybind11::object game_boost = game_boosts[i];

            if (game_boost.attr("is_active").cast<bool>()) {
                pads[i].state = BoostPadState::Available;
            } else {
                pads[i].state = BoostPadState::Unavailable;
            }
            pads[i].timer = game_boost.attr("timer").cast<float>();
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
		.def_readwrite("state", &Game::state)
		.def_readwrite("ball", &Game::ball)
		.def_readwrite("cars", &Game::cars)
		.def_readonly("pads", &Game::pads)
		.def_readonly("goals", &Game::goals)
        .def_readonly_static("map", &Game::map)
        .def_readwrite_static("gravity", &Game::gravity)
        .def_static("set_mode", &Game::set_mode, pybind11::arg("mode"))
        .def("read_field_info", &Game::read_field_info, pybind11::arg("field_info"))
        .def("read_packet", &Game::read_packet, pybind11::arg("packet"));

	pybind11::enum_<GameState>(m, "GameState")
	    .value("Active", GameState::Active)
	    .value("Countdown", GameState::Countdown)
	    .value("Ended", GameState::Ended)
	    .value("GoalScored", GameState::GoalScored)
	    .value("Inactive", GameState::Inactive)
	    .value("Kickoff", GameState::Kickoff)
	    .value("Paused", GameState::Paused)
	    .value("Replay", GameState::Replay);
}