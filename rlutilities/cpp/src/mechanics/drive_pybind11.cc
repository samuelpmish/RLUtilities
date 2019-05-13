#include "mechanics/drive.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_drive(pybind11::module & m) {
  pybind11::class_<Drive>(m, "Drive")
    .def(pybind11::init<Car &>())
    .def_readwrite("target", &Drive::target)
    .def_readwrite("speed", &Drive::speed)
    .def_readwrite("reaction_time", &Drive::reaction_time)
    .def_readwrite("finished", &Drive::finished)
    .def_readwrite("controls", &Drive::controls)
    .def_readonly_static("max_speed", &Drive::max_speed)
    .def_readonly_static("max_throttle_speed", &Drive::max_throttle_speed)
    .def_readonly_static("boost_accel", &Drive::boost_accel)
    .def_readonly_static("brake_accel", &Drive::brake_accel)
    .def_readonly_static("coasting_accel", &Drive::coasting_accel)
    .def_static("max_turning_speed", &Drive::max_turning_speed)
    .def_static("max_turning_curvature", &Drive::max_turning_curvature)
    .def_static("throttle_accel", &Drive::throttle_accel)
    .def("step", &Drive::step);
}
