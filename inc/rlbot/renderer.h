#pragma once

#include "linear_algebra/vec.h"
#include "simulation/geometry.h"

#include "flatbuffers/flatbuffers.h"

#include "rlbot/color.h"
#include "rlbot/rlbot_generated.h"

#include <vector>
#include <stdint.h>

class Renderer {
 public:
  Renderer(int);
  void Start();
  void DrawLine3D(Color color, vec3 start, vec3 end);
  void DrawOBB(Color color, obb box);
  void DrawSphere(Color color, sphere s);
  void DrawPolyLine3D(Color color, std::vector<vec3> vertices);
  void DrawString2D(Color, std::string, vec2 topleft, int scaleX, int scaleY);
  void DrawString3D(Color, std::string, vec3 topleft, int scaleX, int scaleY);
  void Finish();

 private:
  int index;
  flatbuffers::FlatBufferBuilder builder;
  std::vector<flatbuffers::Offset<rlbot::flat::RenderMessage>> messages;
};
