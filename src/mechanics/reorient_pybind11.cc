#include "mechanics/reorient.h"
#include <pybind11/pybind11.h>
void init_reorient(pybind11::module & m) {
  pybind11::class_<Reorient>(m, "Reorient")
    .def(pybind11::init<Car &>())
    .def_readwrite("target_orientation", &Reorient::target_orientation)
    .def_readwrite("eps_phi", &Reorient::eps_phi)
    .def_readwrite("eps_omega", &Reorient::eps_omega)
    .def_readwrite("horizon_time", &Reorient::horizon_time)
    .def_readwrite("finished", &Reorient::finished)
    .def_readwrite("controls", &Reorient::controls)
    .def_readonly("alpha", &Reorient::alpha)
    .def("step", &Reorient::step)
    .def("simulate", &Reorient::simulate);
}
