#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>

#include "simulation/car.h"
void init_car(pybind11::module & m) {
	pybind11::class_<Car>(m, "Car")
		.def(pybind11::init<>())
		.def(pybind11::init<const Car &>())
		.def("step", &Car::step)
		.def("hitbox", &Car::hitbox)
		.def("extrapolate", &Car::extrapolate)
		.def("forward", &Car::forward)
		.def("left", &Car::left)
		.def("up", &Car::up)
		.def_readwrite("time", &Car::time)
		.def_readwrite("id", &Car::id)
		.def_readwrite("team", &Car::team)
		.def_readwrite("position", &Car::position)
		.def_readwrite("velocity", &Car::velocity)
		.def_readwrite("angular_velocity", &Car::angular_velocity)
		.def_readwrite("orientation", &Car::orientation)
		.def_readwrite("supersonic", &Car::supersonic)
		.def_readwrite("jumped", &Car::jumped)
		.def_readwrite("double_jumped", &Car::double_jumped)
		.def_readwrite("on_ground", &Car::on_ground)
		.def_readwrite("jump_timer", &Car::jump_timer)
		.def_readwrite("dodge_timer", &Car::dodge_timer)
		.def_readwrite("boost", &Car::boost)
		.def_readwrite("controls", &Car::controls);

	pybind11::class_<Input>(m, "Input")
		.def(pybind11::init<>())
		.def_readwrite("steer", &Input::steer)
		.def_readwrite("roll", &Input::roll)
		.def_readwrite("pitch", &Input::pitch)
		.def_readwrite("yaw", &Input::yaw)
		.def_readwrite("throttle", &Input::throttle)
		.def_readwrite("jump", &Input::jump)
		.def_readwrite("boost", &Input::boost)
		.def_readwrite("handbrake", &Input::handbrake)
		.def_readwrite("useItem", &Input::useItem);
}
#endif
