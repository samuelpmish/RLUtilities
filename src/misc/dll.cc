#define NOMINMAX

#include <Windows.h>
#include <iostream>
#include <string>

#include "misc/resource.h"
#include "simulation/mesh.h"
#include "mechanics/reorient_ML.h"

#include "experimental/navigator.h"

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

template <typename T>
std::vector<T> load_resource(HINSTANCE hinstDLL, int id) {
  auto resource = FindResourceA(hinstDLL, MAKEINTRESOURCEA(id),
                                MAKEINTRESOURCEA(BINARY_FILE));
  if (!resource) {
    std::cout << "failed to find resource: " << id << std::endl;
  }

  HGLOBAL handle = LoadResource(hinstDLL, resource);
  if (!handle) {
    std::cout << "failed to acquire handle for resource: " << id << std::endl;
  }

  char* data = reinterpret_cast<char*>(LockResource(handle));
  if (!data) {
    std::cout << "failed to lock resource: " << id << std::endl;
  }

  auto size = SizeofResource(hinstDLL, resource);
  return std::vector<T>(reinterpret_cast<T*>(data),
                        reinterpret_cast<T*>(data + size));
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
  // Perform actions based on the reason for calling.
  switch (fdwReason) {
    case DLL_PROCESS_ATTACH: {
      std::cout << "Initializing rlutilities...\n";
      std::cout << hinstDLL << std::endl;

      // clang-format off
      // soccar meshes
      soccar_corner = mesh(
          load_resource<int>(hinstDLL, SOCCAR_CORNER_IDS),
          load_resource<float>(hinstDLL, SOCCAR_CORNER_VERTICES));
      soccar_goal = mesh(
          load_resource<int>(hinstDLL, SOCCAR_GOAL_IDS),
          load_resource<float>(hinstDLL, SOCCAR_GOAL_VERTICES));
      soccar_ramps_0 = mesh(
          load_resource<int>(hinstDLL, SOCCAR_RAMPS_0_IDS),
          load_resource<float>(hinstDLL, SOCCAR_RAMPS_0_VERTICES));
      soccar_ramps_1 = mesh(
          load_resource<int>(hinstDLL, SOCCAR_RAMPS_1_IDS),
          load_resource<float>(hinstDLL, SOCCAR_RAMPS_1_VERTICES));

      // hoops meshes
      hoops_corner = mesh(
          load_resource<int>(hinstDLL, HOOPS_CORNER_IDS),
          load_resource<float>(hinstDLL, HOOPS_CORNER_VERTICES));
      hoops_ramps_0 = mesh(
          load_resource<int>(hinstDLL, HOOPS_RAMPS_0_IDS),
          load_resource<float>(hinstDLL, HOOPS_RAMPS_0_VERTICES));
      hoops_ramps_1 = mesh(
          load_resource<int>(hinstDLL, HOOPS_RAMPS_1_IDS),
          load_resource<float>(hinstDLL, HOOPS_RAMPS_1_VERTICES));
      hoops_net = mesh(
          load_resource<int>(hinstDLL, HOOPS_NET_IDS),
          load_resource<float>(hinstDLL, HOOPS_NET_VERTICES));
      hoops_rim = mesh(
          load_resource<int>(hinstDLL, HOOPS_RIM_IDS),
          load_resource<float>(hinstDLL, HOOPS_RIM_VERTICES));

      // dropshot mesh
      dropshot = mesh(
          load_resource<int>(hinstDLL, DROPSHOT_IDS),
          load_resource<float>(hinstDLL, DROPSHOT_VERTICES));
      
      // throwback meshes
      throwback_goal = mesh(
          load_resource<int>(hinstDLL, THROWBACK_GOAL_IDS),
          load_resource<float>(hinstDLL, THROWBACK_GOAL_VERTICES));
      throwback_side_ramps_lower = mesh(
          load_resource<int>(hinstDLL, THROWBACK_SIDE_RAMPS_LOWER_IDS),
          load_resource<float>(hinstDLL, THROWBACK_SIDE_RAMPS_LOWER_VERTICES));
      throwback_side_ramps_upper = mesh(
          load_resource<int>(hinstDLL, THROWBACK_SIDE_RAMPS_UPPER_IDS),
          load_resource<float>(hinstDLL, THROWBACK_SIDE_RAMPS_UPPER_VERTICES));
      throwback_back_ramps_lower = mesh(
          load_resource<int>(hinstDLL, THROWBACK_BACK_RAMPS_LOWER_IDS),
          load_resource<float>(hinstDLL, THROWBACK_BACK_RAMPS_LOWER_VERTICES));
      throwback_back_ramps_upper = mesh(
          load_resource<int>(hinstDLL, THROWBACK_BACK_RAMPS_UPPER_IDS),
          load_resource<float>(hinstDLL, THROWBACK_BACK_RAMPS_UPPER_VERTICES));
      throwback_corner_ramps_lower = mesh(
          load_resource<int>(hinstDLL, THROWBACK_CORNER_RAMPS_LOWER_IDS),
          load_resource<float>(hinstDLL, THROWBACK_CORNER_RAMPS_LOWER_VERTICES));
      throwback_corner_ramps_upper = mesh(
          load_resource<int>(hinstDLL, THROWBACK_CORNER_RAMPS_UPPER_IDS),
          load_resource<float>(hinstDLL, THROWBACK_CORNER_RAMPS_UPPER_VERTICES));
      throwback_corner_wall_0 = mesh(
          load_resource<int>(hinstDLL, THROWBACK_CORNER_WALL_0_IDS),
          load_resource<float>(hinstDLL, THROWBACK_CORNER_WALL_0_VERTICES));
      throwback_corner_wall_1 = mesh(
          load_resource<int>(hinstDLL, THROWBACK_CORNER_WALL_1_IDS),
          load_resource<float>(hinstDLL, THROWBACK_CORNER_WALL_1_VERTICES));
      throwback_corner_wall_2 = mesh(
          load_resource<int>(hinstDLL, THROWBACK_CORNER_WALL_2_IDS),
          load_resource<float>(hinstDLL, THROWBACK_CORNER_WALL_2_VERTICES));

      Navigator::init_statics(
          load_resource<Graph::edge>(hinstDLL, SOCCAR_NAV_GRAPH),
          load_resource<vec3>(hinstDLL, SOCCAR_NAV_NODES),
          load_resource<vec3>(hinstDLL, SOCCAR_NAV_NORMALS));

      ReorientML::set_model(
          Model(load_resource<float>(hinstDLL, REORIENT_ML_MODEL)));
      // clang-format on
      break;
    }

    case DLL_THREAD_ATTACH: {
      // Do thread-specific initialization.

      break;
    }

    case DLL_THREAD_DETACH: {
      // Do thread-specific cleanup.

      break;
    }

    case DLL_PROCESS_DETACH: {
      // Perform any necessary cleanup.

      break;
    }
  }

  // Successful DLL_PROCESS_ATTACH.

  return TRUE;
}
