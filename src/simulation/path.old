#include "simulation/path.h"

#include "mechanics/drive.h"

bool Path::initialized = false;
std::vector < std::vector < Path::edge > > Path::neighbors;
std::vector < std::vector < std::vector < float > > > Path::times;
std::vector < std::vector < std::vector < Path::node > > > Path::parents;

Path::Path() {

  segments = std::vector < segment >();

  if (!initialized) {
    init(64, 8, 32, 16);
  }

}

void Path::init(int gx, int nx, int ntheta, int nv) {

  const uint32_t DRIVE = 0;
  const uint32_t SLIDE = 1;
  const uint32_t DODGE = 2;

  const float rmin = 250.0f;
  const float vmax = Drive::max_speed;

  const float PI = 3.14159265358979323846f;
  const float k = 2.0f * PI / ntheta;
  const float scale = 32.0f;

  neighbors = std::vector < std::vector < edge > >();

  for (int16_t i = 0; i < ntheta; i++) {
    mat2 R = rotation(k * i);

    std::vector < edge > edges;
    for (int16_t x = -nx; x <= nx; x++) {
      for (int16_t y = -nx; y <= nx; y++) {
        vec2 p = dot(vec2{float(x), float(y)} * scale, R);

        // only consider points in front of me, and skip the center node
        if ((x != 0 || y != 0) && fabs(atan2(p[1], p[0])) < 1.5f) {

          // if the point is DIRECTLY in front of me
          if (fabs(normalize(p)[1]) < 0.05f) {
          
            float dt = norm(p) / vmax;

            edges.push_back(edge{dt, DRIVE, x, y, 0, i});
  
          } else {

            float r = 0.5f * dot(p, p) / p[1];

            if (fabs(r) > rmin) {

              float phi = 2.0f * sgn(p[0]) * asin(fabs(normalize(p)[1]));
              if (phi < 0.0f) phi += 2.0f * PI;

              int16_t j = (i + int16_t(round(sgn(r) * phi / k))) % ntheta;
              if (j < 0) j += ntheta;

              float dt = (fabs(r) * phi) / Drive::max_turning_speed(1.0f / fabs(r));

              edges.push_back(edge{dt, DRIVE, x, y, 0, j});

            }
  
          }
        
        }

      }
    }

    neighbors.push_back(edges);

  }

  std::vector < node > active;

  //times = std::vector < std::vector < std::vector < float > > >(ntheta,
  //                      std::vector < std::vector < float > >  (2*gx+1, 
  //                                    std::vector < float >    (2*gx+1, 100.0f)));

  //parents = std::vector < std::vector < std::vector < node > > >(ntheta,
  //                        std::vector < std::vector < node > >  (2*gx+1, 
  //                                      std::vector < node >    (2*gx+1)));

  times = std::vector < std::vector < std::vector < float > > >(2*gx+1, 
                        std::vector < std::vector < float > >  (2*gx+1, 
                                      std::vector < float >    (ntheta, 100.0f)));

  parents = std::vector < std::vector < std::vector < node > > >(2*gx+1,
                          std::vector < std::vector < node > >  (2*gx+1, 
                                        std::vector < node >    (ntheta)));

  // initialize the central node as the starting point
  //times[0][gx][gx] = 0.0f;
  //parents[0][gx][gx] = node{0, 0, 0, 0};

  times[gx][gx][0] = 0.0f;
  parents[gx][gx][0] = node{0, 0, 0, 0};

  active.push_back(node{0, 0, 0, 0});

  std::vector < node > next;
  next.reserve(1 << 20);

  for (int i = 0; i < 24; i++) {

    next.clear();

    std::cout << i << " " << active.size() << std::endl;

    for (const node & my_cell : active) {

      //float my_time = times[my_cell.theta][my_cell.x + gx][my_cell.y + gx];
      float my_time = times[my_cell.x + gx][my_cell.y + gx][my_cell.theta];

      const auto & neighbor_cells = neighbors[my_cell.theta];

      for (const edge & delta : neighbor_cells) {

        int16_t x = my_cell.x + delta.x;
        int16_t y = my_cell.y + delta.y;
        int16_t v = 0;
        int16_t theta = delta.theta;
        float t = my_time + delta.time;

        if ((abs(x) <= gx) && (abs(y) <= gx)) {

          //float their_time = times[theta][x + gx][y + gx];
          float their_time = times[x + gx][y + gx][theta];

          if (t < their_time) {
            
            //times[theta][x + gx][y + gx] = t;
            //parents[theta][x + gx][y + gx] = my_cell;

            times[x + gx][y + gx][theta] = t;
            parents[x + gx][y + gx][theta] = my_cell;
            next.push_back(node{x, y, v, theta});

          }

        }

      }

    }

    active = next;

  }

}
