#include "simulation/geometry.h"

#include <pybind11/pybind11.h>
void init_geometry(pybind11::module & m) {
	pybind11::class_<ray>(m, "ray")
		.def(pybind11::init())
		.def(pybind11::init< vec<3>, vec<3> >())
		.def_readwrite("start", &ray::start)
		.def_readwrite("direction", &ray::direction);

	pybind11::class_<obb>(m, "obb")
		.def(pybind11::init())
		.def_readwrite("center", &obb::center)
		.def_readwrite("half_width", &obb::half_width)
		.def_readwrite("orientation", &obb::orientation);

	pybind11::class_<sphere>(m, "sphere")
		.def(pybind11::init())
		.def(pybind11::init< vec<3>, float >())
		.def_readwrite("center", &sphere::center)
		.def_readwrite("radius", &sphere::radius);

	pybind11::class_<tri>(m, "tri")
		.def(pybind11::init())
		.def("__getitem__", [](const tri & t, size_t i) { return t.p[i]; })
		.def("__setitem__", [](tri & t, size_t i, vec3 v) { t.p[i] = v; });

	m.def("intersect", (bool(*)(const obb &, const sphere &)) &intersect);
	m.def("intersect", (bool(*)(const sphere &, const obb &)) &intersect);
}
