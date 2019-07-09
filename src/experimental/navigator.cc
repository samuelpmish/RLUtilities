#include "experimental/navigator.h"

#include <math.h>
#include <string>
#include <fstream>
#include <algorithm>

#include "mechanics/Drive.h"

#include "misc/io.h"
#include "misc/timer.h"

//float Navigator::scale = -1.0f;
//int Navigator::nx = -1;
int Navigator::ntheta = 16;
//int Navigator::nv = -1;

//std::array < int, 3 > Navigator::strides;
//std::vector < float > Navigator::LUT_times;
//std::vector < uint32_t > Navigator::LUT_paths;

Graph Navigator::navigation_graph;
std::vector < vec3 > Navigator::navigation_nodes;
std::vector < vec3 > Navigator::navigation_tangents;
std::vector < vec3 > Navigator::navigation_normals;
std::vector < vec3 > Navigator::directions;

void Navigator::init_statics(
    std::vector< Graph::edge > nav_edges,
    std::vector< vec3 > nav_nodes,
    std::vector< vec3 > nav_normals) {

  //scale = float(parameters[0]);
  //nx = parameters[1];
  //ntheta = parameters[2];
  //nv = parameters[3];
  //
  //strides = {nv * ntheta * (2 * nx + 1), nv * ntheta, nv};

  navigation_graph = Graph(nav_edges);

  navigation_nodes = nav_nodes;
  navigation_normals = nav_normals;

  directions = std::vector < vec3 >(ntheta);
  navigation_tangents = std::vector < vec3 >(navigation_normals.size() * ntheta);

  const float k = ntheta / 6.28318530f;
  for (int i = 0; i < ntheta; i++) {
    directions[i] = vec3{cos(i / k), sin(i / k), 0.0f};
  }

  for (int i = 0; i < navigation_normals.size(); i++) {
    mat3 basis = R3_basis(navigation_normals[i]);
    for (int j = 0; j < ntheta; j++) {
      navigation_tangents[i * ntheta + j] = dot(basis, directions[j]);
    }
  };

}

Navigator::Navigator(Car & c) : car(c) {

  debug = false;
  source_node = -1;
  source_direction = -1;

  int n = navigation_graph.num_vertices;
  navigation_frontier = std::vector < int >(n);
  navigation_paths = std::vector < int >(n);
  navigation_times = std::vector < float >(n);

}

void Navigator::analyze_surroundings(float time_budget) {

  timer stopwatch;

  source = car.position;
  size_t nnodes = navigation_nodes.size();

  stopwatch.start();
  source_node = -1;
  float minimum = 1000000.0f;
  for (int i = 0; i < nnodes; i++) {
    float distance = norm(source - navigation_nodes[i]);
    if (distance < minimum) {
      source_node = i;
      minimum = distance;
    }
  }

  vec3 p = navigation_nodes[source_node];
  vec3 n = navigation_normals[source_node];
  source -= dot(source - p, n) * n;

  std::cout << "source: " << source << std::endl;

  vec3 f = car.forward();
  source_direction = -1;
  float maximum_alignment = -2.0f;
  for (int j = 0; j < ntheta; j++) {
    float alignment = dot(f, navigation_tangents[source_node * ntheta + j]);
    if (alignment > maximum_alignment) {
      source_direction = j;
      maximum_alignment = alignment;
    }
  }

  int source_id = source_node * ntheta + source_direction;
  stopwatch.stop();

  if (debug) {
    std::cout << "car close to: " << navigation_nodes[source_node];
    std::cout << " facing " << navigation_tangents[source_id] << std::endl;
    std::cout << "source_node: " << source_node;
    std::cout << " source_direction: " << source_direction;
    std::cout << " source_id: " << source_id << std::endl;
  }

  stopwatch.start();
  navigation_paths = navigation_graph.bellman_ford_sssp(source_id, 15.0f);
  stopwatch.stop();

  if (debug) {
    std::cout << "sssp time: " << stopwatch.elapsed() * 1000 << "ms" << std::endl;
  }
}

Curve Navigator::path_to(vec3 destination, vec3 tangent, float offset) {

  vec3 unit_tangent = normalize(tangent);

  int destination_node = -1;
  float minimum = 1000000.0f;
  for (int i = 0; i < navigation_nodes.size(); i++) {
    float distance = norm(destination - offset * unit_tangent - navigation_nodes[i]);
    if (distance < minimum) {
      destination_node = i;
      minimum = distance;
    }
  }

  int destination_direction = -1;
  float maximum_alignment = -2.0f;
  for (int j = 0; j < ntheta; j++) {
    float alignment = dot(unit_tangent, navigation_tangents[destination_node * ntheta + j]);
    if (alignment > maximum_alignment) {
      destination_direction = j;
      maximum_alignment = alignment;
    }
  }

  int source_id = source_node * ntheta + source_direction;
  int dest_id = destination_node * ntheta + destination_direction;

  if (debug) {
    std::cout << "dest close to: " << navigation_nodes[destination_node];
    std::cout << " facing " << navigation_tangents[dest_id] << std::endl;
    std::cout << "destination_node: " << destination_node;
    std::cout << " destination_direction: " << destination_direction;
    std::cout << " dest_id: " << dest_id << std::endl;
  }

  vec3 p = navigation_nodes[destination_node];
  vec3 t = navigation_tangents[dest_id]; 
  vec3 n = navigation_normals[dest_id / ntheta]; 

  destination -= dot(destination - p, n) * n;

  std::vector < ControlPoint > ctrl_pts = {ControlPoint{p, t, n}};

  for (int i = 0; i < 32; i++) {

    // find the navigation node and tangent that brings me here
    dest_id = navigation_paths[dest_id];

    // if it exists, add another control point to the path
    if (dest_id != -1) {

      p = navigation_nodes[dest_id / ntheta]; 
      t = navigation_tangents[dest_id]; 
      n = navigation_normals[dest_id / ntheta]; 
  
      ctrl_pts.push_back(ControlPoint{p, t, n});

      // if we reach the navigation node for the car, 
      // handle that case differently, and exit the loop
      if (dest_id == source_id) break;

    // otherwise, the path is unreachable
    } else {
      std::cout << "unreachable destination" << std::endl;
      return Curve();
    }

  }

  std::reverse(ctrl_pts.begin(), ctrl_pts.end());

  // modify the control points slightly to better fit 
  // the actual positions of the car and its destination
  vec3 dx1 = source + offset * car.forward() - ctrl_pts.front().p;
  vec3 dt1 = car.forward() - ctrl_pts.front().t;

  vec3 dx2 = destination - offset * unit_tangent - ctrl_pts.back().p;
  vec3 dt2 = unit_tangent - ctrl_pts.back().t;

  return Curve(ctrl_pts, dx1, dt1, dx2, dt2, source, destination);

}

//Curve Navigator::lut_path_to(vec3 destination, vec3 tangent, float offset) {
//
//  const float k = ntheta / 6.28318530f;
//  const vec3 n = vec3{0.0f, 0.0f, 1.0f};
//
//  vec3 unit_tangent = normalize(tangent);
//  mat3 orientation = look_at(xy(car.forward()), n);
//
//  vec3 destination_local = dot(destination - offset * unit_tangent - car.position, orientation);
//  vec3 tangent_local;
//
//  int x = clip(int(roundf(destination_local[0] / scale)), -nx, nx);
//  int y = clip(int(roundf(destination_local[1] / scale)), -nx, nx);
//
//  tangent_local = dot(unit_tangent, orientation);
//  float angle = atan2(tangent_local[1], tangent_local[0]);
//  int theta = int(roundf(k * angle));
//  if (theta < 0) theta += ntheta;
//
//  int v = -1;
//  int v_min = 0;
//  int v_max = nv-1;
//
//  // TODO 
//  // TODO 
//  float speed = 1400.0f;
//  // TODO 
//  // TODO 
//
//  if (isnormal(speed)) {
//    v_min = clip(int(floor(speed / 100.0f)), 0, nv-1);
//  }
//
//  float best_time = 1.0e10f;
//
//  const std::vector < float > & times = LUT_times;
//  const std::vector < uint32_t > & paths = LUT_paths;
//  for (int u = v_min; u <= v_max; u++) {
//    if (times[to_id(x, y, theta, u)] < best_time) {
//      best_time = times[to_id(x, y, theta, u)];
//      v = u;
//    }
//  }
//
//  std::vector < ControlPoint > ctrl_pts;
//
//  for (int i = 0; i < 32; i++) {
//
//    vec3 p = dot(orientation, vec3{x * scale, y * scale, 0.0f}) + car.position;
//    vec3 t = dot(orientation, directions[theta]);
//
//    if (i == 0) {
//      p = destination;
//      t = tangent;
//    }
//
//    ctrl_pts.push_back(ControlPoint{p, t, n});
//
//    std::tie(x, y, theta, v) = from_id(paths[to_id(x, y, theta, v)]);
//
//    if (x == 0 && y == 0 && theta == 0) break;
//
//  }
//  ctrl_pts.push_back(ControlPoint{car.position, car.forward(), n});
//
//  std::reverse(ctrl_pts.begin(), ctrl_pts.end());
//
//  ctrl_pts.push_back(ControlPoint{destination, unit_tangent, n});
//
//  return Curve(ctrl_pts);
//
//};
//
//uint32_t Navigator::to_id(int x, int y, int theta, int v) {
//  return (x + nx) * strides[0] + (y + nx) * strides[1] + theta * strides[2] + v;
//}
//
//std::tuple < int, int, int, int > Navigator::from_id(uint32_t id) {
//  int16_t x     = int16_t((id / strides[0]) - nx); id %= strides[0];
//  int16_t y     = int16_t((id / strides[1]) - nx); id %= strides[1];
//  int16_t theta = int16_t(id / strides[2]);        id %= strides[2];
//  int16_t v     = int16_t(id);
//  return std::make_tuple(x, y, theta, v);
//}
