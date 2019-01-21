#pragma once

#include "simulation/car.h"
#include "simulation/input.h"
#include "simulation/curve.h"

#include "linear_algebra/math.h"

#include <vector>

class Unroller {

public:
	Unroller();

	void setup(vec3 x, mat3 o, vec3 t, vec3 n);

	vec3 unroll_point(vec3 p);
	vec3 unroll_direction(vec3 p);

	vec3 roll_point(vec3 p);
	vec3 roll_direction(vec3 p);

private:
	vec3 car_pos;
	mat3 car_orientation;

	vec3 target;
	vec3 normal;

	float r, h, theta;
	vec3 O;
	mat3 S, frame;
};
