#include "mechanics/boostdash.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_boostdash(pybind11::module & m) {
	pybind11::class_<Boostdash>(m, "Boostdash")
		.def(pybind11::init<Car &>())
		.def_readonly("finished", &Boostdash::finished)
		.def_readonly("controls", &Boostdash::controls)
		.def("step", &Boostdash::step);
	//.def("simulate", &Boostdash::simulate);
}
