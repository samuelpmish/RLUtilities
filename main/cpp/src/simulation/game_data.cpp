#include "simulation/game_data.h"
#include "misc/convert.h"
#include <algorithm>



void GameData::read_game_tick_packet(pybind11::object gtp)
{
	/* Reads an instance of GameTickPacket provided by the rlbot framework,
	and converts it's contents into our internal structure.*/

	int num_cars = gtp.attr("num_cars").cast<int>();

	if (num_cars != 0)
		read_game_cars(gtp.attr("game_cars"), num_cars);
	
	ball.read_game_ball(gtp.attr("game_ball"));

	if (gtp.attr("num_boost").cast<int>() != 0)
		read_game_boosts(gtp.attr("game_boosts"));

	read_game_info(gtp.attr("game_info"));
}


void GameData::read_game_cars(pybind11::list game_cars, int num_cars)
{

	if ((int)cars.size() != num_cars)
	{
		cars.resize(num_cars);

		// assigning references

		if (index >= 0 && index < num_cars)
			my_car = &cars[index];

		opponents.clear();
		teammates.clear();

		for (int i = 0; i < num_cars; i++)
		{
			if (i != index)
			{
				pybind11::object game_car = game_cars[i];
				int game_car_team = game_car.attr("team").cast<int>();

				std::vector<Car*>* team_list = (game_car_team != team) ? &opponents : &teammates;
				team_list->push_back(&cars[i]);
			}
		}
	}

	for (int i = 0; i < (int)cars.size(); i++)
		cars[i].read_game_car(game_cars[i]);

}


void GameData::read_game_boosts(pybind11::list game_boosts)
{
	for (int i = 0; i < 2; i++)
	{
		std::vector<Pad*>* pad_type = (i == 0) ? &large_pads : &small_pads;

		for (int j = 0; j < pad_type->size(); j++)
		{
			Pad* pad = pad_type->at(j);
			/*pad->is_active = game_boosts[pad->index].attr("is_active").cast<bool>();
			pad->timer = game_boosts[pad->index].attr("timer").cast<float>();*/
		}
	}
}


void GameData::read_game_info(pybind11::object game_info)
{
	time = game_info.attr("seconds_elapsed").cast<float>();
	time_remaining = game_info.attr("game_time_remaining").cast<float>();
	overtime = game_info.attr("is_overtime").cast<bool>();
	round_active = game_info.attr("is_round_active").cast<bool>();
	kickoff_pause = game_info.attr("is_kickoff_pause").cast<bool>();
	match_ended = game_info.attr("is_match_ended").cast<bool>();
	world_gravity_z = game_info.attr("world_gravity_z").cast<float>();
	game_speed = game_info.attr("game_speed").cast<float>();

}


void GameData::read_field_info(pybind11::object field_info)
{
	/* Reads an instance of FieldInfoPacket provided by the rlbot framework,
	and converts it's contents into our internal structure.*/

	int num_goals = field_info.attr("num_goals").cast<int>();
	if (num_goals != 0)
		read_goals(field_info.attr("goals"), num_goals);

	int num_boosts = field_info.attr("num_boosts").cast<int>();
	if (num_boosts != 0)
		read_boost_pads(field_info.attr("boost_pads"), num_boosts);

}


void GameData::read_boost_pads(pybind11::list boost_pads, int num_boosts)
{
	if ((int)large_pads.size() + (int)small_pads.size() != num_boosts)
	{
		large_pads.clear();
		small_pads.clear();

		for (int i = 0; i < num_boosts; i++)
		{
			pybind11::object boost_pad = boost_pads[i];
			bool is_full_boost = boost_pad.attr("is_full_boost").cast<bool>();

			Pad* pad = &Pad();
			pad->index = i;
			pad->location = convert::vector3_to_vec3(boost_pad.attr("location"));
			pad->is_full_boost = is_full_boost;

			std::vector<Pad*>* pad_type = (is_full_boost) ? &large_pads : &small_pads;
			pad_type->push_back(pad);
		}
	}
}


void GameData::read_goals(pybind11::list goals, int num_goals)
{
	if ((int)opp_goals.size() + (int)own_goals.size() != num_goals)
	{
		opp_goals.clear();
		own_goals.clear();

		for (int i = 0; i < num_goals; i++)
		{
			pybind11::object goal_info = goals[i];
			int goal_team = goal_info.attr("team_num").cast<bool>();

			Goal* goal = &Goal();
			goal->team = goal_team;
			goal->location = convert::vector3_to_vec3(goal_info.attr("location"));
			goal->direction = convert::vector3_to_vec3(goal_info.attr("direction"));

			std::vector<Goal*>* goal_type = (goal_team != team) ? &opp_goals : &own_goals;
			goal_type->push_back(goal);
		}
	}
}


// TODO: add read_rigid_body_tick


#ifdef GENERATE_PYTHON_BINDINGS

#include <pybind11/stl.h>

void init_game_data(pybind11::module & m) {

	pybind11::class_<GameData>(m, "GameData")

		.def(pybind11::init<>())
		.def(pybind11::init<std::string, int, int>())

		.def_readwrite("name", &GameData::name)
		.def_readwrite("team", &GameData::team)
		.def_readwrite("index", &GameData::index)

		.def_property_readonly("cars", &GameData::GetCars)
		.def_property_readonly("my_car", &GameData::GetMyCar)
		.def_property_readonly("opponents", &GameData::GetOpponents)
		.def_property_readonly("teammates", &GameData::GetTeammates)

		.def_property_readonly("ball", &GameData::GetBall)

		.def_property_readonly("large_pads", &GameData::GetLargePads)
		.def_property_readonly("small_pads", &GameData::GetSmallPads)

		.def_property_readonly("opp_goals", &GameData::GetOppGoals)
		.def_property_readonly("own_goals", &GameData::GetOwnGoals)

		.def_readwrite("time", &GameData::time)
		.def_readwrite("time_remaining", &GameData::time_remaining)
		.def_readwrite("overtime", &GameData::overtime)
		.def_readwrite("round_active", &GameData::round_active)
		.def_readwrite("kickoff_pause", &GameData::kickoff_pause)
		.def_readwrite("match_ended", &GameData::match_ended)
		.def_readwrite("world_gravity_z", &GameData::world_gravity_z)
		.def_readwrite("game_speed", &GameData::game_speed)

		.def("read_game_tick_packet", &GameData::read_game_tick_packet)
		.def("read_field_info", &GameData::read_field_info);

}
#endif