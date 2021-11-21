#include "rlutilities.h"

#include "misc/config.h"
#include "misc/io.h"
#include "simulation/mesh.h"
#include "mechanics/reorient_ML.h"
#include "experimental/navigator.h"

// soccar meshes
mesh soccar_corner{};
mesh soccar_goal{};
mesh soccar_ramps_0{};
mesh soccar_ramps_1{};

// hoops meshes
mesh hoops_corner{};
mesh hoops_ramps_0{};
mesh hoops_ramps_1{};
mesh hoops_net{};
mesh hoops_rim{};

// dropshot mesh
mesh dropshot{};

// throwback meshes
mesh throwback_goal{};
mesh throwback_side_ramps_lower{};
mesh throwback_side_ramps_upper{};
mesh throwback_back_ramps_lower{};
mesh throwback_back_ramps_upper{};
mesh throwback_corner_ramps_lower{};
mesh throwback_corner_ramps_upper{};
mesh throwback_corner_wall_0{};
mesh throwback_corner_wall_1{};
mesh throwback_corner_wall_2{};

namespace rlu {

  void initialize(std::string asset_dir = ASSET_DIR) {

    auto read_mesh = [asset_dir](std::string str) {
      return mesh(
        read_binary<int>(asset_dir + str + "_ids.bin"),
        read_binary<float>(asset_dir + str + "_vertices.bin")
      );
    };

    soccar_corner = read_mesh("soccar/soccar_corner");
    soccar_goal = read_mesh("soccar/soccar_goal");
    soccar_ramps_0 = read_mesh("soccar/soccar_ramps_0");
    soccar_ramps_1 = read_mesh("soccar/soccar_ramps_1");
    hoops_corner = read_mesh("hoops/hoops_corner");
    hoops_ramps_0 = read_mesh("hoops/hoops_ramps_0");
    hoops_ramps_1 = read_mesh("hoops/hoops_ramps_1");
    hoops_net = read_mesh("hoops/hoops_net");
    hoops_rim = read_mesh("hoops/hoops_rim");
    dropshot = read_mesh("dropshot/dropshot");
    throwback_goal = read_mesh("throwback/throwback_goal");
    throwback_side_ramps_lower = read_mesh("throwback/throwback_side_ramps_lower");
    throwback_side_ramps_upper = read_mesh("throwback/throwback_side_ramps_upper");
    throwback_back_ramps_lower = read_mesh("throwback/throwback_back_ramps_lower");
    throwback_back_ramps_upper = read_mesh("throwback/throwback_back_ramps_upper");
    throwback_corner_ramps_lower = read_mesh("throwback/throwback_corner_ramps_lower");
    throwback_corner_ramps_upper = read_mesh("throwback/throwback_corner_ramps_upper");
    throwback_corner_wall_0 = read_mesh("throwback/throwback_corner_wall_0");
    throwback_corner_wall_1 = read_mesh("throwback/throwback_corner_wall_1");
    throwback_corner_wall_2 = read_mesh("throwback/throwback_corner_wall_2");

    auto prefix = asset_dir + std::string("soccar/soccar_navigation_");
    auto navigation_graph = read_binary<Graph::edge>(prefix + "graph.bin");
    auto navigation_nodes = read_binary<vec3>(prefix + "nodes.bin");
    auto navigation_normals = read_binary<vec3>(prefix + "normals.bin");
    if (!navigation_graph.empty() && !navigation_nodes.empty() && !navigation_normals.empty()) {
      Navigator::init_statics(navigation_graph, navigation_nodes, navigation_normals);
    }

    ReorientML::set_model(Model(read_binary<float>(asset_dir + "ML/reorient_ML_model.bin")));
    
    // User should call this instead.
    // Game::set_mode(mode);

  }

}
