#pragma once

#include "simulation/game.h"

struct UpdateBall {
  vec3 position;
  vec3 velocity;
  rotator euler_angles;
  vec3 angular_velocity;
  float damage;
};

struct UpdateCar {
  vec3 position;
  vec3 velocity;
  rotator euler_angles;
  vec3 angular_velocity;
  uint8_t boost;
  CarState state;
};

struct UpdateGoal {
  GoalState state; 
};

struct UpdateBoostPad {
  BoostPadState state;
};

struct Update {
  int frame;
  float time_left;
  float time_elapsed;
  int score[2];

  UpdateBall ball;
  std::vector < UpdateCar > cars;
  std::vector < UpdateGoal > goals;
  std::vector < UpdateBoostPad > pads;

  GameState state;
  bool is_overtime;
  bool is_round_active;
  bool is_kickoff_paused;
  bool is_match_ended;
};

inline void operator<<(Game & game, const Update & updated) {

  game.frame = updated.frame;
  game.time_remaining = updated.time_left;
  game.time = updated.time_elapsed;
  game.score[0] = updated.score[0];
  game.score[1] = updated.score[1];

  game.ball.time = game.time;
  game.ball.position = updated.ball.position;
  game.ball.velocity = updated.ball.velocity;
  game.ball.angular_velocity = updated.ball.angular_velocity;

  for (int i = 0; i < updated.cars.size(); i++) {
    game.cars[i].time = game.time;
    game.cars[i].position = updated.cars[i].position;
    game.cars[i].velocity = updated.cars[i].velocity;
    game.cars[i].angular_velocity = updated.cars[i].angular_velocity;

    vec3 pyr{updated.cars[i].euler_angles.pitch, 
             updated.cars[i].euler_angles.yaw, 
             updated.cars[i].euler_angles.roll}; 

    game.cars[i].orientation = euler_to_rotation(pyr);
    game.cars[i].state = updated.cars[i].state;
    game.cars[i].on_ground = game.cars[i].state == CarState::OnGround;
  }

  for (int i = 0; i < updated.pads.size(); i++) {
    game.pads[i].state = updated.pads[i].state;
  }

  for (int i = 0; i < updated.goals.size(); i++) {
    game.goals[i].state = updated.goals[i].state;
  }
}