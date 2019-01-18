#pragma once

#include "simulation/Car.h"
#include "simulation/RL_structs.h"

#include "mechanics/Dodge.h"
#include "mechanics/AerialTurn.h"

#include "linear_algebra/math.h"

#include <string>
#include <optional>

class Wavedash {

public:

	Car & car;

	std::optional < vec3 > direction;

	Input controls;
	bool finished;

	Wavedash(Car & c);

	void step(float dt);

	Car simulate();

private:

	float timer;

	Dodge dodge;
	AerialTurn turn;

};
