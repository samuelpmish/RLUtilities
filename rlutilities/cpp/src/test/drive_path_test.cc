#include "mechanics/drive_path.h"

#include "linear_algebra/math.h"

#include "misc/timer.h"

int main() {

  std::cout << "make a Car" << std::endl;
  Car c;
  c.v = vec3{1000.0f, 0.0f, 0.0f};

  std::cout << "make a DrivePath" << std::endl;
  DrivePath drive(c);

  drive.target = vec3{1600.0, 600.0f, 0.0f};
  drive.arrival_speed = 1234.0f;
  drive.arrival_tangent = normalize(vec3{-1.0f, 1.0f, 0.0f});
  drive.arrival_time = 3.0f;

  drive.recalculate_path();

  int x = 17;
  int y = -4;
  int theta = 11;
  int v = 6;

  uint32_t id = DrivePath::to_id(x, y, theta, v);

  std::cout << x << ' ' << y << ' ' << theta << ' ' << v << " -> " << id << std::endl;

  auto pack = DrivePath::from_id(id);

  x = pack[0];
  y = pack[1];
  theta = pack[2];
  v = pack[3];

  std::cout << id << " -> " << x << ' ' << y << ' ' << theta << ' ' << v << std::endl;

  drive.step(1.0f/60.0f);

  drive.path.write_to_file(std::string("curve_"));

  return 0;

}
