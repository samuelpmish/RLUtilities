#pragma once

#include "simulation/game.h"

struct InitializeCar {
  std::string name;
  int team;
  bool is_bot;

  CarBody body_type;
  vec3 hitbox_offset;
  vec3 hitbox_dimensions;
};

struct InitializeBall {
  BallShape shape;
  float radius;
  float height;
};

struct InitializeBoostPad {
  vec3 position;
  BoostPadType type;
};

struct InitializeGoal {
  vec3 position;
  vec3 direction;
  float width;
  float height;
  uint8_t team;
};

struct Initialize {
  std::string map;
  std::string mode;
  std::string playtest;
  std::string match_length;
  std::string max_score;
  std::string overtime;
  std::string series;
  std::string speed;
  std::string max_ball_speed;
  std::string ball_shape;
  std::string ball_weight;
  std::string ball_size;
  std::string ball_bounciness;
  std::string boost;
  std::string boost_strength;
  std::string gravity;
  std::string demolition;
  std::string respawn;

  InitializeBall ball;
  std::vector < InitializeCar > cars;
  std::vector < InitializeBoostPad > pads;
  std::vector < InitializeGoal > goals;
};


inline void operator<<(Game & game, const Initialize & init) {

  if (init.mode == "soccer") {
    Game::set_mode("soccar");
  }


  //Ball::radius = init.ball.radius;

  //game.map = init.map;
  //init.ball.shape;
  //init.ball.radius;
  //init.gravity;

  game.cars.resize(init.cars.size());
  for (uint16_t i = 0; i < init.cars.size(); i++) {
    //game.cars[i].name = init.cars[i].name;
    game.cars[i].team = init.cars[i].team;
    //game.cars[i].is_bot = init.cars[i].is_bot;
    //game.cars[i].body_type = init.cars[i].body_type;  
    game.cars[i].hitbox_offset = init.cars[i].hitbox_offset;  
    game.cars[i].hitbox_widths = init.cars[i].hitbox_dimensions / 2.0;  

    std::cout << game.cars[i].hitbox_widths;
  }

  game.pads.reserve(init.pads.size());
  for (uint16_t i = 0; i < init.pads.size(); i++) {
    game.pads[i].position = init.pads[i].position;
    game.pads[i].type = init.pads[i].type;
  }

  game.goals.reserve(init.goals.size());
  for (uint16_t i = 0; i < init.goals.size(); i++) {
    game.goals[i].position = init.goals[i].position;
    game.goals[i].direction = init.goals[i].direction;
    game.goals[i].width = init.goals[i].width;
    game.goals[i].height = init.goals[i].height;
    game.goals[i].team = init.goals[i].team;
  } 

}