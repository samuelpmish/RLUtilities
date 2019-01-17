#pragma once

#include "Car.h"
#include "Dodge.h"
#include "AerialTurn.h"
#include "linalg.h"
#include "RL_structs.h"

#include <string>
#include <optional>

class Boostdash {

public:

	Car & car;


	Input controls;
	bool finished;

	Boostdash(Car & c);

	void step(float dt);

	Car simulate();

private:

	float timer;
	float turn_up;
	float boost_off;

	Dodge dodge;
	AerialTurn turn;

};
