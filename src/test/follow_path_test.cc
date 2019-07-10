#include <fstream>
#include <iostream>

#include "misc/logging.h"

#include "simulation/car.h"
#include "simulation/game.h"
#include "simulation/field.h"

#include "experimental/navigator.h"
#include "experimental/follow_path.h"

using json = nlohmann::json;

int main(int argc, char** argv) {

  if (argc == 3) {

    std::string infilename(argv[1]);
    std::ifstream infile(infilename);

    std::string outfilename(argv[2]);
    std::ofstream outfile(outfilename);

    json input, output;

    infile >> input;
  
    Game g;
    Game::set_mode(std::string("soccar"));
  
    g.time = 0.0f;

    g.cars[0].position = vec3{input["car"]["x"][0], input["car"]["x"][1], input["car"]["x"][2]};
    g.cars[0].velocity = vec3{input["car"]["v"][0], input["car"]["v"][1], input["car"]["v"][2]};
    g.cars[0].orientation = mat3{
      {input["car"]["o"][0][0], input["car"]["o"][0][1], input["car"]["o"][0][2]},
      {input["car"]["o"][1][0], input["car"]["o"][1][1], input["car"]["o"][1][2]},
      {input["car"]["o"][2][0], input["car"]["o"][2][1], input["car"]["o"][2][2]}
    };

    Navigator nav(g.cars[0]);
    nav.debug = true;

    nav.analyze_surroundings(100);

    vec3 destination = vec3{input["dest"][0], input["dest"][1], input["dest"][2]};
    vec3 tangent = vec3{input["tangent"][0], input["tangent"][1], input["tangent"][2]};
    float offset = input["offset"];

    Curve path = nav.path_to(destination, tangent, offset);

    FollowPath follow(g.cars[0]);

    float arrival_time = input["time"];
    float arrival_speed = input["speed"];

    follow.calculate_plan(path, arrival_time, arrival_speed);
          
    output["path"] = to_json(follow.path);
    output["follow"] = to_json(follow);

    outfile << output << std::endl;
  } else {
    std::cout << "error: must specify input file. exiting..." << std::endl;
  }


  return 0;
}
