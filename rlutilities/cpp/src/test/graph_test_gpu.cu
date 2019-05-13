#include "misc/io.h"
#include "misc/timer.h"
#include "misc/graph.h"

#include "linear_algebra/math.h"

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

std::string path("C:\\Users\\sam\\Desktop\\LUT\\");
std::string filename("C:\\Users\\sam\\Desktop\\LUT\\tuples.bin");

int main() {

  timer stopwatch;

  auto edges = read_binary< Graph::edge >(filename);
  auto points = read_binary< vec3 >(path + std::string("navigation_nodes.bin"));

  Graph g(edges);

  std::cout << std::endl;
  std::cout << g.num_vertices << " " << g.num_edges << std::endl;

  auto parents_dijk = g.dijkstra_sssp(72);
  auto parents_bf = g.bellman_ford_sssp(72);

  int errors = 0;
  for (int i = 0; i < g.num_vertices; i++) {
    if (parents_dijk[i] != parents_bf[i]) {
      errors++;
    }
  }

  std::cout << "found " << errors << " mismatches between dijkstra_sssp() and bellman_ford_sssp()" << std::endl;

  write_binary(parents_dijk, "C:\\Users\\sam\\Desktop\\LUT\\parents_dijk.bin");
  write_binary(parents_bf, "C:\\Users\\sam\\Desktop\\LUT\\parents_bf.bin");

  std::vector< bool > mask(g.num_vertices, false);

  float r = 4000.0f;
  vec3 p{0.0f, 0.0f, 0.0f};

  stopwatch.start();
  int count = 0;
  int closest = -1;
  float min_distance = 100000.0f;
  for (int i = 0; i < points.size(); i++) {
    float distance = norm(p - points[i]);
    if (distance < r) {
      for (int j = 0; j < 16; j++) {
        count++;
        mask[i * 16 + j] = true;
      }
    }

    if (distance < min_distance) {
      closest = i;
      min_distance = distance;
    }
  }
  stopwatch.stop();

  std::cout << count << " " << closest << " " << stopwatch.elapsed() << std::endl;

  stopwatch.start();
  parents_bf = g.bellman_ford_sssp(closest * 16 + 6);
  stopwatch.stop();
  std::cout << "bellman_ford_sssp() time: " << stopwatch.elapsed() << std::endl;

  return 0;

}

