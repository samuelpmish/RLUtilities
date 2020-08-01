#pragma once

#include <vector>
#include <string>

#include "simulation/bvh.h"
#include "simulation/geometry.h"

 class Field {

  struct wall {
    vec3 p;
    vec3 n;
    bool collidable;
  };

 public:
  static std::string mode;
  static bvh<tri> collision_mesh;
  static bvh<sphere> navigation_mesh;
  static std::vector<tri> triangles;

  static void initialize_soccar();
  static void initialize_hoops();
  static void initialize_dropshot();
  static void initialize_throwback();

  static ray snap(vec3 p);
  static ray collide(const obb & o);
  static ray collide(const aabb & a);
  static ray collide(const sphere & o);

  // if a hit is found, returns a ray where:
  // ray.start is the position of the intersection
  // ray.direction is the unit normal of the intersected surface
  // 
  // if no hit is found, then ray.normal will be {0,0,0}
  //
  // Note: for raycast_nearest(), the query is more expensive
  // for long rays, so limit the magnitude of direction, when possible
  static ray raycast_any(const ray &);
  static ray raycast_nearest(const ray &);

};
