#pragma once

#include <vector>
#include <string>

namespace Base64 {
  std::string Encode(const std::vector<uint8_t> & data);
  std::vector<uint8_t> Decode(const std::string & input);
  std::vector<uint8_t> Decode(const std::vector < std::string > & inputs);
};