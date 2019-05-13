#include "mechanics/jump.h"
#include <pybind11/pybind11.h>
void init_jump(pybind11::module & m) {
	pybind11::class_<Jump>(m, "Jump")
		.def(pybind11::init<Car &>())
		.def_readwrite("duration", &Jump::duration)
		.def_readwrite("finished", &Jump::finished)
		.def_readwrite("controls", &Jump::controls)
		.def_readonly_static("speed", &Jump::speed)
		.def_readonly_static("acceleration", &Jump::acceleration)
		.def_readonly_static("min_duration", &Jump::min_duration)
		.def_readonly_static("max_duration", &Jump::max_duration)
		.def("step", &Jump::step)
		.def("simulate", &Jump::simulate);
}
