#include "simulation/game.h"
#include "simulation/ball.h"
#include "simulation/field.h"

#include "misc/json.h"

#ifndef GENERATE_PYTHON_BINDINGS
#include "interface.h"
#include "rlbot_generated.h"
#include "statesetting.h"
#endif

#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>

float Game::gravity = -650.0f;
float Game::frametime = 1.0f / 120.0f;
std::string Game::map = "map_not_set";
std::string Game::mode = "mode_not_set";

Game::Game() {
  frame = -1;
  frame_delta = -1;

  num_cars = -1;

  time = -1.0f;
  time_delta = -1.0f;
  time_remaining = -1.0f;

  overtime = false;
  round_active = false;
  kickoff_pause = false;
  match_ended = false;
}

void Game::set_mode(std::string gamemode) {
  mode = gamemode;

  std::cout << "setting mode: " << mode << std::endl;

  if (mode == std::string("soccar")) {
    Field::initialize_soccar();
    Ball::radius = Ball::soccar_radius;
    Ball::collision_radius = Ball::soccar_collision_radius;
    Ball::I = 0.4f * Ball::m * Ball::radius * Ball::radius;
  } else if (mode == std::string("hoops")) {
    Field::initialize_hoops();
    Ball::radius = Ball::hoops_radius;
    Ball::collision_radius = Ball::hoops_collision_radius;
    Ball::I = 0.4f * Ball::m * Ball::radius * Ball::radius;
  } else if (mode == std::string("dropshot")) {
    Field::initialize_dropshot();
    Ball::radius = Ball::dropshot_radius;
    Ball::collision_radius = Ball::dropshot_collision_radius;
    Ball::I = 0.4f * Ball::m * Ball::radius * Ball::radius;
  } else if (mode == std::string("throwback")) {
    Field::initialize_throwback();
    Ball::radius = Ball::soccar_radius;
    Ball::collision_radius = Ball::soccar_collision_radius;
    Ball::I = 0.4f * Ball::m * Ball::radius * Ball::radius;
  } else {
    std::cout << "invalid gametype, use one of the following:" << std::endl;
    std::cout << "\"soccar\"" << std::endl;
    std::cout << "\"hoops\"" << std::endl;
    std::cout << "\"dropshot\"" << std::endl;
    std::cout << "\"throwback\"" << std::endl;
  }
}

// void Game::log(std::string filename) {
//
//  if (filename != log_filename) {
//
//    if (logfile) logfile.close();
//
//    logfile.open(filename);
//
//    log_filename = filename;
//
//  }
//
//  nlohmann::json j;
//
//  j["t"] = time;
//  j["td"] = time_delta;
//  j["f"] = frame;
//  j["fd"] = frame_delta;
//
//  for (int i = 0; i < num_cars; i++) {
//    j[std::string("car_") + std::to_string(i)] = cars[i].to_json();
//  }
//
//  j["ball"] = ball.to_json();
//
//  logfile << j << std::endl;
//
//}

#ifndef GENERATE_PYTHON_BINDINGS
vec3 vector3_to_vec3(const rlbot::flat::Vector3 *flat_vector) {
  return vec3({flat_vector->x(), flat_vector->y(), flat_vector->z()});
}

mat3 rotator_to_mat3(const rlbot::flat::Rotator *flat_vector) {
  return euler_to_rotation(
      vec3({flat_vector->pitch(), flat_vector->yaw(), flat_vector->roll()}));
}

rlbot::DesiredVector3 vec3_to_DesiredVector3(vec3 v)
{
  return {v[0], v[1], v[2]};
}

rlbot::DesiredRotator vec3_to_DesiredRotator(vec3 v)
{
  return {v[0], v[1], v[2]};
}

rlbot::GameState Game::ToGameState() {
  rlbot::GameState game_state;

  // set ball state
  game_state.ballState.physicsState.location = vec3_to_DesiredVector3(ball.position);
  game_state.ballState.physicsState.velocity = vec3_to_DesiredVector3(ball.velocity);
  game_state.ballState.physicsState.angularVelocity = vec3_to_DesiredVector3(ball.angular_velocity);

  // set car states
  for (size_t i = 0; i < num_cars; i++)
  {
    rlbot::CarState car_state;
    car_state.physicsState.location = vec3_to_DesiredVector3(cars[i].position);
    car_state.physicsState.velocity = vec3_to_DesiredVector3(cars[i].velocity);
    car_state.physicsState.angularVelocity = vec3_to_DesiredVector3(cars[i].angular_velocity);
    car_state.physicsState.rotation = vec3_to_DesiredRotator(rotation_to_euler(cars[i].orientation));
    car_state.boostAmount = cars[i].boost;

    game_state.carStates[i] = car_state;
  }
  
  return game_state;
}


void Game::read_flatbuffer_packet(
    const rlbot::flat::GameTickPacket *gameTickPacket,
    const rlbot::flat::FieldInfo *fieldInfo) {
  float time_old = time;

  time = gameTickPacket->gameInfo()->secondsElapsed();
  time_delta = time - time_old;

  time_remaining = gameTickPacket->gameInfo()->gameTimeRemaining();
  overtime = gameTickPacket->gameInfo()->isOvertime();
  round_active = gameTickPacket->gameInfo()->isRoundActive();
  kickoff_pause = gameTickPacket->gameInfo()->isKickoffPause();
  match_ended = gameTickPacket->gameInfo()->isMatchEnded();
  gravity = gameTickPacket->gameInfo()->worldGravityZ();

  num_cars = gameTickPacket->players()->size();

  for (uint32_t i = 0; i < gameTickPacket->players()->size(); i++) {
    auto car = gameTickPacket->players()->Get(i);

    cars[i].id = i;
    cars[i].position = vector3_to_vec3(car->physics()->location());
    cars[i].velocity = vector3_to_vec3(car->physics()->velocity());
    cars[i].angular_velocity = vector3_to_vec3(car->physics()->angularVelocity());
    cars[i].orientation = rotator_to_mat3(car->physics()->rotation());

    // other car data
    cars[i].boost = car->boost();
    cars[i].jumped = car->jumped();
    cars[i].double_jumped = car->doubleJumped();
    cars[i].on_ground = car->hasWheelContact();
    cars[i].supersonic = car->isSupersonic();
    cars[i].team = car->team();

    cars[i].time = time;
  }

  ball.position = vector3_to_vec3(gameTickPacket->ball()->physics()->location());
  ball.velocity = vector3_to_vec3(gameTickPacket->ball()->physics()->velocity());
  ball.angular_velocity =
      vector3_to_vec3(gameTickPacket->ball()->physics()->angularVelocity());

  ball.time = time;

  int num_boost_pads = fieldInfo->boostPads()->size();

  int num_game_boosts = gameTickPacket->boostPadStates()->size();

  if (num_boost_pads != num_game_boosts) {
    std::cout << "boost pad info mismatch" << std::endl;
  } else {
    if (num_boost_pads > pads.size()) pads.resize(num_boost_pads);

    for (int i = 0; i < num_boost_pads; i++) {
      auto boost_pad = fieldInfo->boostPads()->Get(i);
      auto game_boost = gameTickPacket->boostPadStates()->Get(i);

      pads[i].position = vector3_to_vec3(boost_pad->location());
      pads[i].is_full_boost = boost_pad->isFullBoost();
      pads[i].is_active = game_boost->isActive();
      pads[i].timer = game_boost->timer();
    }
  }
}
#endif
