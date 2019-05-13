#pragma once

#include "renderer.h"

#include <string>

class NamedRenderer : public Renderer {
 public:
  NamedRenderer(std::string name);
  void StartPacket();
  void FinishAndSend();
};
