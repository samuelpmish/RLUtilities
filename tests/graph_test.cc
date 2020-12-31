#include "misc/io.h"
#include "misc/timer.h"
#include "misc/graph.h"

#include "linear_algebra/math.h"

timer stopwatch;

int main(int argc, char** argv) {

  int source = 0;
  float max_time = 100.0f;
  std::string filename;

  // parse arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);

    if ((arg == "-source") && i + 1 < argc) {
      source = atoi(argv[++i]);
    } else if ((arg == "-max_time") && i + 1 < argc) {
      max_time = atof(argv[++i]);
    } else if ((arg == "-graph") && i + 1 < argc) {
      filename = std::string(argv[++i]);
    } else {
      std::cerr << "Bad option: '" << arg << "'" << std::endl;
    }
  }

  if (!filename.empty()) {
    auto edges = read_binary< Graph::edge >(filename);

    Graph g(edges);
  
    std::cout << std::endl;
    std::cout << g.num_vertices << " " << g.num_edges << std::endl;
  
    stopwatch.start();
    auto parents_bf = g.bellman_ford_sssp(source, max_time);
    stopwatch.stop();
    std::cout << "bellman_ford_sssp() time: " << stopwatch.elapsed() << std::endl;

    std::cout << "unreachables: " << std::count(parents_bf.begin(), parents_bf.end(), -1) << std::endl;
  
    write_binary(parents_bf, "C:\\Users\\sam\\Desktop\\LUT\\parents_bf.bin");
  
    std::vector < int > frontier = {source};
    frontier.reserve(g.num_vertices);
    std::vector < int > parents(g.num_vertices, -1);
    std::vector < float > weights(g.num_vertices, max_time);
  
    weights[source] = 0.0f;
    parents[source] = source;
  
    stopwatch.start();
    for (int i = 0; i < 100; i++) {
      stopwatch.start();
      bool finished = g.bellman_ford_iteration(frontier, parents, weights);
      stopwatch.stop();
      std::cout << "bellman_ford_iteration() time: " << stopwatch.elapsed() << std::endl;
      if (finished) break;
    }
  } else {
    std::cout << "No input file specified. Exiting..." << std::endl;
  }

  return 0;

}
