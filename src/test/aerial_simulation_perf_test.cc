#include "misc/timer.h"

#include "simulation/car.h"

#include "mechanics/aerial.h"

#include "linear_algebra/math.h"

int main() {

  timer stopwatch;

  int n = 1000;
  float prevent_optimizing_out = 0.0f;  
  Car c;
  c.on_ground = true;
  c.time = 0.0f;
  c.position = vec3{0.0f, 0.0f, 0.0f};
  c.velocity = vec3{500.0f, 0.0f, 0.0f};
  c.angular_velocity = vec3{0.0f, 0.0f, 0.0f};
  c.orientation = eye < 3 >();

  Car copy = Car(c);
  auto aerial = Aerial(copy);
  aerial.target_position = vec3{2000.0f, 300.0f, 1000.0f};
  aerial.arrival_time = 3.0f;

  stopwatch.start();
  for (int i = 0; i < n; i++) {
    Car end = aerial.simulate();
    prevent_optimizing_out += end.orientation[0];
  }
  stopwatch.stop();

  std::cout << "simulated " << n << " aerials in " << stopwatch.elapsed() << " s" << std::endl;

  return 0;

}
