#include "mechanics/slide.h"

extern std::vector < vec2 > translation_LUT;
extern std::vector < vec2 > orientation_LUT;
extern std::vector < float > speed_LUT;
extern std::vector < float > time_LUT;

Slide::Slide(Car & c) : car(c) {
  finished = false;
  controls = Input();

  target = vec3{0.0f, 0.0f, 0.0f};
}
