#include "simulation/field.h"
#include "simulation/geometry.h"
#include "simulation/gametype.h"

#include "linear_algebra/math.h"

#include "misc/timer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

int main() {

  timer stopwatch;

  stopwatch.start();
  gametype(std::string("soccar"));
  stopwatch.stop();
  std::cout << "gametype(): " << stopwatch.elapsed() << std::endl;

  std::ofstream outfile("points.obj");

  stopwatch.start();

  int count = 0;
  float scale = 64.0f;
  int nx = int(4096.0f / scale);
  int ny = int(5120.0f / scale);
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
  std::cout << count << " collisions: " << stopwatch.elapsed() << std::endl;


  std::ifstream infile("navmesh_pruned.txt");
  outfile.open("navmesh_pruned_with_normals.txt");

  vec3 p, n;
  std::string line;
  while (std::getline(infile, line)) {

    std::istringstream istream(line);

    istream >> p[0];
    istream >> p[1];
    istream >> p[2];

    n = Field::collide(sphere{p, 30.0f}).direction;

    if (norm(n) == 0.0f) {
      std::cout << p[0] << ' ' << p[1] << ' ' << p[2] << ' ';
      std::cout << n[0] << ' ' << n[1] << ' ' << n[2] << '\n';
      break;
    }

    outfile << p[0] << ' ' << p[1] << ' ' << p[2] << ' ';
    outfile << n[0] << ' ' << n[1] << ' ' << n[2] << '\n';
    
  }


  outfile.close();
  stopwatch.stop();
  std::cout << stopwatch.elapsed() << std::endl;

  return 0;

}
