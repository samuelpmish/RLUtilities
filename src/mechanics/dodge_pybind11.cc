#include "mechanics/dodge.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_dodge(pybind11::module & m) {
	pybind11::class_<Dodge>(m, "Dodge")
		.def(pybind11::init<Car &>())
		.def_readwrite("direction", &Dodge::direction)
		.def_readwrite("preorientation", &Dodge::preorientation)
		.def_readwrite("jump_duration", &Dodge::jump_duration)
		.def_readwrite("delay", &Dodge::delay)
		.def_readonly("timer", &Dodge::timer)
		.def_readonly("finished", &Dodge::finished)
		.def_readonly("controls", &Dodge::controls)
		.def_readonly_static("timeout", &Dodge::timeout)
		.def_readonly_static("input_threshold", &Dodge::input_threshold)
		.def_readonly_static("z_damping", &Dodge::z_damping)
		.def_readonly_static("z_damping_start", &Dodge::z_damping_start)
		.def_readonly_static("z_damping_end", &Dodge::z_damping_end)
		.def_readonly_static("torque_time", &Dodge::torque_time)
		.def_readonly_static("side_torque", &Dodge::side_torque)
		.def_readonly_static("forward_torque", &Dodge::forward_torque)
		.def("step", &Dodge::step)
		.def("simulate", &Dodge::simulate);
}
