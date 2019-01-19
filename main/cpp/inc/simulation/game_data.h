#pragma once

#include <vector>

#include "simulation/car.h"
#include "simulation/ball.h"
#include "simulation/pad.h"
#include "simulation/goal.h"

class GameData
{
public:
	// cars
	Car my_car;

	std::vector<Car*> opponents;
	std::vector<Car*> teammates;

	// ball
	Ball ball;

	// boost pads
	std::vector<Pad*> large_pads;
	std::vector<Pad*> small_pads;

	// goals
	std::vector<Goal*> opp_goals;
	std::vector<Goal*> own_goals;
};