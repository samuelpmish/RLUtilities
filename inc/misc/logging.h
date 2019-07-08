#pragma once

#include "misc/json.h"
#include "misc/interpolation.h"

#include "linear_algebra/math.h"

#include "simulation/car.h"
#include "simulation/ball.h"
#include "simulation/curve.h"
#include "simulation/input.h"

#include "mechanics/dodge.h"
#include "mechanics/aerial.h"

#include "experimental/follow_path.h"

nlohmann::json to_json(vec3);
nlohmann::json to_json(Car);

nlohmann::json to_json(Car);
nlohmann::json to_json(Ball);
nlohmann::json to_json(Curve);
nlohmann::json to_json(Input);

nlohmann::json to_json(Dodge);
nlohmann::json to_json(Aerial);
nlohmann::json to_json(FollowPath);
