#include "simulation/curve.h"
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
void init_curve(pybind11::module & m) {
	pybind11::class_<ControlPoint>(m, "ControlPoint")
		.def(pybind11::init())
		.def(pybind11::init< vec<3>, vec<3>, vec<3> >())
		.def_readwrite("p", &ControlPoint::p)
		.def_readwrite("t", &ControlPoint::t)
		.def_readwrite("n", &ControlPoint::n);

  pybind11::class_<Curve>(m, "Curve")
    .def(pybind11::init< std::vector < vec<3> > >())
    .def(pybind11::init< std::vector < ControlPoint > >())
    .def("point_at", &Curve::point_at)
    .def("tangent_at", &Curve::tangent_at)
    .def("curvature_at", &Curve::curvature_at)
    .def("max_speed_at", &Curve::max_speed_at)
    .def("find_nearest", &Curve::find_nearest)
    .def("pop_front", &Curve::pop_front)
    .def("calculate_distances", &Curve::calculate_distances)
    .def("calculate_tangents", &Curve::calculate_tangents)
    .def("calculate_max_speeds", &Curve::calculate_max_speeds)
    .def("write_to_file", &Curve::write_to_file)
    .def_readwrite("points", &Curve::points)
    .def_readonly("length", &Curve::length);
}
