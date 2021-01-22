#include "simulation/goal.h"

#include <pybind11/pybind11.h>

void init_goal(pybind11::module & m) {
	pybind11::class_<Goal>(m, "Goal")
		.def(pybind11::init<>())
		.def_readwrite("team", &Goal::team)
		.def_readwrite("position", &Goal::position)
		.def_readwrite("direction", &Goal::direction)
		.def_readwrite("width", &Goal::width)
		.def_readwrite("height", &Goal::height);
}
