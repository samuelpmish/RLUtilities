#include "simulation/bvh.h"
#include "simulation/ogh.h"
#include "simulation/game.h"
#include "simulation/field.h"
#include "simulation/curve.h"
#include "simulation/geometry.h"

#include "mechanics/drive.h"

#include "linear_algebra/math.h"

#include "misc/io.h"
#include "misc/timer.h"
#include "misc/graph.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <inttypes.h>
#include <set>
#include <unordered_set>

#include <cmath>
#include <limits>

#include <omp.h>

// fuck visual C++
const float twopi = 6.28318530f;

int ntheta = 16;
float scale = 128.0f;

timer stopwatch;
size_t nnodes;
std::vector < vec3 > navigation_nodes;
std::vector < vec3 > navigation_normals;

void voxel_intersection(std::string filename) {

  std::ofstream outfile(filename);

  stopwatch.start();

  int count = 0;
  int nx = int(4096.0f / scale);
  int ny = int(5600.0f / scale);
  int nz = int(2048.0f / scale);
  for (int i = 0; i <= nx; i++) {
    for (int j = 0; j <= ny; j++) {
      for (int k = 0; k <= nz; k++) {

        vec3 center{i * scale, j * scale, k * scale};

        aabb box(center[0] - 0.55f * scale, 
                 center[1] - 0.55f * scale,
                 center[2] - 0.55f * scale,
                 center[0] + 0.55f * scale, 
                 center[1] + 0.55f * scale,
                 center[2] + 0.55f * scale);

        ray p = Field::collide(box);

        if (norm(p.direction) > 0.0f) {
          count++;
          outfile << "v " << p.start[0] << " " << p.start[1] << " " << p.start[2] << '\n';
        }

      }
    }
  }
  outfile.close();
  stopwatch.stop();
  std::cout << "found " << count << " voxel intersections in " << stopwatch.elapsed() << "s" << std::endl;

}

int main(int argc, char** argv) {

  std::string mode("soccar");
  std::string filename("vertices.obj");

  // parse arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);

    if ((arg == "-scale") && i + 1 < argc) {
      scale = atof(argv[++i]);
    } else if ((arg == "-o") && i + 1 < argc) {
      filename = std::string(argv[++i]);
    } else if (arg == "-soccar") {
      mode = std::string("soccar");
    } else if (arg == "-hoops") {
      mode = std::string("hoops");
    } else if (arg == "-dropshot") {
      mode = std::string("dropshot");
    } else if (arg == "-throwback") {
      mode = std::string("throwback");
    } else {
      std::cerr << "Bad option: '" << arg << "'" << std::endl;
    }
  }

  Game::set_mode(mode);
  std::cout << "setting gamemode: " << mode << std::endl;

  voxel_intersection(filename);

  return 0;

}
