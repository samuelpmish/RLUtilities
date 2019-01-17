#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

void init_car(pybind11::module & m);
void init_ball(pybind11::module & m);
void init_field(pybind11::module & m);
void init_geometry(pybind11::module & m);
void init_curve(pybind11::module & m);

void init_jump(pybind11::module & m);
void init_drive(pybind11::module & m);
void init_dodge(pybind11::module & m);
void init_aerial(pybind11::module & m);
void init_wavedash(pybind11::module & m);
void init_boostdash(pybind11::module & m);
void init_aerialturn(pybind11::module & m);

void init_gametype(pybind11::module & m);

PYBIND11_MODULE(cpp_extension, m) {
	init_car(m);
	init_ball(m);
	init_field(m);
	init_geometry(m);
	init_curve(m);

	init_jump(m);
	init_dodge(m);
	init_drive(m);
	init_aerial(m);
	init_wavedash(m);
	init_boostdash(m);
	init_aerialturn(m);

	init_gametype(m);
}
