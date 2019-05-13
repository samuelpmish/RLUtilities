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

// fuck visual C++
const float twopi = 6.28318530f;

struct transition {
  int16_t x, y, theta, v;
  float t;
};

void sssp_plane() {

  timer stopwatch;

  stopwatch.start();

  // spacing between gridpoints
  float scale = 64.0f;

  // the size of the stencil to be applied to the grid
  int nx = 16;

  // the half width of the grid
  int gx = 96;

  // number of velocity subdivisions (not to be changed for now)
  int nv = 23;

  // number of angular subdivisions
  int ntheta = 16;

  auto neighbors = std::vector < std::vector < std::vector < transition > > >(nv,
                                 std::vector < std::vector < transition > >  (ntheta));

  vec3 z = {0.0f, 0.0f, 1.0f};
  
  // precompute sines and cosines
  std::vector < float > c(ntheta);
  std::vector < float > s(ntheta);
  for (int i = 0; i < ntheta; i++) {
    c[i] = cos(i * twopi / ntheta);
    s[i] = sin(i * twopi / ntheta);
  }

  std::ofstream neighborfile(std::string("neighbors.txt"));

  for (int16_t vi = 0; vi < nv; vi++) {

    float v1 = (vi + 1) * 100.0f;
    
    for (int16_t theta1 = 0; theta1 < ntheta; theta1++) {

      vec3 t1 = vec2{c[theta1], s[theta1], 0.0f};

      for (int16_t x = -nx; x <= nx; x++) {
        for (int16_t y = -nx; y <= nx; y++) {

          if ((x == 0 && y == 0) || (x*x + y*y > nx * nx)) continue;

          vec3 p = vec2{x * scale, y * scale, 0.0f};

          // discard points that are behind me
          if (dot(p, t1) < 0.0f) continue;

          for (int16_t theta2 = 0; theta2 < ntheta; theta2++) {

            vec3 t2 = vec2{c[theta2], s[theta2], 0.0f};

            float delta_theta = acos(clip(dot(t1, t2), -1.0f, 1.0f));

            // discard curves that have require too much turning
            if (delta_theta > 1.5f) continue;

            Curve segment({
              ControlPoint{vec3{0.0f, 0.0f, 0.0f}, t1, z},
              ControlPoint{p, t2, z}
            });

            //float mean_curvature = mean(segment.curvatures);
            float dev_curvature = standard_deviation(segment.curvatures);

            if (dev_curvature > 0.00075f) continue;

            for (int16_t vj = 0; vj < nv; vj++) {

              float v2 = (vj + 1) * 100.0f;

              float time = segment.calculate_max_speeds(v1, v2) + 0.05f;

              if (fabs(segment.max_speeds.front() - v1) > 50.0f || 
                  fabs(segment.max_speeds.back() - v2) > 50.0f) {
                continue;
              }

              neighbors[vi][theta1].push_back(transition{x, y, theta2, vj, time});
              neighborfile << int(vi) << ' ' << int(theta1) << ' ' << int(x);
              neighborfile << ' ' << int(y) << ' ' << int(theta2) << ' ' << int(vj) << ' ' << time << std::endl;

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

  std::ofstream pathfile(std::string("paths_planar.bin"), std::ios::binary);
  std::ofstream timefile(std::string("times_planar.bin"), std::ios::binary);

  std::ofstream paramfile(std::string("parameters.txt"));
  paramfile << scale << ' ' << gx << ' ' << ntheta << ' ' << nv << std::endl;

  //for (int16_t v = 0; v < nv; v++) 
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

          transition target{int16_t(s.x + other.x), int16_t(s.y + other.y), 
            other.theta, other.v, s.t + other.t}; 

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

  sssp_plane();
  
  return 0;

}
