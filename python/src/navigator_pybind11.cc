#include "experimental/navigator.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_navigator(pybind11::module & m) {
  pybind11::class_<Navigator>(m, "Navigator")
    .def(pybind11::init<Car &>())
    .def_readonly_static("nodes", &Navigator::navigation_nodes)
    .def("path_to", &Navigator::path_to, pybind11::arg("destination"), pybind11::arg("tangent"), pybind11::arg("offset"))
    .def("analyze_surroundings", &Navigator::analyze_surroundings, pybind11::arg("time_budget"));
}
