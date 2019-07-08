#include "mechanics/aerial_turn.h"
#include <pybind11/pybind11.h>
void init_aerialturn(pybind11::module & m) {
  pybind11::class_<AerialTurn>(m, "AerialTurn")
    .def(pybind11::init<Car &>())
    .def_readwrite("target", &AerialTurn::target)
    .def_readwrite("eps_phi", &AerialTurn::eps_phi)
    .def_readwrite("eps_omega", &AerialTurn::eps_omega)
    .def_readwrite("horizon_time", &AerialTurn::horizon_time)
    .def_readwrite("finished", &AerialTurn::finished)
    .def_readwrite("controls", &AerialTurn::controls)
    .def_readonly("alpha", &AerialTurn::alpha)
    .def("step", &AerialTurn::step)
    .def("simulate", &AerialTurn::simulate);
}
