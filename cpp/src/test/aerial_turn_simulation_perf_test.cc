#include "misc/timer.h"

#include "simulation/car.h"

#include "mechanics/aerial_turn.h"

#include "linear_algebra/math.h"

int main() {

  timer stopwatch;

  int n = 1000;
  float prevent_optimizing_out = 0.0f;  
  Car c;
  c.on_ground = false;
  c.time = 0.0f;
  c.x = vec3{0.0f, 0.0f, 0.0f};
  c.v = vec3{500.0f, 0.0f, 0.0f};
  c.w = vec3{1.1f, -2.1f, 0.5f};
  c.o = axis_to_rotation(vec3{0.5f, 1.9f, -0.8f});

  stopwatch.start();
  for (int i = 0; i < n; i++) {
    Car copy = Car(c);
    AerialTurn aerial_turn = AerialTurn(copy);
    aerial_turn.target = eye<3>();
    Car end = aerial_turn.simulate();
    prevent_optimizing_out += end.w[0];
  }
  stopwatch.stop();

  std::cout << "simulated " << n << " aerial turns in " << stopwatch.elapsed() << " s" << std::endl;

  return 0;

}
