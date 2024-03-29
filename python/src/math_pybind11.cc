#include <string>
#include <utility>
#include <sstream>

#include "linear_algebra/math.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
void init_linalg(pybind11::module & m) {

	using namespace pybind11::literals;

	pybind11::class_<vec2>(m, "vec2")
		.def(pybind11::init< float, float >(), "x"_a = 0.0f, "y"_a = 0.0f)
		.def(pybind11::init< const vec2 & >())
		.def(pybind11::init([](const vec3 & v) {
			return vec2{ v[0], v[1] };
		}))
		.def("__getitem__", [](const vec2 & v, size_t i) { return v[i]; })
		.def("__setitem__", [](vec2 & v, size_t i, float f) { v[i] = f; })
		.def_property("x", [](const vec2 & v) {return v[0];}, [](vec2 & v, float f) {v[0] = f;})
		.def_property("y", [](const vec2 & v) {return v[1];}, [](vec2 & v, float f) {v[1] = f;})
		.def("__str__", [](const vec2 & v) {
			return std::to_string(v[0]) + " " + std::to_string(v[1]);
		})
		.def("__repr__", [](const vec2 & v) {
			std::stringstream ss;
			ss << "vec2(" << v[0] << ", " << v[1] << ")";
			return ss.str();
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
		.def(pybind11::init< float, float, float >(), "x"_a = 0.0f, "y"_a = 0.0f, "z"_a = 0.0f)
		.def(pybind11::init< const vec3 & >())
		.def(pybind11::init([](const vec2 & v) {
			return vec3{ v[0], v[1], 0.0f };
		}))
		.def("__getitem__", [](const vec3 & v, size_t i) { return v[i]; })
		.def("__setitem__", [](vec3 & v, size_t i, float f) { v[i] = f; })
		.def_property("x", [](const vec3 & v) {return v[0];}, [](vec3 & v, float f) {v[0] = f;})
		.def_property("y", [](const vec3 & v) {return v[1];}, [](vec3 & v, float f) {v[1] = f;})
		.def_property("z", [](const vec3 & v) {return v[2];}, [](vec3 & v, float f) {v[2] = f;})
		.def("__str__", [](const vec3 & v) {
			return std::to_string(v[0]) + " " + std::to_string(v[1]) + " " + std::to_string(v[2]);
		})
		.def("__repr__", [](const vec3 & v) {
			std::stringstream ss;
			ss << "vec3(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
			return ss.str();
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
		.def(pybind11::init([](float a11, float a12, float a21, float a22) {
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
		})
		.def("__repr__", [](const mat2 & A) {
			std::stringstream ss;
			ss << "mat2(" << A(0, 0) << ", " << A(0, 1) << ", " << A(1, 0) << ", " << A(1, 1) << ")";
			return ss.str();
		})
		.def(pybind11::self - pybind11::self)
		.def(pybind11::self + pybind11::self)
		.def(float() * pybind11::self)
		.def(pybind11::self * float())
		.def(pybind11::self / float());

	pybind11::class_<vec4>(m, "vec4")
		.def(pybind11::init< float, float, float, float >())
		.def("__getitem__", [](const vec4 & v, size_t i) { return v[i]; })
		.def("__setitem__", [](vec4 & v, size_t i, float f) { v[i] = f; })
		.def("__str__", [](const vec4 & v) {
			return std::to_string(v[0]) + " " + std::to_string(v[1]) +
			 " " + std::to_string(v[2]) + " " + std::to_string(v[3]);
		})
		.def("__repr__", [](const vec4 & v) {
			std::stringstream ss;
			ss << "vec4(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
			return ss.str();
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
		})
		.def("__repr__", [](const mat3 & A) {
			std::stringstream ss;
			ss << "mat3(" << A(0, 0) << ", " << A(0, 1) << ", " << A(0, 2) << ", " <<
			                 A(1, 0) << ", " << A(1, 1) << ", " << A(1, 2) << ", " <<
			                 A(2, 0) << ", " << A(2, 1) << ", " << A(2, 2) << ")";
			return ss.str();
		})
		.def(pybind11::self - pybind11::self)
		.def(pybind11::self + pybind11::self)
		.def(float() * pybind11::self)
		.def(pybind11::self * float())
		.def(pybind11::self / float());

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

	m.def("eye2", (mat2(*)()) &eye);
	m.def("eye3", (mat3(*)()) &eye);

	m.def("det", (float(*)(const mat2 &)) &det);
	m.def("det", (float(*)(const mat3 &)) &det);

	m.def("inv", (mat2(*)(const mat2 &)) &inv);
	m.def("inv", (mat3(*)(const mat3 &)) &inv);
	m.def("look_at", (mat2(*)(const vec2 &)) &look_at, "forward"_a);
	m.def("look_at", (mat3(*)(const vec3 &, const vec3 &)) &look_at, "forward"_a, "up"_a = vec3{0, 0, 1});
	m.def("rotation", &rotation, "angle"_a);
	m.def("euler_to_rotation", &euler_to_rotation, "euler_angles"_a);
	m.def("rotation_to_euler", &rotation_to_euler, "rotation_matrix"_a);
	m.def("axis_to_rotation", &axis_to_rotation, "axis"_a);
	m.def("rotation_to_axis", &rotation_to_axis, "rotation_matrix"_a);
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

	m.def("lerp", (vec2(*)(const vec2 &, const vec2 &, const float)) &lerp);
	m.def("lerp", (vec3(*)(const vec3 &, const vec3 &, const float)) &lerp);
}
