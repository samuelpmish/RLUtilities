#include "mechanics/dodge.h"

#include "simulation/car.h"
#include "simulation/ball.h"

#include "misc/timer.h"

#include <fstream>
#include <iostream>

int main() {

  timer stopwatch;

  Ball b;
  Car c;
  Dodge d(c);

  std::ofstream outfile("test.ndjson");

  stopwatch.start();
  nlohmann::json log;
  log["ball"] = b.to_json();
  log["car"] = c.to_json();
  log["state"] = d.to_json();
  outfile << log << std::endl;
  outfile << log << std::endl;
  outfile << log << std::endl;
  stopwatch.stop();
  std::cout << stopwatch.elapsed() << std::endl;

  outfile.close();
  
};
