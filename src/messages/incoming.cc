#include "messages/incoming.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include <iostream>
#include <unordered_map>

namespace impl{

  template < typename T >
  T deserialize(const rapidjson::Value & json);

  template <>
  vec3 deserialize(const rapidjson::Value & json) {
    return vec3{
      static_cast<float>(json[0].GetDouble()),
      static_cast<float>(json[1].GetDouble()),
      static_cast<float>(json[2].GetDouble())
    };
  }

  template <>
  rotator deserialize(const rapidjson::Value & json) {
    return rotator{
      static_cast<float>(json[0].GetDouble()),
      static_cast<float>(json[1].GetDouble()),
      static_cast<float>(json[2].GetDouble())
    };
  }

  template <>
  UpdateBall deserialize(const rapidjson::Value & json) {

    UpdateBall ball;
    
    ball.position = deserialize<vec3>(json["position"]);
    ball.velocity = deserialize<vec3>(json["velocity"]);
    ball.euler_angles = deserialize<rotator>(json["euler_angles"]);
    ball.angular_velocity = deserialize<vec3>(json["angular_velocity"]);
    ball.damage = static_cast<float>(json["damage"].GetDouble());

    return ball; 

  }
  
  template <>
  UpdateCar deserialize(const rapidjson::Value & json) {

    UpdateCar car;

    car.position = deserialize<vec3>(json["position"]);
    car.velocity = deserialize<vec3>(json["velocity"]);
    car.euler_angles = deserialize<rotator>(json["euler_angles"]);
    car.angular_velocity = deserialize<vec3>(json["angular_velocity"]);
    car.boost = json["boost"].GetInt();
    car.state = static_cast<CarState>(json["state"].GetInt());

    return car;

  }

  template <>
  UpdateGoal deserialize(const rapidjson::Value & json) {
    return UpdateGoal{static_cast<GoalState>(json["state"].GetInt())};
  }

  template <>
  UpdateBoostPad deserialize(const rapidjson::Value & json) {
    return UpdateBoostPad{static_cast<BoostPadState>(json["available"].GetBool())};
  }

  template <>
  Update deserialize(const rapidjson::Value & json) {
  
    Update game;
  
    game.frame = json["frame"].GetInt();
  
    const rapidjson::Value & score = json["score"];
    game.score[0] = score[0].GetInt();
    game.score[1] = score[1].GetInt();
  
    game.ball = deserialize< UpdateBall >(json["ball"]);

    const rapidjson::Value & cars = json["cars"];
    game.cars.resize(cars.Size());
    for (uint32_t i = 0; i < cars.Size(); i++) {
      game.cars[i] = deserialize< UpdateCar >(cars[i]);
    }
  
    const rapidjson::Value & goals = json["goals"];
    game.goals.resize(goals.Size());
    for (uint32_t i = 0; i < goals.Size(); i++) {
      game.goals[i] = deserialize< UpdateGoal >(goals[i]);
    }
  
    const rapidjson::Value & pads = json["pads"];
    game.pads.resize(pads.Size());
    for (uint32_t i = 0; i < pads.Size(); i++) {
      game.pads[i] = deserialize< UpdateBoostPad >(pads[i]);
    }
  
    game.time_left = static_cast<float>(json["time_left"].GetDouble());
    game.time_elapsed = static_cast<float>(json["time_elapsed"].GetDouble());

    game.state = GameState(json["state"].GetInt());
    return game;
  }
  
  template <>
  Terminate deserialize(const rapidjson::Value & json) {
    return Terminate{};
  }

  template <>
  Custom deserialize(const rapidjson::Value & json) {
    return Custom {
      json["index"].GetInt(),
      json["contents"].GetString()
    };
  }

  template <>
  InitializeBall deserialize(const rapidjson::Value & json) {
    return InitializeBall {
      BallShape(json["shape"].GetInt()),
      float(json["radius"].GetDouble()),
      float(json["height"].GetDouble())
    };
  }

  template <>
  InitializeCar deserialize(const rapidjson::Value & json) {
    return InitializeCar {
      json["name"].GetString(),
      json["team"].GetInt(),
      json["is_bot"].GetBool(),
      CarBody(json["body_type"].GetInt()),
      deserialize< vec3 >(json["hitbox_offset"]),
      deserialize< vec3 >(json["hitbox_dimensions"])
    };
  }

  template <>
  InitializeGoal deserialize(const rapidjson::Value & json) {
    return InitializeGoal {
      deserialize< vec3 >(json["position"]),
      deserialize< vec3 >(json["direction"]),
      float(json["width"].GetDouble()),
      float(json["height"].GetDouble()),
      uint8_t(json["team"].GetInt())
    };
  }

  template <>
  InitializeBoostPad deserialize(const rapidjson::Value & json) {
    return InitializeBoostPad {
      deserialize< vec3 >(json["position"]),
      BoostPadType(json["type"].GetInt())
    };
  }

  template <>
  Initialize deserialize(const rapidjson::Value & json) {
    Initialize init;

    if (json.HasMember("map")) init.map = json["map"].GetString();
    if (json.HasMember("mode")) init.mode = json["mode"].GetString();
    if (json.HasMember("playtest")) init.playtest = json["playtest"].GetString();
    if (json.HasMember("match_length")) init.match_length = json["match_length"].GetString();
    if (json.HasMember("max_score")) init.max_score = json["max_score"].GetString();
    if (json.HasMember("overtime")) init.overtime = json["overtime"].GetString();
    if (json.HasMember("series")) init.series = json["series"].GetString();
    if (json.HasMember("speed")) init.speed = json["speed"].GetString();
    if (json.HasMember("max_ball_speed")) init.max_ball_speed = json["max_ball_speed"].GetString();
    if (json.HasMember("ball_shape")) init.ball_shape = json["ball_shape"].GetString();
    if (json.HasMember("ball_weight")) init.ball_weight = json["ball_weight"].GetString();
    if (json.HasMember("ball_size")) init.ball_size = json["ball_size"].GetString();
    if (json.HasMember("ball_bounciness")) init.ball_bounciness = json["ball_bounciness"].GetString();
    if (json.HasMember("boost")) init.boost = json["boost"].GetString();
    if (json.HasMember("boost_strength")) init.boost_strength = json["boost_strength"].GetString();
    if (json.HasMember("gravity")) init.gravity = json["gravity"].GetString();
    if (json.HasMember("demolition")) init.demolition = json["demolition"].GetString();
    if (json.HasMember("respawn")) init.respawn = json["respawn"].GetString();

    init.ball = deserialize< InitializeBall >(json["ball"]);

    const rapidjson::Value & cars = json["cars"];
    init.cars.reserve(cars.Size());
    for (uint32_t i = 0; i < cars.Size(); i++) {
      init.cars.push_back(deserialize< InitializeCar >(cars[i]));
    }

    const rapidjson::Value & pads = json["pads"];
    init.pads.reserve(pads.Size());
    for (uint32_t i = 0; i < pads.Size(); i++) {
      init.pads.push_back(deserialize< InitializeBoostPad >(pads[i]));
    }
  
    const rapidjson::Value & goals = json["goals"];
    init.goals.reserve(goals.Size());
    for (uint32_t i = 0; i < goals.Size(); i++) {
      init.goals.push_back(deserialize< InitializeGoal >(goals[i]));
    }

    return init;
  }

}

namespace Incoming {

  std::vector < Message > deserialize(const std::string & message) {

    rapidjson::Document json;
    rapidjson::ParseResult ok = json.Parse(message.data(), message.size());
    if (!ok) {
      std::cout << "JSON parse error:" << std::endl;
      std::cout << rapidjson::GetParseError_En(ok.Code()) << " " << ok.Offset() << std::endl;
      std::cout << message << std::endl;
    }

    std::vector < Message > packet;
    for (uint32_t i = 0; i < json.Size(); i++) {
      const rapidjson::Value & obj = json[i];
      if(obj.HasMember("type")) {
        std::string type = obj["type"].GetString();
        if (type == "Initialize") {
          packet.push_back(impl::deserialize<Initialize>(obj));
        } else if (type == "Update") {
          packet.push_back(impl::deserialize<Update>(obj));
        } else if (type == "Terminate") {
          packet.push_back(impl::deserialize<Terminate>(obj));
        } else if (type == "Custom") {
          packet.push_back(impl::deserialize<Custom>(obj));
        } else {
          // invalid messages are not added to the packet
        }
      }
    }

    return packet;
  }

}