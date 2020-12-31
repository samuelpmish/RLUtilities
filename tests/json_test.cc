#include <fstream>
#include <iostream>

#include "misc/timer.h"
#include "misc/logging.h"

#include "simulation/car.h"
#include "simulation/ball.h"

#include "mechanics/dodge.h"

int main() {

  timer stopwatch;

  Ball b;
  Car c;
  Dodge d(c);

  d.preorientation = eye<3>();

  Input i;

  std::ofstream outfile("test.ndjson");

  stopwatch.start();
  nlohmann::json log;
  log["ball"] = to_json(b);
  log["car"] = to_json(c);
  log["state"] = to_json(d);
  log["inputs"] = to_json(i);
  outfile << log << std::endl;
  outfile << log << std::endl;
  outfile << log << std::endl;
  stopwatch.stop();
  std::cout << stopwatch.elapsed() << std::endl;

  outfile.close();
  
};
