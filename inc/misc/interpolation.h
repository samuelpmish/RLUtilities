#pragma once

#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>

#include "linear_algebra/math.h"

template < typename output_type >
class Interpolation {

 typedef std::pair< float, output_type > pair_type;

 public:
  std::vector < pair_type > values;

  Interpolation() {}

  Interpolation(std::vector< pair_type > _values) {
    values = _values;
    std::sort(values.begin(), values.end(), [](const pair_type & a, const pair_type & b){
      return (a.first < b.first);
    });
  }

  Interpolation(std::initializer_list< pair_type > _values) {
    values = std::vector(_values.begin(), _values.end());
    std::sort(values.begin(), values.end(), [](const pair_type & a, const pair_type & b){
      return (a.first < b.first);
    });
  }



  output_type operator()(float input) {
  
    float t = clip(input, values.front().first, values.back().first);
  
    if (values.size() >= 2) {
  
      for (size_t i = 0; i < values.size()-1; i++) {
        if (values[i].first <= t && t <= values[i+1].first) {
          float u = (t - values[i].first) / (values[i+1].first - values[i].first); 
          float v = (values[i+1].first - t) / (values[i+1].first - values[i].first); 
          return v * values[i].second + u * values[i+1].second;
        }
      }
  
    } else {
  
      std::cout << "not enough values to interpolate" << std::endl;
  
    }
  
    return output_type();
    
  }
};
