#include "mechanics/reorient_ML.h"
#include <pybind11/pybind11.h>
void init_reorient_ML(pybind11::module & m) {
  pybind11::class_<ReorientML>(m, "ReorientML")
    .def(pybind11::init<Car &>())
    .def_readwrite("target_orientation", &ReorientML::target_orientation)
    .def_readwrite("eps_phi", &ReorientML::eps_phi)
    .def_readwrite("finished", &ReorientML::finished)
    .def_readwrite("controls", &ReorientML::controls)
    .def("step", &ReorientML::step)
    .def("simulate", &ReorientML::simulate);
}
