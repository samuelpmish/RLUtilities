#include "experimental/follow_path.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_followpath(pybind11::module & m) {
  pybind11::class_<FollowPath>(m, "FollowPath")
    .def(pybind11::init<Car &>())
    .def_readwrite("arrival_time", &FollowPath::arrival_time)
    .def_readwrite("arrival_speed", &FollowPath::arrival_speed)
    .def_readwrite("path", &FollowPath::path)
    .def_readonly("finished", &FollowPath::finished)
    .def_readonly("controls", &FollowPath::controls)
    .def("step", &FollowPath::step)
    .def("calculate_plan", &FollowPath::calculate_plan, pybind11::arg("path"), pybind11::arg("arrival_time"), pybind11::arg("arrival_speed"));
}
