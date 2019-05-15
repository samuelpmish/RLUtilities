#include "misc/graph.h"
#include "misc/timer.h"

#include <queue>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>

bool edge_comparator(Graph::edge i, Graph::edge j) {
  return (i.src < j.src) || ((i.src == j.src) && (i.dst < j.dst));
}

Graph::Graph() {
  num_edges = 0;
  num_vertices = 0;

  offsets = std::vector < int >(0);
  destinations = std::vector < int >(0);
  weights = std::vector < float >(0);
}

Graph::Graph(const std::vector < edge > & edges) {

  auto sorted_edges = edges;

  std::sort(sorted_edges.begin(), sorted_edges.end(), edge_comparator);

  num_edges = int(sorted_edges.size());
  num_vertices = sorted_edges.back().src + 1;

  offsets = std::vector< int >(num_vertices + 1, 0); 

  destinations = std::vector< int >(num_edges);
  weights = std::vector< float >(num_edges);

  // compress edge representation and calculate the outdegree of each vertex
  for (int e = 0; e < num_edges; e++) {
    offsets[sorted_edges[e].src+1]++;
    destinations[e] = sorted_edges[e].dst;
    weights[e] = sorted_edges[e].weight;
  } 

  // prefix sum the outdegrees to get the offsets
  for (int i = 0; i < num_vertices; i++) {
    offsets[i+1] += offsets[i];
  }
}

std::vector < int > Graph::bellman_ford_sssp(int start, float maximum_weight) const {

  std::vector < int > best_parents(num_vertices, -1);
  std::vector < float > best_weights(num_vertices, maximum_weight);

  best_parents[start] = start;
  best_weights[start] = 0.0f;

  std::vector < int > frontier = {start};
  frontier.reserve(num_vertices);

  for (int iter = 0; iter < 128 && frontier.size() > 0; iter++) {
    bellman_ford_iteration(frontier, best_parents, best_weights);  
  }

  return best_parents;

}

bool Graph::bellman_ford_iteration(std::vector < int > & frontier, 
    std::vector < int > & best_parents, 
    std::vector < float > & best_weights) const {

  typedef uint64_t word_t;
  constexpr int nbits = sizeof(word_t) * 8;

  constexpr word_t one = 1;

  std::vector < uint64_t > visited((num_vertices + nbits - 1) / nbits, 0);

  for (const int source : frontier) {

    uint32_t begin = offsets[source];
    uint32_t end = offsets[source+1];

    for (uint32_t j = begin; j < end; j++) {

      int destination = destinations[j];

      float new_weight = best_weights[source] + weights[j];
      float old_weight = best_weights[destination];

      if (new_weight < old_weight) {
        best_parents[destination] = source;
        best_weights[destination] = new_weight;
        visited[destination / nbits] |= (one << (destination % nbits));
      }

    }

  }

  frontier.clear();
  for (int i = 0; i < visited.size(); i++) {
    word_t word = visited[i];
    if (word != 0) {
      for (int j = 0; j < nbits; j++) {
        if (word & (one << j)) {
          frontier.push_back(i * nbits + j);
        }
      }
    }
  }

  //std::cout << frontier.size() << std::endl;

  return (frontier.size() == 0);

}
