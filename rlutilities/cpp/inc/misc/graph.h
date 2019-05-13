#pragma once

#include <vector>
#include <string>
#include <set>

class Graph {

 public:

  struct edge {
    int src, dst;
    float weight;
  };

  Graph();
  Graph(const std::vector < edge > &);

  int num_edges;
  int num_vertices;

  std::vector < int > offsets;
  std::vector < int > destinations;
  std::vector < float > weights;

  std::vector < int > bellman_ford_sssp(int start, float maximum_weight) const;

  bool bellman_ford_iteration(std::vector < int > & frontier, 
      std::vector < int > & best_parents, 
      std::vector < float > & best_weights) const;

};
