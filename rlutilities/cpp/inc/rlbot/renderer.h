#pragma once

#include "linear_algebra/vec.h"

#include "flatbuffers/flatbuffers.h"
#include "rlbot/rlbot_generated.h"

#include <stdint.h>

#include <vector>

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

class Renderer {
 private:
  int _index;
  std::vector<flatbuffers::Offset<rlbot::flat::RenderMessage>> renderMessages;

 public:
  void DrawLine3D(Color color, vec3 start, vec3 end);
  void DrawPolyLine3D(Color color, std::vector<vec3> vertices);
  void DrawString2D(std::string text, Color c, vec2 topleft, int scaleX,
                    int scaleY);
  void DrawString3D(std::string text, Color c, vec3 topleft, int scaleX,
                    int scaleY);

 protected:
  Renderer(int index);
  flatbuffers::FlatBufferBuilder flatBufferBuilder;
  void Finish();
};
