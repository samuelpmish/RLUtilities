#pragma once

#include <vector>

#include "bvh.h"

class Pitch {
public:
	aabb empty;
	bvh<tri> mesh;
	std::vector<int> hits;
	std::vector<tri> triangles;

	Pitch();

	bool in_contact_with(const obb &o);
	bool in_contact_with(const sphere &s);
	ray last_contact_info();

	ray raycast_any(const ray &);
};
