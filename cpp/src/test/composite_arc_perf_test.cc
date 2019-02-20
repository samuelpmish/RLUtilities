#include "simulation/composite_arc.h"

#include "linear_algebra/math.h"

#include "misc/timer.h"

int main() {

  timer stopwatch;

  int n = 1000;
  float prevent_optimizing_out = 0.0f;  
  CompositeArc path;

  stopwatch.start();
  for (int i = 0; i < n; i++) {
    path = CompositeArc(
      200.0f + float(i) / n,
      vec2{0.0f, 0.0f},
      vec2{1.0f, 0.0f},
      -400.0f,
      100.0f,
      vec2{1000.0f, -1000.0f},
      normalize(vec2{1.0f, 1.0f}),
      800.0f
    );
    prevent_optimizing_out += path.phi1;
  }
  stopwatch.stop();

  std::cout << "generated " << n << " paths in " << stopwatch.elapsed() << " s" << std::endl;

  stopwatch.start();
  for (int i = 0; i < n; i++) {
    auto curve = path.discretize(32 + i%2);
    prevent_optimizing_out += curve.length;
  }
  stopwatch.stop();

  std::cout << "discretized " << n << " paths in " << stopwatch.elapsed() << " s" << std::endl;

  return 0;

}
