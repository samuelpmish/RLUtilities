#pragma once

#include "misc/convert.h"


namespace convert {

	vec3 vector3_to_vec3(pybind11::object vector3) {
		return vec3{
			vector3.attr("x").cast<float>(),
			vector3.attr("y").cast<float>(),
			vector3.attr("z").cast<float>()
		};
	}

	mat3 rotator_to_mat3(pybind11::object rotator) {
		return euler_to_rotation(vec3{
			rotator.attr("pitch").cast<float>(),
			rotator.attr("yaw").cast<float>(),
			rotator.attr("roll").cast<float>() 
    });
	}

}
