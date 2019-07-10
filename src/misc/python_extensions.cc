#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

void init_car(pybind11::module & m);
void init_ball(pybind11::module & m);
void init_pad(pybind11::module & m);
void init_goal(pybind11::module & m);
void init_field(pybind11::module & m);
void init_geometry(pybind11::module & m);
void init_curve(pybind11::module & m);
void init_game(pybind11::module & m);
void init_navigator(pybind11::module & m);

void init_jump(pybind11::module & m);
void init_drive(pybind11::module & m);
void init_dodge(pybind11::module & m);
void init_aerial(pybind11::module & m);
void init_wavedash(pybind11::module & m);
void init_boostdash(pybind11::module & m);
void init_reorient(pybind11::module & m);
void init_reorient_ML(pybind11::module & m);
void init_followpath(pybind11::module & m);

void init_linalg(pybind11::module & m);

PYBIND11_MODULE(rlutilities, m) {

  pybind11::module simulation = m.def_submodule("simulation");
	init_car(simulation);
	init_ball(simulation);
	init_pad(simulation);
	init_goal(simulation);
	init_field(simulation);
	init_geometry(simulation);
	init_curve(simulation);
	init_game(simulation);
	init_navigator(simulation);

  pybind11::module mechanics = m.def_submodule("mechanics");
	init_jump(mechanics);
	init_dodge(mechanics);
	init_drive(mechanics);
	init_aerial(mechanics);
	init_wavedash(mechanics);
	init_boostdash(mechanics);
  init_reorient(mechanics);
  init_reorient_ML(mechanics);
	init_followpath(mechanics);

  pybind11::module linear_algebra = m.def_submodule("linear_algebra");
	init_linalg(linear_algebra);

}
