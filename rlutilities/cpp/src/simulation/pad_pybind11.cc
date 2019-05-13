#include "simulation/pad.h"

#include <pybind11/pybind11.h>

void init_pad(pybind11::module & m) {

	pybind11::class_<Pad>(m, "Pad")
		.def(pybind11::init<>())
		.def_readwrite("location", &Pad::location)
		.def_readwrite("is_active", &Pad::is_active)
		.def_readwrite("is_full_boost", &Pad::is_active)
		.def_readwrite("timer", &Pad::timer);

}
