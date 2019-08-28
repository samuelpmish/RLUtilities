#pragma once

#include "linear_algebra/vec.h"
#include "simulation/geometry.h"

#include "misc/rlu_dll.h"

#include "color.h"
#include "scopedrenderer.h"

#include <vector>

class RLURenderer {
 public:
  RLU_DLL RLURenderer(std::string name);
  RLU_DLL void DrawLine3D(rlbot::Color color, vec3 start, vec3 end);
  RLU_DLL void DrawOBB(rlbot::Color color, obb box);
  RLU_DLL void DrawSphere(rlbot::Color color, sphere s);
  RLU_DLL void DrawPolyLine3D(rlbot::Color color, std::vector<vec3> vertices);
  RLU_DLL void DrawString2D(rlbot::Color, std::string, vec2 topleft, int scaleX, int scaleY);
  RLU_DLL void DrawString3D(rlbot::Color, std::string, vec3 topleft, int scaleX, int scaleY);

 private:
  rlbot::ScopedRenderer scoped_renderer;
};
