#pragma once

#include <vector>

#include "simulation/car.h"
#include "simulation/ball.h"
#include "simulation/pad.h"
#include "simulation/goal.h"

#include <pybind11/pybind11.h>

class GameData {
 public:
	// initialization
	GameData() {}
	GameData(std::string name, int team, int index) : name(name), team(team), index(index) {}

	std::string name;
	int team;
	int index;

	// cars
	const std::vector<Car> &GetCars() const { return cars; }

	const Car &GetMyCar() const { return *my_car; }

	const std::vector<Car*> &GetOpponents() const { return opponents; }
	const std::vector<Car*> &GetTeammates() const { return teammates; }

	// ball
	const Ball &GetBall() const { return ball; }

	// boost pads
	const std::vector<Pad*> &GetLargePads() const { return large_pads; }
	const std::vector<Pad*> &GetSmallPads() const { return small_pads; }

	// goals
	const std::vector<Goal*> &GetOppGoals() const { return opp_goals; }
	const std::vector<Goal*> &GetOwnGoals() const { return own_goals; }

	// game info
	float time;
	float time_remaining;
	bool overtime;
	bool round_active;
	bool kickoff_pause;
	bool match_ended;
	float world_gravity_z;
	float game_speed;

	// read from packets
	void read_game_tick_packet(pybind11::object gtp);
	void read_game_cars(pybind11::list game_cars, int num_cars);
	void read_game_boosts(pybind11::list game_boosts);
	void read_game_info(pybind11::object game_info);

	void read_field_info(pybind11::object field_info);
	void read_boost_pads(pybind11::list boost_pads, int num_boosts);
	void read_goals(pybind11::list goals, int num_goals);

 private:

	// cars
	std::vector<Car> cars;

	Car* my_car;

	std::vector<Car*> opponents;
	std::vector<Car*> teammates;

	// ball
	Ball ball;

	// boost pads
	std::vector<Pad*> large_pads;
	std::vector<Pad*> small_pads;

	// goals
	std::vector<Goal*> opp_goals;
	std::vector<Goal*> own_goals;
};
