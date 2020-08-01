#include "mechanics/wavedash.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_wavedash(pybind11::module & m) {
	pybind11::class_<Wavedash>(m, "Wavedash")
		.def(pybind11::init<Car &>())
		.def_readwrite("direction", &Wavedash::direction)
		.def_readonly("finished", &Wavedash::finished)
		.def_readonly("controls", &Wavedash::controls)
		.def("step", &Wavedash::step);
	//.def("simulate", &Wavedash::simulate);
}
