#pragma once

#include <vector>
#include <string>

#include "misc/rlu_dll.h"

#include "simulation/bvh.h"
#include "simulation/geometry.h"

 class Field {

  struct wall {
    vec3 p;
    vec3 n;
    bool collidable;
  };

 public:
  RLU_DLL static bvh<tri> collision_mesh;
  RLU_DLL static bvh<sphere> navigation_mesh;
  RLU_DLL static std::vector<wall> walls;
  RLU_DLL static std::vector<tri> triangles;
  RLU_DLL static std::vector<aabb> obstacles;
  RLU_DLL static std::string mode;

  RLU_DLL static void initialize_soccar();
  RLU_DLL static void initialize_hoops();
  RLU_DLL static void initialize_dropshot();
  RLU_DLL static void initialize_throwback();

  RLU_DLL static ray snap(vec3 p);
  RLU_DLL static ray collide(const obb & o);
  RLU_DLL static ray collide(const aabb & a);
  RLU_DLL static ray collide(const sphere & o);

  RLU_DLL static ray raycast_any(const ray &);

 private:
  static float r;
  static float R;

};
