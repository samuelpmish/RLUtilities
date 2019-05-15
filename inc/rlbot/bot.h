#pragma once

#include <string>

#include "simulation/game.h"

#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

class Bot {

 public:
  int index;
  int team;
  std::string name;

  Bot(int index, int team, std::string name);
  virtual Input GetOutput(Game g);

  void SendQuickChat(rlbot::flat::QuickChatSelection message, bool teamOnly);
};
