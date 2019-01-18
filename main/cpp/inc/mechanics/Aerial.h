#pragma once

#include "simulation/Car.h"
#include "simulation/RL_structs.h"

#include "mechanics/Dodge.h"
#include "mechanics/AerialTurn.h"

#include "linear_algebra/math.h"

#include <string>
#include <optional>

class Aerial {

public:

	Car & car;

	float arrival_time;
	vec3 target;
	std::optional < mat3 > target_orientation;
	vec3 up;
	float angle_threshold;
	float reorient_distance;
	float throttle_distance;

	Input controls;
	bool finished;

	Aerial(Car & c);

	void step(float dt);

	Car simulate();

	static const float boost_accel;
	static const float throttle_accel;

private:

	bool jumping;

	Dodge double_jump;
	AerialTurn turn;

};
