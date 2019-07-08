#include "misc/random.h"

#include <chrono>

std::default_random_engine rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::uniform_real_distribution< float > dist(0.0, 1.0);

float random::real(float a, float b) {
  return a + dist(rng) * (b - a);
}
