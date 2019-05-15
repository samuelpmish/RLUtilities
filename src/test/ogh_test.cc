#include "simulation/ogh.h"

#include "linear_algebra/math.h"

#include "misc/timer.h"

#include <fstream>

int main() {

  auto path = OGH({
    vec3{5.0f,  0.0f, 0.0f},
    vec3{0.0f,  1.0f, 0.0f},
    vec3{0.0f,  5.0f, 0.0f},
    vec3{-1.0f,  0.0f, 0.0f}
  });

  for (int i = 0; i < 32; i++) {
    float t = float(i) / 31.0f;
    std::cout << path.evaluate(t) << ' ';
    std::cout << path.tangent(t) << ' ';
    std::cout << path.acceleration(t) << std::endl;
  }

  return 0;

}
