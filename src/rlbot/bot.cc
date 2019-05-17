#include "rlbot/bot.h"

#include <vector>

#include "rlbot/interface.h"
#include "rlbot/rlbot_generated.h"

Bot::Bot(int _index, int _team, std::string _name) {
  index = _index;
  team = _team;
  name = _name;
}

Input Bot::GetOutput(Game g) { 
  return Input(); 
}

void Bot::SendQuickChat(rlbot::flat::QuickChatSelection message,
                        bool teamOnly) {
  Interface::SendQuickChat(message, index, teamOnly);
}
