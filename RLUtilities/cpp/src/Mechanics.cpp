#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <utility>

#include "AerialTurn.h"

PYBIND11_MODULE(Mechanics, m) {
  pybind11::class_<AerialTurn>(m, "AerialTurn")
    .def(pybind11::init<Car &, const mat3 &>())
    .def_readwrite("target", &AerialTurn::target)
    .def_readwrite("finished", &AerialTurn::finished)
    .def_readwrite("controls", &AerialTurn::controls)
    .def("step", &AerialTurn::step)
    .def("time_to_complete", &AerialTurn::time_to_complete);
}
