#include "simulation/boost_pad.h"

#include <pybind11/pybind11.h>

void init_pad(pybind11::module & m) {
	pybind11::class_<BoostPad>(m, "BoostPad")
		.def(pybind11::init<>())
		.def_readwrite("position", &BoostPad::position)
		.def_readwrite("state", &BoostPad::state)
		.def_readwrite("type", &BoostPad::type);
}
