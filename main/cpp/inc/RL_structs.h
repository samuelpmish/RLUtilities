#pragma once

#define CONST_MaxBoosts  50
#define CONST_MaxPlayers 10

struct Vector3 {
  float X;
  float Y;
  float Z;
};

struct Rotator {
  int Pitch;
  int Yaw;
  int Roll;
};

struct Input {
  float steer;
  float roll;
  float pitch;
  float yaw;
  float throttle;
  bool jump;
  bool boost;
  bool slide;
  bool handbrake;
};

struct ScoreInfo {
  int Score;
  int Goals;
  int OwnGoals;
  int Assists;
  int Saves;
  int Shots;
  int Demolitions;
};

struct PlayerInfo {
  Vector3       Location;
  Rotator       Rotation;
  Vector3       Velocity;
  Vector3       AngularVelocity;
  ScoreInfo     Score;
  bool          Demolished;
  bool          OnGround;
  bool          SuperSonic;
  bool          Bot;
  bool          Jumped;
  bool          DoubleJumped;
  wchar_t       Name[32];
  unsigned char Team;
  int           Boost;
};

struct BoostInfo {
  Vector3 Location;
  bool    Active;
  float   Timer;
};

struct Touch {
  wchar_t PlayerName[32];
  float   TimeSeconds;
  Vector3 HitLocation;
  Vector3 HitNormal;
};

struct BallInfo {
  Vector3 Location;
  Rotator Rotation;
  Vector3 Velocity;
  Vector3 AngularVelocity;
  Vector3 Acceleration;
  Touch   LatestTouch;
};

struct GameInfo {
  float TimeSeconds;
  float GameTimeRemaining;
  bool  OverTime;
  bool  UnlimitedTime;
  bool  RoundActive;
  bool  BallHasBeenHit;
  bool  MatchEnded;
};

struct LiveDataPacket {
  PlayerInfo GameCars[CONST_MaxPlayers];
  int        NumCars;
  BoostInfo  GameBoosts[CONST_MaxBoosts];
  int        NumBoosts;
  BallInfo   GameBall;
  GameInfo   GameInfo;
};

struct LiveDataWrapper {
  long           ExchangedValue;
  LiveDataPacket LiveDataPacket;
};
