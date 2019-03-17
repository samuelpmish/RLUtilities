#include "simulation/bvh.h"
#include "simulation/game.h"
#include "simulation/field.h"
#include "simulation/geometry.h"

#include "mechanics/drive.h"

#include "linear_algebra/math.h"

#include "misc/timer.h"

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

std::string path("C:\\Users\\sam\\Desktop\\LUT\\");

const uint32_t time_bits = 10;
const uint32_t time_mask = (uint32_t(1) << time_bits) - 1;
const float time_scale = 6.0f / time_mask;

struct edge_info {
  uint32_t id;
  float time;
};

// fuck visual C++
const float twopi = 6.28318530f;

const int ntheta = 16;
const int ndivisions = 32;

timer stopwatch;
std::vector < vec3 > navigation_nodes;
std::vector < vec3 > navigation_normals;
std::vector < std::vector < int > > neighbors;

void voxel_intersection() {

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
  std::cout << count << " voxel intersections: " << stopwatch.elapsed() << std::endl;

}

void find_normals() {

  std::ifstream infile("navmesh_pruned.txt");
  std::ofstream outfile("navmesh_pruned_with_normals.txt");

  stopwatch.start();

  vec3 p, n;
  std::string line;
  while (std::getline(infile, line)) {

    std::istringstream istream(line);

    istream >> p[0];
    istream >> p[1];
    istream >> p[2];

    n = Field::collide(sphere{p, 30.0f}).direction;

    outfile << p[0] << ' ' << p[1] << ' ' << p[2] << ' ';
    outfile << n[0] << ' ' << n[1] << ' ' << n[2] << '\n';

    navigation_nodes.push_back(p);
    navigation_normals.push_back(n);

  }

  outfile.close();
  stopwatch.stop();
  std::cout << stopwatch.elapsed() << std::endl;

}

void find_neighbors() {

  stopwatch.start();
  float close_enough = 500.0f;

  size_t nnodes = navigation_nodes.size();

  stopwatch.start();

  std::vector < sphere > navigation_spheres(nnodes);

  for (int i = 0; i < nnodes; i++) {
    navigation_spheres[i] = sphere{navigation_nodes[i], 0.001f};
  }

  bvh < sphere > navnode_bvh(navigation_spheres);

  std::vector < vec3 > nodes_copy(navigation_nodes);
  std::vector < vec3 > normals_copy(navigation_normals);

  std::ofstream outfile(path + "navmesh_points_and_normals_morton.txt");
  for (int i = 0; i < nnodes; i++) {
    auto id = navnode_bvh.code_ids[i] & navnode_bvh.mask;
     
    vec3 p = nodes_copy[id];
    vec3 n = normals_copy[id];

    navigation_nodes[i] = p;
    navigation_normals[i] = n;

    outfile << p[0] << ' ' << p[1] << ' ' << p[2] << ' ';
    outfile << n[0] << ' ' << n[1] << ' ' << n[2] << '\n';
  };
  outfile.close();


  for (int i = 0; i < nnodes; i++) {
    navigation_spheres[i] = sphere{navigation_nodes[i], 0.001f};
  }

  navnode_bvh = bvh < sphere >(navigation_spheres);

  for (int i = 0; i < nnodes; i++) {

    auto hits = navnode_bvh.intersect(sphere{navigation_nodes[i], close_enough});

    std::sort(hits.begin(), hits.end());

    hits.erase(std::remove(hits.begin(), hits.end(), i), hits.end());
    
    neighbors.push_back(hits);

  }

  stopwatch.stop();
  std::cout << "find neighbors (bvh): " << stopwatch.elapsed() << std::endl;

}

void compute_edges() {

  stopwatch.start();

  std::vector < float > c(ntheta);
  std::vector < float > s(ntheta);
  for (int i = 0; i < ntheta; i++) {
    c[i] = cos(i * twopi / ntheta);
    s[i] = sin(i * twopi / ntheta);
  }

  int total = 0;
  size_t nnodes = navigation_nodes.size();

  std::ofstream edgefile(path + std::string("edges_morton.bin"), std::ios::binary);
  std::ofstream offsetfile(path + std::string("offsets_morton.bin"), std::ios::binary);

  offsetfile.write((char*)&total, sizeof(uint32_t));

  for (int i = 0; i < nnodes; i++) {

    vec3 pi = navigation_nodes[i];
    vec3 ni = navigation_normals[i];
    mat3 iBasis = R3_basis(ni);

    for (int itheta = 0; itheta < ntheta; itheta++) {
      
      vec3 ti = dot(iBasis, vec3{c[itheta], s[itheta], 0.0f}); 

      for (auto j : neighbors[i]) {

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
          if (delta_theta > 1.5f) continue;

          float offset = ((1.0f / 3.0f) + (delta_theta * delta_theta) / 18.0f) * norm(delta_p);

          float arc_length = 0.0f; 
          float max_curvature = 0.0f;
          float avg_curvature = 0.0f;

          // sample the curve to determine its length and curvature profile
          for (int k = 0; k < ndivisions; k++) { 
            float u = (float(k) + 0.5f) / float(ndivisions); 
            float v = 1.0f - u;

            vec3 dg = (-6.0f * u * v) * pi +
                      (-6.0f * u * v + 3.0f * v * v) * offset * ti +
                      (-6.0f * u * v + 3.0f * u * u) * offset * tj +
                      ( 6.0f * u * v) * pj;

            vec3 d2g = (6.0f * (u - v)) * pi +
                       (6.0f * (u - 2.0f * v)) * offset * ti +
                       (6.0f * (2.0f * u - v)) * offset * tj +
                       (6.0f * (v - u)) * pj;

            float curvature = dot(cross(dg, d2g), normalize(v * ni + u * nj)) / 
                                    (norm(dg) * norm(dg) * norm(dg));

            arc_length += norm(dg);
            avg_curvature += curvature;
            max_curvature = fmaxf(max_curvature, fabs(curvature));
          }

          arc_length /= ndivisions;
          avg_curvature /= ndivisions;

          if (max_curvature < 0.00375f) {
            total++;            
            float time = arc_length / Drive::max_turning_speed(max_curvature);
            edgefile.write((char*)&edge_info{uint32_t(j * ntheta + jtheta), time}, sizeof(edge_info));
          }

        }

      }

      offsetfile.write((char*)&total, sizeof(uint32_t));

    }

    if (i % 1000 == 0) {
      std::cout << i << std::endl;
    }

  }
  edgefile.close();
  offsetfile.close();
  stopwatch.stop();
  std::cout << "computed " << total << " edges in " << stopwatch.elapsed() << "s" << std::endl;

}

void sssp() {

  std::ifstream infile(path + std::string("edges_morton.bin"), std::ios::binary);

  size_t n;

  std::vector< edge_info > edges;
  std::vector< uint32_t > offsets;

  if (infile) {

    stopwatch.start();

    infile.seekg(0,std::ios::end);
    std::streampos filesize = infile.tellg();
    infile.seekg(0,std::ios::beg);

    size_t num_edges = filesize / sizeof(edge_info);
    edges.reserve(num_edges);
    infile.read((char*)&edges[0], filesize);

    stopwatch.stop();
    std::cout << "read " << num_edges << " edges from file in " << stopwatch.elapsed() << "s" << std::endl;

  } else {

    std::cout << "file not found: " << path + std::string("edges_morton.bin") << std::endl;

  }

  infile.close();

  infile.open(path + std::string("offsets_morton.bin"), std::ios::binary);

  if (infile) {

    stopwatch.start();

    infile.seekg(0,std::ios::end);
    std::streampos filesize = infile.tellg();
    infile.seekg(0,std::ios::beg);

    n = filesize / sizeof(uint32_t);
    offsets.reserve(n + 1);
    infile.read((char*)&offsets[0], filesize);

    stopwatch.stop();
    std::cout << "read " << n << " nodes from file in " << stopwatch.elapsed() << "s" << std::endl;

  } else {

    std::cout << "file not found: " << path + std::string("offsets_morton.bin") << std::endl;

  }

  stopwatch.start();
  uint32_t which = 5884;
  int total = 1;

  std::vector < char > considered(n, 0);
  std::vector < edge_info > best(n, edge_info{0, time_mask});
  best[which] = edge_info{which, 0};

  std::vector < uint32_t > active;
  active.push_back(which);

  std::vector < uint32_t > next;
  next.reserve(1<<20);

  for (int iter = 0; iter < 26; iter++) {

    next.clear();

    for (const uint32_t source : active) {

      uint32_t begin = offsets[source];
      uint32_t end = offsets[source+1];

      float this_time = best[source].time;

      for (uint32_t j = begin; j < end; j++) {

        edge_info e = edges[j];

        uint32_t destination = e.id;
        float delta_time = e.time;
        
        float that_time = best[destination].time;

        if (this_time + delta_time < that_time) {
          best[destination] = edge_info{source, this_time + delta_time};

          if (!considered[destination]) {
            next.push_back(destination);
            considered[destination] = 1;
          }

        }

      }

      total += end-begin;

    }

    active = next;

    considered = std::vector < char >(n, 0);

  }

  stopwatch.stop();
  std::cout << total << " edges traversed in " << stopwatch.elapsed() << "s" << std::endl;

  std::ofstream outfile(path + std::string("results.txt"));

  for (int i = 0; i < best.size(); i++) {
    outfile << best[i].id << " " << best[i].time << '\n';
  }

  outfile.close();

}

struct transition {
  int16_t x, y, theta, v;
  float t;
};

const float max_time = 10.0f;
const uint16_t max_int = std::numeric_limits< uint16_t >::max(); 

void sssp_plane() {

  float scale = 64.0f;

  int nx = 16;
  int gx = 48;
  int nv = 23;

  auto neighbors = std::vector < std::vector < std::vector < transition > > >(nv,
                                 std::vector < std::vector < transition > >  (ntheta));

  std::vector < float > c(ntheta);
  std::vector < float > s(ntheta);
  for (int i = 0; i < ntheta; i++) {
    c[i] = cos(i * twopi / ntheta);
    s[i] = sin(i * twopi / ntheta);
  }

  std::ofstream neighborfile(path + std::string("neighbors.txt"));

  for (int16_t vi = 0; vi < nv; vi++) {

    float v1 = (vi + 1) * 100.0f;
    
    for (int16_t theta1 = 0; theta1 < ntheta; theta1++) {

      vec2 t1 = vec2{c[theta1], s[theta1]};

      for (int16_t x = -nx; x <= nx; x++) {
        for (int16_t y = -nx; y <= nx; y++) {

          if ((x == 0 && y == 0) || (x*x + y*y > nx * nx)) continue;

          vec2 p = vec2{x * scale, y * scale};

          // discard points that are behind me
          if (dot(p, t1) < 0.0f) continue;

          for (int16_t theta2 = 0; theta2 < ntheta; theta2++) {

            vec2 t2 = vec2{c[theta2], s[theta2]};

            float delta_theta = acos(clip(dot(t1, t2), -1.0f, 1.0f));

            // discard curves that have require too much turning
            if (delta_theta > 1.5f) continue;

            float offset = ((1.0f / 3.0f) + (delta_theta * delta_theta) / 18.0f) * norm(p);

            float arc_length = 0.0f; 
            float max_curvature = 0.0f;
            float avg_curvature = 0.0f;
            float dev_curvature = 0.0f;
            float mean_curvature = 0.0f;

            // sample the curve to determine its length and curvature profile
            for (int k = 0; k < ndivisions; k++) { 
              float u = (float(k) + 0.5f) / float(ndivisions); 
              float v = 1.0f - u;

              vec2 dg = (-6.0f * u * v + 3.0f * v * v) * offset * t1 +
                        (-6.0f * u * v + 3.0f * u * u) * offset * t2 +
                        ( 6.0f * u * v) * p;

              vec2 d2g = (6.0f * (u - 2.0f * v)) * offset * t1 +
                         (6.0f * (2.0f * u - v)) * offset * t2 +
                         (6.0f * (v - u)) * p;

              float curvature = det(dg, d2g) / (norm(dg) * norm(dg) * norm(dg));

              arc_length += norm(dg);
              avg_curvature += fabs(curvature);
              max_curvature = fmaxf(max_curvature, fabs(curvature));
              mean_curvature += curvature;
              dev_curvature += curvature * curvature;
            }

            arc_length /= ndivisions;
            avg_curvature /= ndivisions;

            mean_curvature /= ndivisions;
            dev_curvature /= ndivisions;

            dev_curvature = sqrt(dev_curvature - mean_curvature * mean_curvature);

            for (int16_t vj = 0; vj < nv; vj++) {

              float v2 = (vj + 1) * 100.0f;
              float v_avg = (0.5f * (v1 + v2));

              float a = 0.5f * (v2*v2 - v1*v1) / arc_length;
              float a_min = -Drive::brake_accel;
              float a_max = Drive::throttle_accel(v_avg) + Drive::boost_accel;
              float allowable_curvature = Drive::max_turning_curvature(v_avg);
              float kappa = 0.5f * (max_curvature + avg_curvature);

              if (a_min <= a && a <= a_max && 
                  kappa < allowable_curvature && 
                  dev_curvature < 0.00075f) {
                float time = (arc_length / v_avg) + 0.05f;
                neighbors[vi][theta1].push_back(transition{x, y, theta2, vj, time});
                neighborfile << int(vi) << ' ' << int(theta1) << ' ' << int(x);
                neighborfile << ' ' << int(y) << ' ' << int(theta2) << ' ' << int(vj) << ' ' << time << ' ' << arc_length << std::endl;
              }

            }

          }

        }
      }

    }
  }

  neighborfile.close();

  int strides[] = {
    nv * ntheta * (2 * gx + 1),
    nv * ntheta,
    nv,
    1
  };

  auto to_id = [=](transition t) {
    return (t.x + gx) * strides[0] + 
           (t.y + gx) * strides[1] + 
              t.theta * strides[2] + 
                  t.v * strides[3];
  };

  auto from_id = [=](uint32_t id) {
    int16_t x     = int16_t((id / strides[0]) - gx); id %= strides[0];
    int16_t y     = int16_t((id / strides[1]) - gx); id %= strides[1];
    int16_t theta = int16_t(id / strides[2]); id %= strides[2];
    int16_t v     = int16_t(id / strides[3]);

    return transition{x, y, theta, v, 0.0f};
  };

  uint32_t num_nodes = (2 * gx + 1) * (2 * gx + 1) * ntheta * nv;

  for (int16_t x = -gx; x <= gx; x++) {
    for (int16_t y = -gx; y <= gx; y++) {
      for (int8_t theta = 0; theta < ntheta; theta++) {
        for (int8_t v = 0; v < nv; v++) {

          uint32_t id = to_id(transition{x, y, theta, v, 0});

          if (0 <= id && id < num_nodes) {
            transition converted = from_id(id);

            if (converted.x != x || 
                converted.y != y || 
                converted.theta != theta ||
                converted.v != v) {
              
              std::cout << "invalid conversion:" << std::endl;
              std::cout << "(" << x << ' ' << y << ' ' << theta << ' ' << v << ") -> " << id;
              std::cout << " -> (" << converted.x << ' ' << converted.y << ' ' << converted.theta << ' ' << converted.v << ")" << std::endl;

            }

          } else {
            std::cout << "id out of range:";
            std::cout << "(" << x << ' ' << y << ' ' << theta << ' ' << v << ") -> " << id;
          }

        }
      }
    }
  }

  std::ofstream pathfile(path + std::string("paths_planar.bin"), std::ios::binary);
  std::ofstream timefile(path + std::string("times_planar.bin"), std::ios::binary);

  std::ofstream paramfile(path + std::string("parameters.txt"));
  paramfile << scale << ' ' << gx << ' ' << ntheta << ' ' << nv << std::endl;

  //for (int16_t v = 0; v < nv; v++) {
  for (int16_t v = 14; v < 15; v++) {

    std::cout << "starting v = " << v << std::endl;

    transition start{0, 0, 0, v, 0.0f};
    std::vector < transition > best(num_nodes, transition{0, 0, 0, 69, 10.0f});
    best[to_id(start)] = start;

    std::unordered_set < uint32_t > active;
    active.insert(to_id(start));

    std::unordered_set < uint32_t > next;
    next.reserve(1 << 24);

    for (int iter = 0; iter < 64; iter++) {

      next.clear();

      for (const uint32_t source : active) {

        auto s = from_id(source);
        s.t = best[source].t;

        for (const transition & other : neighbors[s.v][s.theta]) {

          transition target{s.x + other.x, s.y + other.y, other.theta, other.v, s.t + other.t}; 

          if ((abs(target.x) <= gx) && (abs(target.y) <= gx)) {

            if (target.t < best[to_id(target)].t) {
              best[to_id(target)] = transition{s.x, s.y, s.theta, s.v, target.t};
              next.insert(to_id(target));
            }

          }

        }

      }

      active = next;
      std::cout << iter << " " << active.size() << std::endl;

    }

    stopwatch.stop();
    std::cout << "total time: " << stopwatch.elapsed() << "s" << std::endl;

    for (uint32_t i = 0; i < num_nodes; i++) {
      uint32_t id = to_id(best[i]);
      pathfile.write((char*)&id, sizeof(uint32_t));
      timefile.write((char*)&best[i].t, sizeof(float));
    }

    paramfile << v << '\n';

  }

  pathfile.close();
  timefile.close();
  paramfile.close();

}

int main() {

  stopwatch.start();
  Game::set_mode(std::string("soccar"));
  stopwatch.stop();
  std::cout << "setting gamemode: " << stopwatch.elapsed() << "s" << std::endl;

  //voxel_intersection();
  
  //find_normals();

  //find_neighbors();

  //compute_edges();

  //sssp();

  //sssp_plane();
  
  return 0;

}
