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

timer stopwatch;

// fuck visual C++
const float twopi = 6.28318530f;

float r = 500.0f;
int ntheta = 16;

void generate_edges(std::string infile_prefix, std::string outfile_name) {

  std::ofstream outfile(outfile_name, std::ios::binary);

  stopwatch.start();

  // read in the files
  auto navigation_nodes = read_binary< vec3 >(infile_prefix + std::string("_nodes.bin"));
  auto navigation_normals = read_binary< vec3 >(infile_prefix + std::string("_normals.bin"));
  int nnodes = navigation_nodes.size();

  // make a BVH to quickly find neighboring nodes
  std::vector < sphere > navigation_spheres(nnodes);
  for (int i = 0; i < nnodes; i++) {
    navigation_spheres[i] = sphere{navigation_nodes[i], 0.001f};
  }
  bvh < sphere > navnode_bvh(navigation_spheres);

  std::cout << nnodes << std::endl;

  std::vector < float > c(ntheta);
  std::vector < float > s(ntheta);
  for (int i = 0; i < ntheta; i++) {
    c[i] = cos(i * twopi / ntheta);
    s[i] = sin(i * twopi / ntheta);
  }

  int total = 0;

  for (int i = 0; i < nnodes; i++) {

    // find the nodes close to me
    std::vector<int> neighbors = navnode_bvh.intersect(sphere{navigation_nodes[i], r});
    std::sort(neighbors.begin(), neighbors.end());

    vec3 pi = navigation_nodes[i];
    vec3 ni = navigation_normals[i];
    mat3 iBasis = R3_basis(ni);

    for (int itheta = 0; itheta < ntheta; itheta++) {
      
      vec3 ti = dot(iBasis, vec3{c[itheta], s[itheta], 0.0f}); 

      for (auto j : neighbors) {

        // skip trivial path segments
        if (j == i) continue;

        vec3 pj = navigation_nodes[j];
        vec3 nj = navigation_normals[j];
        mat3 jBasis = R3_basis(nj);

        vec3 delta_p = pj - pi;

        // discard points that are behind me
        if (dot(delta_p, ti) < 0.0f) continue;

        for (int jtheta = 0; jtheta < ntheta; jtheta++) {

          vec3 tj = dot(jBasis, vec3{c[jtheta], s[jtheta], 0.0f}); 

          vec3 tj_para = normalize(tj - dot(tj, ni) * ni);

          float delta_theta = acos(clip(dot(ti, tj_para), -1.0f, 1.0f));

          // discard curves that have require too much turning
          if (delta_theta > 1.0f) continue;

          OGH g(pi, ti, pj, tj);

          if (g.a0 < 0 || g.a1 < 0) continue;

          Curve segment({ControlPoint{pi, ti, ni}, ControlPoint{pj, tj, nj}});

          float mean_curvature = abs(mean(segment.curvatures));
          float dev_curvature = standard_deviation(segment.curvatures);

          //if (mean_curvature < 0.003f && dev_curvature < 0.0015f) {
          if (mean_curvature < 0.0025f && dev_curvature < 0.0002f) {
            
            total++;            

            // calculate the least time to traverse this curve segment
            float time = segment.calculate_max_speeds(2300, 2300) + 0.05f;

            Graph::edge e{
              i * ntheta + itheta, 
              j * ntheta + jtheta, 
              time
            };

            outfile.write((char*)&e, sizeof(Graph::edge));

          }

        } // for each destination node direction

      } // for each destination

    } // for each source node direction

    if (i % 1000 == 0) {
      std::cout << i << " of " << nnodes << std::endl;
    }

  } // for each source node

  outfile.close();
  stopwatch.stop();
  std::cout << "computed " << total << " edges in " << stopwatch.elapsed() << "s" << std::endl;

}

int main(int argc, char** argv) {

  std::string mode("soccar");
  std::string infile_prefix;
  std::string outfile_name("navigation_edges.bin");

  // parse arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);

    if ((arg == "-prefix") && i + 1 < argc) {
      infile_prefix = std::string(argv[++i]);
    } else if ((arg == "-o") && i + 1 < argc) {
      outfile_name = std::string(argv[++i]);
    } else if ((arg == "-r") && i + 1 < argc) {
      r = atof(argv[++i]);
    } else if ((arg == "-ntheta") && i + 1 < argc) {
      ntheta = atoi(argv[++i]);
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

  if (!infile_prefix.empty()) {
    std::cout << "setting gamemode: " << mode << std::endl;
    Game::set_mode(mode);
    generate_edges(infile_prefix, outfile_name);
  } else {
    std::cout << "No input files specified. Exiting..." << std::endl;
  }
 
  return 0;

}
