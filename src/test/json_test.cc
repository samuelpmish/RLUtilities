#include "simulation/car.h"
#include "simulation/ball.h"

#include "misc/timer.h"

#include <fstream>
#include <iostream>

int main() {

  timer stopwatch;

  Car c;
  Ball b;

  std::ofstream outfile("test.ndjson");

  stopwatch.start();
  outfile << "{";
  outfile << "car:" << c.to_json() << ",";
  outfile << "ball:" << b.to_json();
  outfile << "}" << std::endl;
  stopwatch.stop();
  std::cout << stopwatch.elapsed() << std::endl;
  
};
