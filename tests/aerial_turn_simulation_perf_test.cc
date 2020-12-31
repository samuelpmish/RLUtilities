#include "misc/timer.h"

#include "simulation/car.h"

#include "mechanics/reorient.h"

#include "linear_algebra/math.h"

int main() {

  timer stopwatch;

  int n = 1000;
  float prevent_optimizing_out = 0.0f;  
  Car c;
  c.on_ground = false;
  c.time = 0.0f;
  c.position = vec3{0.0f, 0.0f, 0.0f};
  c.velocity = vec3{500.0f, 0.0f, 0.0f};
  c.angular_velocity = vec3{1.1f, -2.1f, 0.5f};
  c.orientation = axis_to_rotation(vec3{0.5f, 1.9f, -0.8f});

  stopwatch.start();
  for (int i = 0; i < n; i++) {
    Car copy = Car(c);
    Reorient reorient(copy);
    reorient.target_orientation = eye<3>();
    Car end = reorient.simulate();
    prevent_optimizing_out += end.angular_velocity[0];
  }
  stopwatch.stop();

  std::cout << "simulated " << n << " aerial turns in " << stopwatch.elapsed() << " s" << std::endl;

  return 0;

}
