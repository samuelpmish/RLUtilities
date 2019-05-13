#include "simulation/field.h"

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
void init_field(pybind11::module & m) {
  pybind11::class_<Field>(m, "Field")
    .def_readonly_static("mode", &Field::mode)
    .def_readonly_static("walls", &Field::walls)
    .def_readonly_static("triangles", &Field::triangles)
    .def_static("snap", &Field::snap)
    .def_static("collide", static_cast<ray(*)(const obb &)>(&Field::collide))
    .def_static("collide", static_cast<ray(*)(const sphere &)>(&Field::collide))
    .def_static("raycast_any", &Field::raycast_any);
}
