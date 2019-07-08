#pragma once

#include <string>

#include "misc/rlu_dll.h"

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
  RLU_DLL static void LoadInterface(std::string dll);
  RLU_DLL static bool IsInitialized();
  RLU_DLL static void Free(void* ptr);

  RLU_DLL static ByteBuffer UpdateLiveDataPacketFlatbuffer();
  RLU_DLL static ByteBuffer UpdateFieldInfoFlatbuffer();
  RLU_DLL static ByteBuffer GetBallPrediction();

  RLU_DLL static int SetBotInput(Input inputs, int index);
  RLU_DLL static int RenderGroup(void* data, int size);
  RLU_DLL static int SendQuickChat(rlbot::flat::QuickChatSelection message,
                                   int botIndex, bool teamOnly);
  RLU_DLL static int SetGameState(void* data, int size);
  RLU_DLL static int SetGameState(Game g);
  RLU_DLL static int StartMatch(int num_players = 1);
};
