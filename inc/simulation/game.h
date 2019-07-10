#pragma once

#include <array>
#include <fstream>
#include <string>
#include <vector>

#include "simulation/ball.h"
#include "simulation/car.h"
#include "simulation/goal.h"
#include "simulation/pad.h"

#include "rlbot/rlbot_generated.h"

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#endif

enum class UpdateStatus { InvalidData, OldData, NewData };

class Game {
 public:
  Game();

  int frame;
  int frame_delta;

  float time;
  float time_delta;
  float time_remaining;

  int num_cars;
  bool overtime;
  bool round_active;
  bool kickoff_pause;
  bool match_ended;

  static float gravity;
  static float frametime;
  static std::string map;
  static std::string mode;

  Ball ball;

  std::array<Car, 8> cars;

  std::vector<Pad> pads;

  static void set_mode(std::string);

#ifdef GENERATE_PYTHON_BINDINGS
  void read_game_information(pybind11::object gametick_packet,
                             pybind11::object phystick_packet,
                             pybind11::object fieldinfo_packet);
#else
  int SetState();
  UpdateStatus GetState();

  void read_flatbuffer_packet(const rlbot::flat::GameTickPacket *gameTickPacket,
                              const rlbot::flat::FieldInfo *fieldInfo);
#endif

};
