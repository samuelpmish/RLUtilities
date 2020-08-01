#include "simulation/field.h"
#include "simulation/mesh.h"

// soccar meshes
extern mesh soccar_corner;
extern mesh soccar_goal;
extern mesh soccar_ramps_0;
extern mesh soccar_ramps_1;

// hoops meshes
extern mesh hoops_corner;
extern mesh hoops_ramps_0;
extern mesh hoops_ramps_1;
extern mesh hoops_net;
extern mesh hoops_rim;

// dropshot mesh
extern mesh dropshot;

// throwback meshes
extern mesh throwback_goal;
extern mesh throwback_side_ramps_lower;
extern mesh throwback_side_ramps_upper;
extern mesh throwback_back_ramps_lower;
extern mesh throwback_back_ramps_upper;
extern mesh throwback_corner_ramps_lower;
extern mesh throwback_corner_ramps_upper;
extern mesh throwback_corner_wall_0;
extern mesh throwback_corner_wall_1;
extern mesh throwback_corner_wall_2;

mat3 flip_x = {{-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

mat3 flip_y = {{1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

bvh<tri> Field::collision_mesh;
std::vector<tri> Field::triangles;
std::string Field::mode = std::string("Uninitialized");

mesh quad(vec3 p, vec3 e1, vec3 e2) {
  vec3 vertices[4] = {p + e1 + e2, p - e1 + e2, p - e1 -e2, p + e1 - e2};
  return mesh(
    {0, 1, 3, 1, 2, 3},
    {
      vertices[0][0], vertices[0][1], vertices[0][2], 
      vertices[1][0], vertices[1][1], vertices[1][2], 
      vertices[2][0], vertices[2][1], vertices[2][2], 
      vertices[3][0], vertices[3][1], vertices[3][2]
    }
  );
}

void Field::initialize_soccar() {

  mesh floor = quad({0.0f, 0.0f, 0.0f}, {4096.0f, 0.0f, 0.f}, {0.0f, 5120.0f, 0.0f});
  mesh ceiling = quad({0.0f, 0.0f, 2048.0f}, {-4096.0f, 0.0f, 0.f}, {0.0f, 5120.0f, 0.0f});
  mesh side_walls[] = {
    quad({4096.0f, 0.0f, 1024.0f}, {0.0f, -5120.0f, 0.f}, {0.0f, 0.0f, 1024.0f}),
    quad({-4096.0f, 0.0f, 1024.0f}, {0.0f, 5120.0f, 0.f}, {0.0f, 0.0f, 1024.0f}),
  };

  mesh soccar = {
      soccar_corner,
      soccar_corner.transform(flip_x),
      soccar_corner.transform(flip_y),
      soccar_corner.transform(dot(flip_x, flip_y)),
      soccar_goal.translate(vec3{0.0f, -5120.0f, 0.0f}),
      soccar_goal.translate(vec3{0.0f, -5120.0f, 0.0f}).transform(flip_y),
      soccar_ramps_0,
      soccar_ramps_0.transform(flip_x),
      soccar_ramps_1,
      soccar_ramps_1.transform(flip_x),
      floor, ceiling, 
      side_walls[0], side_walls[1]
    };
  triangles = soccar.to_triangles();
  collision_mesh = bvh<tri>(triangles);

  mode = std::string("soccar");
}

void Field::initialize_hoops() {
  float scale = 0.9f;
  float y_offset = 431.664f;

  mat3 S = {
      {scale, 0.0f, 0.0f},
      {0.0f, scale, 0.0f},
      {0.0f, 0.0f, scale},
  };

  vec3 dy = vec3{0.0f, y_offset, 0.0f};

  mesh transformed_hoops_net = hoops_net.transform(S).translate(dy);
  mesh transformed_hoops_rim = hoops_rim.transform(S).translate(dy);

  mesh floor = quad({0.0f, 0.0f, 0.0f}, {2966.6f, 0.0f, 0.f}, {0.0f, 3581.0f, 0.0f});
  mesh ceiling = quad({0.0f, 0.0f, 1820.0f}, {-2966.6f, 0.0f, 0.f}, {0.0f, 3581.0f, 0.0f});
  mesh side_walls[] = {
    quad({2966.6f, 0.0f, 910.0f}, {0.0f, -3581.0f, 0.f}, {0.0f, 0.0f, 910.0f}),
    quad({-2966.6f, 0.0f, 910.0f}, {0.0f, 3581.0f, 0.f}, {0.0f, 0.0f, 910.0f})
  };

  mesh back_walls[] = {
    quad({0.0f, 0.0f, 1024.0f}, {0.0f, -5120.0f, 0.f}, {0.0f, 0.0f, 1024.0f}),
    quad({0.0f, 0.0f, 1024.0f}, {0.0f, 5120.0f, 0.f}, {0.0f, 0.0f, 1024.0f})
  };

  mesh hoops = {hoops_corner,
                hoops_corner.transform(flip_x),
                hoops_corner.transform(flip_y),
                hoops_corner.transform(dot(flip_x, flip_y)),
                transformed_hoops_net,
                transformed_hoops_net.transform(flip_y),
                transformed_hoops_rim,
                transformed_hoops_rim.transform(flip_y),
                hoops_ramps_0,
                hoops_ramps_0.transform(flip_x),
                hoops_ramps_1,
                hoops_ramps_1.transform(flip_y),
                floor, ceiling, 
                side_walls[0], side_walls[1], 
                back_walls[0], back_walls[1]
                };

  triangles = hoops.to_triangles();
  collision_mesh = bvh<tri>(triangles);

  mode = std::string("hoops");
}

void Field::initialize_dropshot() {
  float scale = 0.393f;
  float z_offset = -207.565f;

  mat3 Q = axis_to_rotation(vec3{0.0f, 0.0f, 0.52359877559f});

  mat3 S = {
      {scale, 0.0f, 0.0f},
      {0.0f, scale, 0.0f},
      {0.0f, 0.0f, scale},
  };

  vec3 dz = vec3{0.0f, 0.0f, z_offset};

  mesh floor = quad({0.0f, 0.0f, 2.0f}, {10000.0f, 0.0f, 0.0f}, {0.0f, 7000.0f, 0.0f});
  mesh ceiling = quad({0.0f, 0.0f, 2020.0f}, {-10000.0f, 0.0f, 0.f}, {0.0f, 7000.0f, 0.0f});
  mesh walls[6];

  vec3 p = vec3{0.0f, 11683.6f * scale, 2768.64f * scale - z_offset};
  vec3 x = vec3{5000.0f, 0.0f, 0.0f};
  vec3 z = vec3{0.0f, 0.0f, 1010.0f};
  mat3 R = axis_to_rotation(vec3{0.0f, 0.0f, 1.047197551196598f});
  for (int i = 0; i < 6; i++) {
    walls[i] = quad(p, x, z);
    p = dot(R, p);
    x = dot(R, x);
  }

  mesh transformed_dropshot = {
    dropshot.transform(dot(Q, S)).translate(dz),
    floor, ceiling, walls[0], walls[1], walls[2], walls[3], walls[4], walls[5] 
  };

  triangles = transformed_dropshot.to_triangles();
  collision_mesh = bvh<tri>(triangles);

  mode = std::string("dropshot");
}

void Field::initialize_throwback() {
  float scale = 100;

  mat3 S = {
      {scale, 0.0f, 0.0f},
      {0.0f, scale, 0.0f},
      {0.0f, 0.0f, scale},
  };

  mesh floor = quad({0.0f, 0.0f, 0.0f}, {4096.6f, 0.0f, 0.f}, {0.0f, 6910.0f, 0.0f});
  mesh ceiling = quad({0.0f, 0.0f, 2048.0f}, {-4096.6f, 0.0f, 0.f}, {0.0f, 6910.0f, 0.0f});
  mesh side_walls[] = {
    quad({4096.6f, 0.0f, 1024.0f}, {0.0f, -6910.0f, 0.0f}, {0.0f, 0.0f, 1024.0f}),
    quad({-4096.6f, 0.0f, 1024.0f}, {0.0f, 6910.0f, 0.0f}, {0.0f, 0.0f, 1024.0f}),
  };

  mesh back_walls[] = {
    quad({0.0f,  6910.0f, 1024.0f}, {4096.0f, 0.0f, 0.f}, {0.0f, 0.0f, 1024.0f}),
    quad({0.0f, -6910.0f, 1024.0f}, {-4096.0f, 0.0f, 0.f}, {0.0f, 0.0f, 1024.0f}),
  };

  mesh throwback = {
      throwback_goal.transform(S),
      throwback_goal.transform(S).transform(flip_y),

      throwback_side_ramps_lower.transform(S),
      throwback_side_ramps_lower.transform(S).transform(flip_x),
      throwback_side_ramps_upper.transform(S),
      throwback_side_ramps_upper.transform(S).transform(flip_x),

      throwback_back_ramps_lower.transform(S),
      throwback_back_ramps_lower.transform(S).transform(flip_y),
      throwback_back_ramps_upper.transform(S),
      throwback_back_ramps_upper.transform(S).transform(flip_y),

      throwback_corner_ramps_lower.transform(S),
      throwback_corner_ramps_lower.transform(S).transform(flip_x),
      throwback_corner_ramps_lower.transform(S).transform(flip_y),
      throwback_corner_ramps_lower.transform(S).transform(flip_y).transform(
          flip_x),
      throwback_corner_ramps_upper.transform(S),
      throwback_corner_ramps_upper.transform(S).transform(flip_x),
      throwback_corner_ramps_upper.transform(S).transform(flip_y),
      throwback_corner_ramps_upper.transform(S).transform(flip_y).transform(
          flip_x),

      throwback_corner_wall_0.transform(S),
      throwback_corner_wall_0.transform(S).transform(flip_x),
      throwback_corner_wall_0.transform(S).transform(flip_y),
      throwback_corner_wall_0.transform(S).transform(flip_y).transform(flip_x),

      throwback_corner_wall_1.transform(S),
      throwback_corner_wall_1.transform(S).transform(flip_x),
      throwback_corner_wall_1.transform(S).transform(flip_y),
      throwback_corner_wall_1.transform(S).transform(flip_y).transform(flip_x),

      throwback_corner_wall_2.transform(S),
      throwback_corner_wall_2.transform(S).transform(flip_x),
      throwback_corner_wall_2.transform(S).transform(flip_y),
      throwback_corner_wall_2.transform(S).transform(flip_y).transform(flip_x),

      floor, ceiling, side_walls[0], side_walls[1], back_walls[0], back_walls[1]
  };

  triangles = throwback.to_triangles();
  collision_mesh = bvh<tri>(triangles);

  mode = std::string("throwback");
}

ray Field::raycast_any(const ray& r) { return collision_mesh.raycast_any(r); }
ray Field::raycast_nearest(const ray& r) { return collision_mesh.raycast_nearest(r); }

void error_uninitialized_field() {
  std::cout << "Attempting to query an uninitialized Field." << std::endl;
  std::cout << "Please do one of the following before making a geometric "
               "query with the Field:"
            << std::endl;
  std::cout << "Game.set_mode(\"soccar\")" << std::endl;
  std::cout << "Game.set_mode(\"hoops\")" << std::endl;
  std::cout << "Game.set_mode(\"dropshot\")" << std::endl;
}

ray Field::collide(const obb& o) {
  return ray{vec3{0.0f, 0.0f, 0.0f}, vec3{0.0f, 0.0f, 0.0f}};
}

ray Field::collide(const aabb& a) {
  vec3 center =
      0.5f * vec3{a.max_x + a.min_x, a.max_y + a.min_y, a.max_z + a.min_z};
  vec3 diagonal{a.max_x - a.min_x, a.max_y - a.min_y, a.max_z - a.min_z};

  auto contact_point = ray{vec3{0.0, 0.0, 0.0}, vec3{0.0f, 0.0f, 0.0f}};

  if (mode == std::string("Uninitialized")) {
    error_uninitialized_field();
  } else {
    float count = 0.0f;

    auto tris_hit = collision_mesh.intersect(a);

    for (const auto id : tris_hit) {
      vec3 p = triangles[id].center();
      vec3 n = triangles[id].unit_normal();

      vec3 abs_axis{fabs(n[0]), fabs(n[1]), fabs(n(2))};
      float radius = 0.5f * dot(abs_axis, diagonal);

      float separation = dot(center - p, n);
      if (fabs(separation) <= radius) {
        count++;
        contact_point.start += center - separation * n;
        contact_point.direction += n * (radius - separation);
      }
    }

    if (count > 0) {
      contact_point.start /= count;
      contact_point.direction = normalize(contact_point.direction);
    }
  }

  return contact_point;
}

ray Field::collide(const sphere& s) {
  auto contact_point = ray{vec3{0.0, 0.0, 0.0}, vec3{0.0f, 0.0f, 0.0f}};

  if (mode == std::string("Uninitialized")) {
    error_uninitialized_field();
  } else {
    float count = 0.0f;

    auto tris_hit = collision_mesh.intersect(s);

    for (const auto id : tris_hit) {
      vec3 p = triangles[id].center();
      vec3 n = triangles[id].unit_normal();

      float separation = dot(s.center - p, n);
      if (separation <= s.radius) {
        count++;
        contact_point.start += s.center - separation * n;
        contact_point.direction += n * (s.radius - separation);
      }
    }

    if (count > 0) {
      contact_point.start /= count;
      contact_point.direction = normalize(contact_point.direction);
    }
  }

  return contact_point;
}
