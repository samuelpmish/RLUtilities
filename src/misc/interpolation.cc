#include "misc/interpolation.h"

Interpolation::Interpolation(std::vector< pair_type > _values) {
  values = _values;
  std::sort(values.begin(), values.end(), [](const pair_type & a, const pair_type & b){
    return (a.first < b.first);
  });
}


