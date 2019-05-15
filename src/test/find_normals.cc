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

timer stopwatch;

void find_normals(std::string infile_name, std::string outfile_prefix) {

  std::ifstream infile(infile_name);

  stopwatch.start();

  std::vector < vec3 > nav_nodes;
  std::vector < vec3 > nav_normals;

  vec3 p, n;
  std::string line;
  while (std::getline(infile, line)) {

    std::istringstream istream(line);

    istream >> p[0];
    istream >> p[1];
    istream >> p[2];

    n = Field::collide(sphere{p, 30.0f}).direction;

    std::cout << n << std::endl;

    nav_nodes.push_back(p);
    nav_normals.push_back(n);
  }

  write_binary(nav_nodes, outfile_prefix + "_nodes.bin"); 
  write_binary(nav_normals, outfile_prefix + "_normals.bin"); 

  stopwatch.stop();
  std::cout << "located " << nav_nodes.size() << " normals in " << stopwatch.elapsed() << "s" << std::endl;

}

int main(int argc, char** argv) {

  std::string mode("soccar");
  std::string infile_name;
  std::string outfile_prefix("navigation");

  // parse arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);

    if ((arg == "-i") && i + 1 < argc) {
      infile_name = std::string(argv[++i]);
    } else if ((arg == "-prefix") && i + 1 < argc) {
      outfile_prefix = std::string(argv[++i]);
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

  if (!infile_name.empty()) {
    std::cout << "setting gamemode: " << mode << std::endl;
    Game::set_mode(mode);
    find_normals(infile_name, outfile_prefix);
  } else {
    std::cout << "No input file specified. Exiting..." << std::endl;
  }
  
  return 0;

}
