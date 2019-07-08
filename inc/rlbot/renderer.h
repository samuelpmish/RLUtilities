#pragma once

#include "linear_algebra/vec.h"
#include "simulation/geometry.h"

#include "flatbuffers/flatbuffers.h"

#include "misc/rlu_dll.h"

#include "rlbot/color.h"
#include "rlbot/rlbot_generated.h"

#include <vector>
#include <stdint.h>

class Renderer {
 public:
  RLU_DLL Renderer(int);
  RLU_DLL void Start();
  RLU_DLL void DrawLine3D(Color color, vec3 start, vec3 end);
  RLU_DLL void DrawOBB(Color color, obb box);
  RLU_DLL void DrawSphere(Color color, sphere s);
  RLU_DLL void DrawPolyLine3D(Color color, std::vector<vec3> vertices);
  RLU_DLL void DrawString2D(Color, std::string, vec2 topleft, int scaleX, int scaleY);
  RLU_DLL void DrawString3D(Color, std::string, vec3 topleft, int scaleX, int scaleY);
  RLU_DLL void Finish();

 private:
  int index;
  flatbuffers::FlatBufferBuilder builder;
  std::vector<flatbuffers::Offset<rlbot::flat::RenderMessage>> messages;
};
