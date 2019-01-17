#include <pybind11/pybind11.h>
#include "linalg.h"


vec2 return_vec2(float x, float y) {

    vec2 result = {x, y};
    return result;
}


PYBIND11_MODULE(example, m) {

    m.def("return_vec2", &return_vec2);
}