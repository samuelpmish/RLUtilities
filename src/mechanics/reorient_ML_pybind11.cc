#include "mechanics/orientation.h"
#include <pybind11/pybind11.h>
void init_orientation_controller(pybind11::module & m) {
  pybind11::class_<OrientationController>(m, "OrientationController")
    .def(pybind11::init<Car &>())
    .def_readwrite("target", &OrientationController::target)
    .def_readwrite("eps_phi", &OrientationController::eps_phi)
    .def_readwrite("finished", &OrientationController::finished)
    .def_readwrite("controls", &OrientationController::controls)
    .def("step", &OrientationController::step)
    .def("simulate", &OrientationController::simulate);
}
