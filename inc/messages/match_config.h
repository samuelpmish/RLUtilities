#pragma once

#include <string>

struct MatchConfig{

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

  std::string start_when;
  bool force_restart;

  int num_players;
  int num_cars;

  MatchConfig() {
    map = "Mannfield";
    mode = "soccer";
    playtest = "enabled";
    match_length = "5 minutes";
    max_score = "unlimited";
    overtime = "unlimited";
    series = "unlimited";
    speed = "default";
    max_ball_speed = "default";
    ball_shape = "default";
    ball_weight = "default";
    ball_size = "default";
    ball_bounciness = "default";
    boost = "default";
    boost_strength = "default";
    gravity = "default";
    demolition = "default";
    respawn = "3 seconds";

    start_when = "immediately";
    //start_when = "all bots ready";
    num_players = 2;
    num_cars = 2;

    force_restart = false;
  }
};