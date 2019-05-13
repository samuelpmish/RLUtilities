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

const int ntheta = 16;

//void sssp() {
//
//  std::ifstream infile(path + std::string("edges_morton.bin"), std::ios::binary);
//
//  size_t n;
//
//  std::vector< edge_info > edges;
//  std::vector< uint32_t > offsets;
//
//  if (infile) {
//
//    stopwatch.start();
//
//    infile.seekg(0,std::ios::end);
//    std::streampos filesize = infile.tellg();
//    infile.seekg(0,std::ios::beg);
//
//    size_t num_edges = filesize / sizeof(edge_info);
//    edges.resize(num_edges);
//    infile.read((char*)&edges[0], filesize);
//
//    stopwatch.stop();
//    std::cout << "read " << num_edges << " edges from file in " << stopwatch.elapsed() << "s" << std::endl;
//
//  } else {
//
//    std::cout << "file not found: " << path + std::string("edges_morton.bin") << std::endl;
//
//  }
//
//  infile.close();
//
//  infile.open(path + std::string("offsets_morton.bin"), std::ios::binary);
//
//  if (infile) {
//
//    stopwatch.start();
//
//    infile.seekg(0,std::ios::end);
//    std::streampos filesize = infile.tellg();
//    infile.seekg(0,std::ios::beg);
//
//    n = filesize / sizeof(uint32_t);
//    offsets.resize(n + 1);
//    infile.read((char*)&offsets[0], filesize);
//
//    stopwatch.stop();
//    std::cout << "read " << n << " nodes from file in " << stopwatch.elapsed() << "s" << std::endl;
//
//  } else {
//
//    std::cout << "file not found: " << path + std::string("offsets_morton.bin") << std::endl;
//
//  }
//
//  // Only select navmesh nodes that are close by
//  vec3 where_I_am = vec3{000.0f, 000.0f, 17.0f};
//
//  std::vector < sphere > navigation_spheres(nnodes);
//
//  for (int i = 0; i < nnodes; i++) {
//    navigation_spheres[i] = sphere{navigation_nodes[i], 0.001f};
//  }
//
//  bvh < sphere > navnode_bvh(navigation_spheres);
//
//  stopwatch.start();
//  auto hits = navnode_bvh.intersect(sphere{where_I_am, 5000.0f});
//  stopwatch.stop();
//  std::cout << "find nearby navnodes: " << stopwatch.elapsed() << std::endl;
//  std::cout << "number of nearby navnodes: " << hits.size() << std::endl;
//
//  std::vector < char > is_nearby(nnodes, 0);
//  int which_point = -1;
//  float min_distance = 100000.0f;
//  for (const int hit : hits) {
//
//    is_nearby[hit] = 1;
//
//    float distance = norm(where_I_am - navigation_nodes[hit]);
//    
//    if (distance < min_distance) {
//      which_point = hit;
//      min_distance = distance;
//    }
//
//  }
//
//  std::cout << which_point << " " << where_I_am << " " << navigation_nodes[which_point] << " " << min_distance << std::endl;
//
//  int which_dir = 9;
//  uint32_t which = which_point * ntheta + which_dir; 
//
//  std::vector< edge_info > edges_filtered;
//  std::vector< uint32_t > offsets_filtered(offsets.size(), 0);
//
//  edges_filtered.reserve(1<<24);
//
//  stopwatch.start();
//  for (int i = 0; i < nnodes * ntheta; i++) {
//
//    if (is_nearby[i / ntheta]) {
//
//      uint32_t begin = offsets[i];
//      uint32_t end = offsets[i+1];
//
//      for (uint32_t j = begin; j < end; j++) {
//
//        if (is_nearby[edges[j].id / ntheta]) {
//          edges_filtered.push_back(edges[j]);
//        }
//       
//      }
//
//    }
//
//    offsets_filtered[i+1] = int(edges_filtered.size());
//
//  }
//  stopwatch.stop();
//  std::cout << "filtering: " << stopwatch.elapsed() << std::endl;
//  std::cout << "edges remaining: " << edges_filtered.size() << std::endl;
//
//  std::vector < uint32_t > next;
//  next.reserve(1<<20);
//
//  stopwatch.start();
//  int total = 1;
//
//  std::vector < char > considered(n, 0);
//  std::vector < edge_info > best(n, edge_info{0, 100.0f});
//  best[which] = edge_info{which, 0.0f};
//
//  std::vector < uint32_t > active;
//  active.push_back(which);
//  for (int iter = 0; iter < 26; iter++) {
//
//    next.clear();
//
//    //for (const uint32_t source : active) {
//    for (int q = 0; q < active.size(); q++) {
//   
//      uint32_t source = active[q];
//
//      //uint32_t begin = offsets[source];
//      //uint32_t end = offsets[source+1];
//      uint32_t begin = offsets_filtered[source];
//      uint32_t end = offsets_filtered[source+1];
//
//      float this_time = best[source].time;
//
//      for (uint32_t j = begin; j < end; j++) {
//
//        //edge_info e = edges[j];
//        edge_info e = edges_filtered[j];
//
//        uint32_t destination = e.id;
//        float delta_time = e.time;
//        
//        float that_time = best[destination].time;
//
//        if (this_time + delta_time < that_time) {
//
//          best[destination] = edge_info{source, this_time + delta_time};
//
//          if (!considered[destination]) {
//            next.push_back(destination);
//            considered[destination] = 1;
//          }
//
//        }
//
//      }
//
//      total += end-begin;
//
//    }
//
//    active = next;
//
//    considered = std::vector < char >(n, 0);
//
//  }
//
//  stopwatch.stop();
//  std::cout << total << " edges traversed in " << stopwatch.elapsed() << "s" << std::endl;
//
//  std::ofstream outfile(path + std::string("results.txt"));
//
//  for (int i = 0; i < best.size(); i++) {
//    outfile << best[i].id << " " << best[i].time << '\n';
//  }
//
//  outfile.close();
//
//}
//
int main() {

#if 0
  std::string mode("soccar");
  std::string infile_prefix;
  std::string outfile_name("navigation_edges.bin");

  // parse arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);

    if ((arg == "-prefix") && i + 1 < argc) {
      infile_prefix = std::string(argv[++i]);
    } else if ((arg == "-o") && i + 1 < argc) {
      outfile = std::string(argv[++i]);
    } else if ((arg == "-r") && i + 1 < argc) {
      r = atod(argv[++i]);
    } else if ((arg == "-ntheta") && i + 1 < argc) {
      ntheta = atoi(argv[++i]);
    } else if ((arg == "-soccar") && i + 1 < argc) {
      mode = std::string("soccar");
    } else if ((arg == "-hoops") && i + 1 < argc) {
      mode = std::string("hoops");
    } else if ((arg == "-dropshot") && i + 1 < argc) {
      mode = std::string("dropshot");
    } else if ((arg == "-throwback") && i + 1 < argc) {
      mode = std::string("throwback");
    } else {
      std::cerr << "Bad option: '" << arg << "'" << std::endl;
    }
  }

  if (infile_prefix) {
    std::cout << "setting gamemode: " << mode << std::endl;
    Game::set_mode(mode);
    find_normals(infile_name, outfile_prefix);
  } else {
    std::cout << "No input files specified. Exiting..." << mode << std::endl;
  }
 

  std::cout << "find_normals()" << std::endl;
  find_normals();

  std::cout << "find_neighbors()" << std::endl;
  find_neighbors();

  std::cout << "compute_edges()" << std::endl;
  compute_edges();

//  std::cout << "sssp()" << std::endl;
//  sssp();

  //sssp_plane();
#endif
  
  return 0;

}
