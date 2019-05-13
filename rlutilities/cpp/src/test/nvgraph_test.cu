#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "misc/io.h"
#include "misc/timer.h"
#include "misc/graph.h"

#include "cuda.h"
#include "nvgraph.h"

std::string filename("C:\\Users\\sam\\Desktop\\LUT\\tuples.bin");

Graph g;

timer stopwatch;

void check(nvgraphStatus_t status) {
  if (status != NVGRAPH_STATUS_SUCCESS) {
    printf("ERROR : %d\n",status);
    exit(0);
  }
}

void nvgraph_spmv_test() {

  size_t vertex_numsets = 2;
  size_t edge_numsets = 1;
  float alpha = 1.0, beta = 1.0;
  void *alpha_p = (void *)&alpha, *beta_p = (void *)&beta;
  void** vertex_dim;
  cudaDataType_t edge_dimT = CUDA_R_32F;
  cudaDataType_t* vertex_dimT;
  // nvgraph variables
  nvgraphHandle_t handle;
  nvgraphGraphDescr_t graph;
  nvgraphCSRTopology32I_t CSR_input;
  // Init host data
  vertex_dim  = (void**)malloc(vertex_numsets*sizeof(void*));
  vertex_dimT = (cudaDataType_t*)malloc(vertex_numsets*sizeof(cudaDataType_t));
  CSR_input = (nvgraphCSRTopology32I_t) malloc(sizeof(struct nvgraphCSRTopology32I_st));
  
  std::vector < float > x_h(g.num_vertices);
  std::vector < float > y_h(g.num_vertices);
  vertex_dim[0] = (void*)(&x_h[0]); 
  vertex_dim[1] = (void*)(&y_h[0]);
  vertex_dimT[0] = CUDA_R_32F; 
  vertex_dimT[1] = CUDA_R_32F;

  check(nvgraphCreate(&handle));
  check(nvgraphCreateGraphDescr(handle, &graph));

  CSR_input->nvertices = g.num_vertices; 
  CSR_input->nedges = g.num_edges;
  CSR_input->source_offsets = &g.offsets[0];
  CSR_input->destination_indices = &g.destinations[0];
  // Set graph connectivity and properties (tranfers)
  check(nvgraphSetGraphStructure(handle, graph, (void*)CSR_input, NVGRAPH_CSR_32));
  check(nvgraphAllocateVertexData(handle, graph, vertex_numsets, vertex_dimT));
  for (int i = 0; i < vertex_numsets; ++i) {
     check(nvgraphSetVertexData(handle, graph, vertex_dim[i], i));
  }
  check(nvgraphAllocateEdgeData(handle, graph, edge_numsets, &edge_dimT));
  check(nvgraphSetEdgeData(handle, graph, (void*)(&g.weights[0]), 0));

  // Solve

  check(nvgraphSrSpmv(handle, graph, 0, alpha_p, 0, beta_p, 1, NVGRAPH_MIN_PLUS_SR));
  cudaDeviceSynchronize();

  stopwatch.start();
  check(nvgraphSrSpmv(handle, graph, 0, alpha_p, 0, beta_p, 1, NVGRAPH_MIN_PLUS_SR));
  cudaDeviceSynchronize();
  stopwatch.stop();

  std::cout << "SrSpmv time: " << stopwatch.elapsed() << std::endl;
 
  //Get result
  check(nvgraphGetVertexData(handle, graph, (void*)(&y_h[0]), 1));

  //Clean 
  check(nvgraphDestroyGraphDescr(handle, graph));
  check(nvgraphDestroy(handle));
  free(vertex_dim); 
  free(vertex_dimT); 
  free(CSR_input);

}

void nvgraph_sssp_test() {

  size_t vertex_numsets = 1;
  size_t edge_numsets = 1;

  // nvgraph variables
  nvgraphHandle_t handle;
  nvgraphGraphDescr_t graph;
  nvgraphCSCTopology32I_t CSC_input;
  cudaDataType_t edge_dimT = CUDA_R_32F;

  std::vector < float > sssp_h(g.num_vertices);

  // Init host data
  CSC_input = (nvgraphCSCTopology32I_t) malloc(sizeof(struct nvgraphCSCTopology32I_st));

  void * vertex_dim[] = {(void*)(&sssp_h[0])}; 
  cudaDataType_t vertex_dimT[] = {CUDA_R_32F};

  check(nvgraphCreate(&handle));
  check(nvgraphCreateGraphDescr (handle, &graph));
  CSC_input->nvertices = g.num_vertices; 
  CSC_input->nedges = g.num_edges;
  CSC_input->source_indices = &g.destinations[0];
  CSC_input->destination_offsets = &g.offsets[0];

  // Set graph connectivity and properties (tranfers)
  check(nvgraphSetGraphStructure(handle, graph, (void*)CSC_input, NVGRAPH_CSC_32));
  check(nvgraphAllocateVertexData(handle, graph, vertex_numsets, vertex_dimT));
  check(nvgraphAllocateEdgeData  (handle, graph, edge_numsets, &edge_dimT));
  check(nvgraphSetEdgeData(handle, graph, (void*)(&g.weights[0]), 0));

  // Solve
  int source_vert = 0;
  check(nvgraphSssp(handle, graph, 0,  &source_vert, 0));
  cudaDeviceSynchronize();

  stopwatch.start();
  check(nvgraphSssp(handle, graph, 0,  &source_vert, 0));
  cudaDeviceSynchronize();
  stopwatch.stop();

  std::cout << "sssp time: " << stopwatch.elapsed() << std::endl;

  // Get and print result
  check(nvgraphGetVertexData(handle, graph, (void*)(&sssp_h[0]), 0));

  //Clean
  free(CSC_input);
  check(nvgraphDestroyGraphDescr(handle, graph));
  check(nvgraphDestroy(handle));

}

int main() {

  auto edges = read_binary< Graph::edge >(filename);

  g = Graph(edges);

  nvgraph_spmv_test();
  nvgraph_sssp_test();

}
