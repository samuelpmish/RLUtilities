#pragma once

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <utility>

#include "Ball.h"
#include "Car.h"
#include "linalg.h"
#include "Bezier.h"
#include "Curve.h"
#include "ArcLineArc.h"

PYBIND11_MODULE(Lobot, m) {

	pybind11::class_<Bot>(m, "Bot")
		.def(pybind11::init<int>())
		.def("get_response", &Bot::get_response);

	pybind11::class_<Ball>(m, "Ball")
		.def(pybind11::init<>())
		.def(pybind11::init<const Ball &>())
		.def_readwrite("pos", &Ball::x)
		.def_readwrite("vel", &Ball::v)
		.def_readwrite("omega", &Ball::w)
		.def_readwrite("t", &Ball::t)
		.def_readwrite("last_bounce", &Ball::last_bounce)
		.def("step", &Ball::step)
		.def("wall_nearby", &Ball::wall_nearby);

	pybind11::class_<Pitch>(m, "Pitch")
		.def(pybind11::init<>())
		.def("in_contact_with", (bool (Pitch::*)(const obb &)) &Pitch::in_contact_with)
		.def("in_contact_with", (bool (Pitch::*)(const sphere &)) &Pitch::in_contact_with)
		.def("raycast_any", &Pitch::raycast_any);

	pybind11::class_<vec2>(m, "vec2")
		.def(pybind11::init< float, float >())
		.def(pybind11::init< const vec2 & >())
		.def(pybind11::init([](const vec3 & v) {
		return vec2{ v[0], v[1] };
	}))
		.def("__getitem__", [](const vec2 & v, size_t i) { return v[i]; })
		.def("__setitem__", [](vec2 & v, size_t i, float f) { v[i] = f; })
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
	});

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
	});

	pybind11::class_<Bezier>(m, "Bezier")
		.def(pybind11::init< std::vector < vec<3> > >())
		.def("__call__", &Bezier::operator())
		.def("tangent", &Bezier::tangent)
		.def("length", &Bezier::length)
		.def("t_nearest", &Bezier::t_nearest)
		.def_readwrite("points", &Bezier::points);

	pybind11::class_<ArcLineArc>(m, "ArcLineArc")
		.def(pybind11::init< vec2, vec2, float, vec2, vec2, float >())
		.def_readwrite("length", &ArcLineArc::length)
		.def_readwrite("L1", &ArcLineArc::L1)
		.def_readwrite("L2", &ArcLineArc::L2)
		.def_readwrite("L3", &ArcLineArc::L3)
		.def_readwrite("p1", &ArcLineArc::p1)
		.def_readwrite("p2", &ArcLineArc::p2)
		.def_readwrite("q1", &ArcLineArc::q1)
		.def_readwrite("q2", &ArcLineArc::q2)
		.def_readwrite("t1", &ArcLineArc::t1)
		.def_readwrite("t2", &ArcLineArc::t2)
		.def_readwrite("n1", &ArcLineArc::n1)
		.def_readwrite("n2", &ArcLineArc::n2)
		.def_readwrite("o1", &ArcLineArc::o1)
		.def_readwrite("o2", &ArcLineArc::o2)
		.def_readwrite("r1", &ArcLineArc::r1)
		.def_readwrite("r2", &ArcLineArc::r2)
		.def_readwrite("phi1", &ArcLineArc::phi1)
		.def_readwrite("phi2", &ArcLineArc::phi2)
		.def_readwrite("is_valid", &ArcLineArc::is_valid);

	pybind11::class_<Curve>(m, "Curve")
		.def(pybind11::init< std::vector < vec<3> > >())
		.def(pybind11::init< const ArcLineArc &, float >())
		.def("point_at", &Curve::point_at)
		.def("tangent_at", &Curve::tangent_at)
		.def("find_nearest", &Curve::find_nearest)
		.def("pop_front", &Curve::pop_front)
		.def_readwrite("length", &Curve::length);

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

	pybind11::class_<Input>(m, "Input")
		.def(pybind11::init<>())
		.def_readwrite("steer", &Input::steer)
		.def_readwrite("roll", &Input::roll)
		.def_readwrite("pitch", &Input::pitch)
		.def_readwrite("yaw", &Input::yaw)
		.def_readwrite("throttle", &Input::throttle)
		.def_readwrite("jump", &Input::jump)
		.def_readwrite("boost", &Input::boost)
		.def_readwrite("slide", &Input::slide);

	pybind11::class_<Car>(m, "Car")
		.def(pybind11::init<>())
		.def(pybind11::init<const Car &>())
		.def("step", &Car::step)
		.def("pitch_surface_normal", &Car::pitch_surface_normal)
		.def("bounding_box", &Car::bounding_box)
		.def("extrapolate", &Car::extrapolate)
		.def("forward", &Car::forward)
		.def("left", &Car::left)
		.def("up", &Car::up)
		.def_readwrite("id", &Car::id)
		.def_readwrite("team", &Car::team)
		.def_readwrite("target", &Car::target)
		.def_readwrite("time", &Car::time)
		.def_readwrite("ETA", &Car::ETA)
		.def_readwrite("pos", &Car::x)
		.def_readwrite("vel", &Car::v)
		.def_readwrite("omega", &Car::w)
		.def_readwrite("theta", &Car::o)
		.def_readwrite("supersonic", &Car::supersonic)
		.def_readwrite("jumped", &Car::jumped)
		.def_readwrite("double_jumped", &Car::double_jumped)
		.def_readwrite("on_ground", &Car::on_ground)
		.def_readwrite("boost", &Car::boost)
		.def_readwrite("last_input", &Car::last);

	// free functions
	m.def("dot", (float(*)(const vec2 &, const vec2 &)) &dot);
	m.def("dot", (vec2(*)(const vec2 &, const mat2 &)) &dot);
	m.def("dot", (vec2(*)(const mat2 &, const vec2 &)) &dot);
	m.def("dot", (mat2(*)(const mat2 &, const mat2 &)) &dot);

	m.def("dot", (float(*)(const vec3 &, const vec3 &)) &dot);
	m.def("dot", (vec3(*)(const vec3 &, const mat3 &)) &dot);
	m.def("dot", (vec3(*)(const mat3 &, const vec3 &)) &dot);
	m.def("dot", (mat3(*)(const mat3 &, const mat3 &)) &dot);

	m.def("cross", (vec3(*)(const vec3 &, const vec3 &)) &cross);
	m.def("cross", (vec3(*)(const vec3 &)) &cross);
	m.def("cross", (vec2(*)(const vec2 &)) &cross);

	m.def("inv", (mat2(*)(const mat2 &)) &inv);
	m.def("inv", (mat3(*)(const mat3 &)) &inv);
	m.def("rotation", &rotation);
	m.def("euler_rotation", &euler_rotation);
	m.def("axis_rotation", &axis_rotation);
	m.def("rotation_to_axis", &rotation_to_axis);
	m.def("transpose", (mat2(*)(const mat2 &)) &transpose);
	m.def("transpose", (mat3(*)(const mat3 &)) &transpose);
	m.def("angle_between", (float(*)(const vec3 &, const vec3 &)) &angle_between);
	m.def("angle_between", (float(*)(const mat3 &, const mat3 &)) &angle_between);

	m.def("sgn", &sgn);
	m.def("clip", &clip);

	m.def("xy", &xy);
	m.def("norm", (float(*)(const vec3 &)) &norm);
	m.def("norm", (float(*)(const vec2 &)) &norm);
	m.def("normalize", (vec3(*)(const vec3 &)) &normalize);
	m.def("normalize", (vec2(*)(const vec2 &)) &normalize);

	m.def("max_speed", &max_speed);
	m.def("max_curvature", &max_curvature);


}
