#include "mechanics/drive_path.h"

#include "linear_algebra/math.h"

#include "misc/timer.h"

int main() {

#if 0
  std::cout << "make a Car" << std::endl;
  Car c;
  c.x = vec3{2000.0f, 1000.0f, 0.0f};
  c.v = vec3{1000.0f, 0.0f, 0.0f};

  std::cout << "make a DrivePath" << std::endl;
  DrivePath drive(c);

  drive.target = vec3{1600.0, 600.0f, 0.0f};
  drive.arrival_speed = 1234.0f;
  drive.arrival_tangent = normalize(vec3{-1.0f, 1.0f, 0.0f});
  drive.arrival_time = 3.0f;

  //drive.recalculate_path();

  //int x = 17;
  //int y = -4;
  //int theta = 11;
  //int v = 6;

  //uint32_t id = DrivePath::to_id(x, y, theta, v);

  //std::cout << x << ' ' << y << ' ' << theta << ' ' << v << " -> " << id << std::endl;

  //auto pack = DrivePath::from_id(id);

  //x = pack[0];
  //y = pack[1];
  //theta = pack[2];
  //v = pack[3];

  //std::cout << id << " -> " << x << ' ' << y << ' ' << theta << ' ' << v << std::endl;

  //drive.step(1.0f/60.0f);

  //drive.path.write_to_file(std::string("curve_"));

  std::cout << "sssp" << std::endl;
  drive.sssp(4000.0f, 1000.0f);

  std::cout << drive.plan_path(vec3{4096.0f, 1000.0f, 512.0f}, vec3{0.0f, 1.0f, 0.0f}) << std::endl; 

  for (int i = 0; i < drive.path.points.size(); i++) {
    std::cout << drive.path.points[i] << std::endl;

  }

#endif
  return 0;

}
