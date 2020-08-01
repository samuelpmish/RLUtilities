#pragma once

#include <string>
#include <vector>
#include <variant>

#include "messages/initialize.h"
#include "messages/update.h"
#include "messages/custom.h"

struct Terminate{};

namespace Incoming{

  using Message = std::variant <
    Update,
    Initialize,
    Terminate,
    Custom
  >;

  std::vector < Message > deserialize(const std::string &);

}