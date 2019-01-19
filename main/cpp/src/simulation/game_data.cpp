#include "simulation/game_data.h"


#ifdef GENERATE_PYTHON_BINDINGS

#include <pybind11/pybind11.h>

void init_game_data(pybind11::module & m) {

	pybind11::class_<GameData>(m, "GameData")
		.def(pybind11::init<>())
		.def_readwrite("my_car", &GameData::my_car)
		.def_readwrite("opponents", &GameData::opponents)
		.def_readwrite("teammates", &GameData::teammates)
		.def_readwrite("ball", &GameData::ball)
		.def_readwrite("large_pads", &GameData::large_pads)
		.def_readwrite("small_pads", &GameData::small_pads)
		.def_readwrite("opp_goals", &GameData::opp_goals)
		.def_readwrite("own_goals", &GameData::own_goals);

}
#endif