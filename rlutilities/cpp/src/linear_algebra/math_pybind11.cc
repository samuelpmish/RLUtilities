#include <string>
#include <utility>

#include "linear_algebra/math.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
void init_linalg(pybind11::module & m) {

	pybind11::class_<vec2>(m, "vec2")
		.def(pybind11::init< float, float >())
		.def(pybind11::init< const vec2 & >())
		.def(pybind11::init([](const vec3 & v) {
		return vec2{ v[0], v[1] };
	}))
		.def("__getitem__", [](const vec2 & v, size_t i) { return v[i]; })
		.def("__setitem__", [](vec2 & v, size_t i, float f) { v[i] = f; })
		.def("__str__", [](const vec2 & v) {
		return std::to_string(v[0]) + std::string(" ") +
			std::to_string(v[1]);
	})
		.def(pybind11::self - pybind11::self)
		.def(pybind11::self + pybind11::self)
		.def(pybind11::self -= pybind11::self)
		.def(pybind11::self += pybind11::self)
		.def(pybind11::self *= float())
		.def(pybind11::self /= float())
		.def(float() * pybind11::self)
		.def(pybind11::self * float())
		.def(pybind11::self / float());

	pybind11::class_<vec3>(m, "vec3")
		.def(pybind11::init< float, float, float >())
		.def(pybind11::init< const vec3 & >())
		.def(pybind11::init([](const vec2 & v) {
		return vec3{ v[0], v[1], 0.0f };
	}))
		.def("__getitem__", [](const vec3 & v, size_t i) { return v[i]; })
		.def("__setitem__", [](vec3 & v, size_t i, float f) { v[i] = f; })
		.def("__str__", [](const vec3 & v) {
		return std::to_string(v[0]) + std::string(" ") +
			std::to_string(v[1]) + std::string(" ") +
			std::to_string(v[2]);
	})
		.def(pybind11::self - pybind11::self)
		.def(pybind11::self + pybind11::self)
		.def(pybind11::self -= pybind11::self)
		.def(pybind11::self += pybind11::self)
		.def(pybind11::self *= float())
		.def(pybind11::self /= float())
		.def(float() * pybind11::self)
		.def(pybind11::self * float())
		.def(pybind11::self / float());

	pybind11::class_<mat2>(m, "mat2")
		.def(pybind11::init([](float a11, float a12,
			float a21, float a22) {
		return mat2{
		  {a11, a12},
		  {a21, a22}
		};
	}))
		.def("__getitem__", [](const mat2 & A, std::pair < size_t, size_t > i) {
		return A(i.first, i.second);
	})
		.def("__setitem__", [](mat2 & A, std::pair < size_t, size_t > i, float v) {
		A(i.first, i.second) = v;
	})
		.def("__str__", [](const mat2 & A) {
		std::string s;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				s += std::to_string(A(i, j));
				if (j == 1) s += "\n";
				else        s += " ";
			}
		}
		return s;
	});

	pybind11::class_<vec4>(m, "vec4")
		.def(pybind11::init< float, float, float, float >())
		.def("__getitem__", [](const vec4 & v, size_t i) { return v[i]; })
		.def("__setitem__", [](vec4 & v, size_t i, float f) { v[i] = f; })
		.def("__str__", [](const vec4 & v) {
		return std::to_string(v[0]) + std::string(" ") +
           std::to_string(v[1]) + std::string(" ") +
           std::to_string(v[2]) + std::string(" ") +
			     std::to_string(v[3]);
	  })
		.def(pybind11::self - pybind11::self)
		.def(pybind11::self + pybind11::self)
		.def(pybind11::self -= pybind11::self)
		.def(pybind11::self += pybind11::self)
		.def(pybind11::self *= float())
		.def(pybind11::self /= float())
		.def(float() * pybind11::self)
		.def(pybind11::self * float())
		.def(pybind11::self / float());

	pybind11::class_<mat3>(m, "mat3")
		.def(pybind11::init([](float a11, float a12, float a13,
			float a21, float a22, float a23,
			float a31, float a32, float a33) {
		return mat3{
		  {a11, a12, a13},
		  {a21, a22, a23},
		  {a31, a32, a33}
		};
	}))
		.def("__getitem__", [](const mat3 & A, std::pair < size_t, size_t > i) {
		return A(i.first, i.second);
	})
		.def("__setitem__", [](mat3 & A, std::pair < size_t, size_t > i, float v) {
		A(i.first, i.second) = v;
	})
		.def("__str__", [](const mat3 & A) {
		std::string s;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				s += std::to_string(A(i, j));
				if (j == 2) s += "\n";
				else        s += " ";
			}
		}
		return s;
	});

	// free functions
	m.def("dot", (float(*)(const vec2 &, const vec2 &)) &dot);
	m.def("dot", (vec2(*)(const vec2 &, const mat2 &)) &dot);
	m.def("dot", (vec2(*)(const mat2 &, const vec2 &)) &dot);
	m.def("dot", (mat2(*)(const mat2 &, const mat2 &)) &dot);

	m.def("dot", (float(*)(const vec3 &, const vec3 &)) &dot);
	m.def("dot", (vec3(*)(const vec3 &, const mat3 &)) &dot);
	m.def("dot", (vec3(*)(const mat3 &, const vec3 &)) &dot);
	m.def("dot", (mat3(*)(const mat3 &, const mat3 &)) &dot);

	m.def("dot", (float(*)(const vec4 &, const vec4 &)) &dot);

	m.def("cross", (vec3(*)(const vec3 &, const vec3 &)) &cross);
	m.def("cross", (vec3(*)(const vec3 &)) &cross);
	m.def("cross", (vec2(*)(const vec2 &)) &cross);

	m.def("inv", (mat2(*)(const mat2 &)) &inv);
	m.def("inv", (mat3(*)(const mat3 &)) &inv);
	m.def("look_at", &look_at);
	m.def("rotation", &rotation);
	m.def("euler_to_rotation", &euler_to_rotation);
	m.def("axis_to_rotation", &axis_to_rotation);
	m.def("rotation_to_axis", &rotation_to_axis);
	m.def("transpose", (mat2(*)(const mat2 &)) &transpose);
	m.def("transpose", (mat3(*)(const mat3 &)) &transpose);
	m.def("angle_between", (float(*)(const vec2 &, const vec2 &)) &angle_between);
	m.def("angle_between", (float(*)(const vec3 &, const vec3 &)) &angle_between);
	m.def("angle_between", (float(*)(const mat3 &, const mat3 &)) &angle_between);

	m.def("sgn", &sgn);
	m.def("clip", (float(*)(float, float, float)) &clip);
	m.def("clip", (int(*)(const int, const int, const int)) &clip);

	m.def("xy", &xy);
	m.def("norm", (float(*)(const vec3 &)) &norm);
	m.def("norm", (float(*)(const vec2 &)) &norm);
	m.def("normalize", (vec3(*)(const vec3 &)) &normalize);
	m.def("normalize", (vec2(*)(const vec2 &)) &normalize);

}
