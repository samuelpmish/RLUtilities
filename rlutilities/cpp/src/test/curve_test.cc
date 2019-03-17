#include "simulation/curve.h"

#include "linear_algebra/math.h"

#include "misc/timer.h"

#include <fstream>

int main() {

  timer stopwatch;

  stopwatch.start();

  auto path = Curve({
    ControlPoint{vec3{ 1.0f,  0.0f, 0.0f}, vec3{ 0.0f,  1.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}},
    ControlPoint{vec3{ 0.0f,  1.0f, 0.0f}, vec3{-1.0f,  0.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}},
    ControlPoint{vec3{-1.0f,  0.0f, 0.0f}, vec3{ 0.0f, -1.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}},
    ControlPoint{vec3{ 0.0f, -1.0f, 0.0f}, vec3{ 1.0f,  0.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}}
  });

  stopwatch.stop();

  std::cout << stopwatch.elapsed() << std::endl;


  for (int i = 0; i < path.points.size(); i++) {
    std::cout << path.points[i] << ' ';
    std::cout << path.tangents[i] << ' ';
    std::cout << path.curvatures[i] << std::endl;
  }

  return 0;

}
