#pragma once

#include "rlbot/bot.h"

class ExampleBot : public Bot {
 public:
  ExampleBot(int _index, int _team, std::string _name);
  Input GetOutput(Game g);
};
