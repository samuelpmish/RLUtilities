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

PYBIND11_MODULE(Simulation, m) {
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
    .def("hitbox", &Car::hitbox)
    .def("extrapolate", &Car::extrapolate)
    .def("forward", &Car::forward)
    .def("left", &Car::left)
    .def("up", &Car::up)
    .def_readwrite("time", &Car::time)
    .def_readwrite("id", &Car::id)
    .def_readwrite("team", &Car::team)
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

  m.def("max_speed", &max_speed);
  m.def("max_curvature", &max_curvature);
}
