#include <pybind11/pybind11.h>

#include "Car.h"

PYBIND11_MODULE(Simulation, m) {

  m.def("max_speed", &max_speed);
  m.def("max_curvature", &max_curvature);

}
