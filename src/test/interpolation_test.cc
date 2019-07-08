#include "misc/interpolation.h"
#include "linear_algebra/math.h"

int main() {

  Interpolation<float> curvature = {{   0.0f, 0.00690f},
                                    { 500.0f, 0.00398f},
                                    {1000.0f, 0.00235f},
                                    {1500.0f, 0.00138f},
                                    {1750.0f, 0.00110f},
                                    {2300.0f, 0.00088f}};

  for (int i = 0; i < 2300; i+=10) {
    std::cout << i << " " << curvature(i) << std::endl;

  }

  Interpolation<vec2> vector_valued = {{   0.0f, vec2{0.0f, 5.0f}},
                                       { 500.0f, vec2{1.0f, 4.0f}},
                                       {1000.0f, vec2{2.0f, 3.0f}},
                                       {1500.0f, vec2{3.0f, 2.0f}},
                                       {1750.0f, vec2{4.0f, 1.0f}},
                                       {2300.0f, vec2{5.0f, 0.0f}}};

  for (int i = 0; i <= 2400; i+=10) {
    std::cout << i << " " << curvature(i) << " " << vector_valued(i) << std::endl;
  }

}
