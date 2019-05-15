#include "simulation/game.h"
#include "simulation/ball.h"
#include "simulation/field.h"

#include "linear_algebra/math.h"

#include "misc/timer.h"

#include <iostream>

int main() {

  timer stopwatch;

  Game::set_mode(std::string("soccar"));

  int n = 10000000;

  Ball b;

  b.x = vec3{0.0f, 0.0f, 100.0f};
  b.v = vec3{0.0f, 0.0f, 1000.0f};
  b.w = vec3{0.0f, 0.0f, 0.0f};

  stopwatch.start();
  for (int i = 0; i < n; i++) {
    b.step(1.0f / 120.0f);
  }
  stopwatch.stop();

  std::cout << stopwatch.elapsed() << ' ' << b.x << std::endl;

  return 0;

}
