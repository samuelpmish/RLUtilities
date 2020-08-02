#pragma once

#include <array>
#include <fstream>
#include <string>
#include <vector>

#include "simulation/ball.h"
#include "simulation/car.h"
#include "simulation/goal.h"
#include "simulation/boost_pad.h"

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#endif

enum class GameState {
  Inactive,
  Countdown,
  Kickoff,
  Active,
  GoalScored,
  Replay,
  Paused,
  Ended
};

enum class GameMode {
  Unknown, 
  Soccar, 
  Hoops, 
  Snowday, 
  Rumble, 
  Dropshot 
};

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#endif

struct Game {
  Game();

  int frame;
  int score[2];

  float time;
  float time_remaining;

  GameState state;

  static float gravity;
  static std::string map;
  static std::string mode;

  Ball ball;

  std::vector<Car> cars;

  std::vector<BoostPad> pads;

  std::vector<Goal> goals;

  static void set_mode(std::string);

  #ifdef GENERATE_PYTHON_BINDINGS
  void read_game_information(pybind11::object gametick,
                             pybind11::object phystick,
                             pybind11::object fieldinfo);
  #endif

};
