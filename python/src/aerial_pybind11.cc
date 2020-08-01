#include "mechanics/aerial.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_aerial(pybind11::module & m) {
  pybind11::class_<Aerial>(m, "Aerial")
    .def(pybind11::init<Car &>())
    .def_readwrite("arrival_time", &Aerial::arrival_time)
    .def_readwrite("target_position", &Aerial::target_position)
    .def_readwrite("target_orientation", &Aerial::target_orientation)
    .def_readwrite("up", &Aerial::up)
    .def_readwrite("angle_threshold", &Aerial::angle_threshold)
    .def_readwrite("reorient_distance", &Aerial::reorient_distance)
    .def_readwrite("throttle_distance", &Aerial::throttle_distance)
    .def_readonly("finished", &Aerial::finished)
    .def_readonly("controls", &Aerial::controls)
    .def_readonly_static("max_speed", &Aerial::max_speed)
    .def_readonly_static("boost_accel", &Aerial::boost_accel)
    .def_readonly_static("throttle_accel", &Aerial::throttle_accel)
    .def_readonly_static("boost_per_second", &Aerial::boost_per_second)
    .def("step", &Aerial::step)
    .def("is_viable", &Aerial::is_viable)
    .def("simulate", &Aerial::simulate);
}
