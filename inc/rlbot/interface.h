#pragma once

#include <string>

#include "simulation/game.h"
#include "simulation/input.h"
#include "rlbot/rlbot_generated.h"

#ifdef _WIN64
#define DLLNAME "RLBot_Core_Interface.dll"
#else
#ifdef _WIN32
#define DLLNAME "RLBot_Core_Interface_32.dll"
#endif
#endif

struct ByteBuffer {
  void* ptr;
  int32_t size;
};

class Interface {
 public:
  static void LoadInterface(std::string dll);
  static bool IsInitialized();
  static void Free(void* ptr);

  static ByteBuffer UpdateLiveDataPacketFlatbuffer();
  static ByteBuffer UpdateFieldInfoFlatbuffer();
  static ByteBuffer GetBallPrediction();

  static int SetBotInput(Input inputs, int index);
  static int RenderGroup(void* data, int size);
  static int SendQuickChat(rlbot::flat::QuickChatSelection message,
                           int botIndex, bool teamOnly);
  static int SetGameState(void* data, int size);
  static int SetGameState(Game g);
  static int StartMatch(int num_players = 1);
};
