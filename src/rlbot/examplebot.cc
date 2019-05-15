#include "rlbot/examplebot.h"

#include <math.h>
#include <string>

#include "rlbot/bot.h"
#include "rlbot/interface.h"
#include "rlbot/namedrenderer.h"
#include "rlbot/rlbot_generated.h"
#include "rlbot/statesetting.h"

#define PI 3.1415

ExampleBot::ExampleBot(int _index, int _team, std::string _name)
    : Bot(_index, _team, _name) {
  GameState gamestate = GameState();

  gamestate.ballState.physicsState.location = {0, 0, 1000};
  gamestate.ballState.physicsState.velocity = {0, 0, 5000};

  CarState carstate = CarState();
  carstate.physicsState.location = {0, 500, 1000};
  carstate.physicsState.velocity = {500, 1000, 1000};
  carstate.physicsState.angularVelocity = {1, 2, 3};

  carstate.boostAmount = 50;

  gamestate.carStates[_index] = carstate;

  gamestate.BuildAndSend();
}

Input ExampleBot::GetOutput(Game g) {

//  NamedRenderer renderer("test");
//
//  renderer.StartPacket();
//
//  std::vector<const rlbot::flat::Vector3*> points;
//
//  for (uint32_t i = 0; i < ballPrediction->slices()->size(); i++) {
//    points.push_back(ballPrediction->slices()->Get(i)->physics()->location());
//  }
//
//  renderer.DrawPolyLine3D(Color{0xFF, 0x00, 0x00, 0xFF}, points);
//
//  renderer.DrawString2D("Hello world!", Color{0x00, 0xFF, 0x00, 0xFF}, {10, 10, 0}, 4, 4);
//  renderer.DrawString3D(std::to_string(velocity), Color{0xFF, 0x00, 0xFF, 0xFF},
//                        ballLocation, 2, 2);
//
//  renderer.FinishAndSend();
//
//  // Calculate to get the angle from the front of the bot's car to the ball.
//  double botToTargetAngle = atan2(ballLocation.y() - carLocation.y(),
//                                  ballLocation.x() - carLocation.x());
//  double botFrontToTargetAngle = botToTargetAngle - carRotation.yaw();
//  // Correct the angle
//  if (botFrontToTargetAngle < -PI) botFrontToTargetAngle += 2 * PI;
//  if (botFrontToTargetAngle > PI) botFrontToTargetAngle -= 2 * PI;
//
//  // Decide which way to steer in order to get to the ball.
//  if (botFrontToTargetAngle > 0)
//    controller.steer = 1;
//  else
//    controller.steer = -1;
//
//  controller.throttle = 1.0f;

  return Input{};
}
