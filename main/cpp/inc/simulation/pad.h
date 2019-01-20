#pragma once

#include "linear_algebra/math.h"

class Pad
{
public:
	int index;
	vec3 location;
	bool is_active;
	bool is_full_boost;
	float timer;
};