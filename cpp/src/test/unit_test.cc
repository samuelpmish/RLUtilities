#include "simulation/car.h"

#include "linear_algebra/math.h"

int main() {

  Car c;

  c.x = vec3{0.0f, 0.0f, 100.0f};

  std::cout << c.x << " " << c.v << std::endl;

  return 0;

}
