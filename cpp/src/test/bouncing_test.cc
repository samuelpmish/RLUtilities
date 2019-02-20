#include "simulation/ball.h"
#include "simulation/field.h"
#include "simulation/gametype.h"

#include "linear_algebra/math.h"

#include <fstream>

int main() {

  gametype(std::string("soccar"));

  int n = 10000;

  Ball b;

  b.x = vec3{0.0f, 0.0f, 100.0f};
  b.v = vec3{0.0f, 0.0f, 1000.0f};
  b.w = vec3{0.0f, 0.0f, 0.0f};

  std::ofstream outfile("bouncing_60Hz.txt");

  for (int i = 0; i < n; i++) {
    b.step(1.0f / 60.0f);
    outfile << b.x << ' ' << b.v << '\n';
  }

  outfile.close();


  b.x = vec3{0.0f, 0.0f, 100.0f};
  b.v = vec3{0.0f, 0.0f, 1000.0f};
  b.w = vec3{0.0f, 0.0f, 0.0f};

  outfile.open("bouncing_120Hz.txt");

  for (int i = 0; i < n; i++) {
    b.step(1.0f / 120.0f);
    outfile << b.x << ' ' << b.v << '\n';
  }

  outfile.close();

  return 0;

}
