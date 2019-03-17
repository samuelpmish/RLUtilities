#include "simulation/game.h"
#include "simulation/ball.h"
#include "simulation/field.h"

#include "linear_algebra/math.h"

#include "misc/timer.h"

#include <fstream>
#include <iostream>

int main() {

  Game::set_mode(std::string("soccar"));

  Field::write_mesh_files();

  return 0;

}
