#pragma once

#include <windows.h>
#include "bot.h"

template <typename bot_type>
void run_bot(bot_type bot) {

  Game g(bot.index, bot.team);

  float last_time = 0.0f;

  while (true) {
    ByteBuffer flatbufferData = Interface::UpdateLiveDataPacketFlatbuffer();

    // Don't try to read the packets when they are very small.
    if (flatbufferData.size > 4) {
      const rlbot::flat::GameTickPacket* gametickpacket =
          flatbuffers::GetRoot<rlbot::flat::GameTickPacket>(flatbufferData.ptr);

      // Only run the bot when we recieve a new packet.
      if (last_time != gametickpacket->gameInfo()->secondsElapsed()) {
        ByteBuffer fieldInfoData = Interface::UpdateFieldInfoFlatbuffer();

        // The fieldinfo or ball prediction might not have been set up by the
        // framework yet.
        if (fieldInfoData.size > 4) {
          const rlbot::flat::FieldInfo* fieldinfo =
              flatbuffers::GetRoot<rlbot::flat::FieldInfo>(fieldInfoData.ptr);

          g.read_flatbuffer_packet(gametickpacket, fieldinfo); 

          int status = Interface::SetBotInput(bot.GetOutput(g), bot.index);

          Interface::Free(fieldInfoData.ptr);
        }

        last_time = gametickpacket->gameInfo()->secondsElapsed();
      } else {
        Sleep(1);
      }
    } else {
      Sleep(100);
    }

    Interface::Free(flatbufferData.ptr);
  }
}
