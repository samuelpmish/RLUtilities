#include "messages/outgoing.h"

#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

#include <iostream>
#include <unordered_map>

namespace impl{

  using writer_type = rapidjson::Writer<rapidjson::StringBuffer>;

  template < typename T>
  void write(writer_type & writer, const T & value);

  template <>
  void write(writer_type & writer, const vec3 & value) {
    writer.StartArray();
    writer.Double(value[0]);
    writer.Double(value[1]);
    writer.Double(value[2]);
    writer.EndArray();
  } 

  template <>
  void write(writer_type & writer, const rotator & value) {
    writer.StartArray();
    writer.Double(value.roll);
    writer.Double(value.pitch);
    writer.Double(value.yaw);
    writer.EndArray();
  } 

  template <>
  void write(writer_type & writer, const Input & controls) {
    writer.StartObject();

    writer.String("type");
    writer.String("PlayerInput");

    writer.String("steer");
    writer.Double(controls.steer);

    writer.String("throttle");
    writer.Double(controls.throttle);

    writer.String("roll");
    writer.Double(controls.roll);
    writer.String("pitch");
    writer.Double(controls.pitch);
    writer.String("yaw");
    writer.Double(controls.yaw);

    writer.String("jump");
    writer.Bool(controls.jump);
    writer.String("boost");
    writer.Bool(controls.boost);
    writer.String("handbrake");
    writer.Bool(controls.handbrake);
    writer.String("use_item");
    writer.Bool(controls.use_item);

    writer.EndObject();
  }

  template <>
  void write(writer_type & writer, const Loadout & loadout) {
    writer.StartObject();
    writer.String("type");
    writer.String("Loadout");

    writer.String("primary");
    writer.StartArray();
    writer.Int(loadout.primary.r);
    writer.Int(loadout.primary.g);
    writer.Int(loadout.primary.b);
    writer.EndArray();

    writer.String("secondary");
    writer.StartArray();
    writer.Int(loadout.primary.r);
    writer.Int(loadout.primary.g);
    writer.Int(loadout.primary.b);
    writer.EndArray();

    writer.String("car_id");
    writer.Int(loadout.car_id);
    writer.String("car_paint_id");
    writer.Int(loadout.car_paint_id);
    writer.String("decal_id");
    writer.Int(loadout.decal_id);
    writer.String("decal_paint_id");
    writer.Int(loadout.decal_paint_id);
    writer.String("wheels_id");
    writer.Int(loadout.wheels_id);
    writer.String("wheels_paint_id");
    writer.Int(loadout.wheels_paint_id);
    writer.String("boost_id");
    writer.Int(loadout.boost_id);
    writer.String("boost_paint_id");
    writer.Int(loadout.boost_paint_id);
    writer.String("antenna_id");
    writer.Int(loadout.antenna_id);
    writer.String("antenna_paint_id");
    writer.Int(loadout.antenna_paint_id);
    writer.String("hat_id");
    writer.Int(loadout.hat_id);
    writer.String("hat_paint_id");
    writer.Int(loadout.hat_paint_id);
    writer.String("custom_finish_id");
    writer.Int(loadout.custom_finish_id);
    writer.String("paint_finish_id");
    writer.Int(loadout.paint_finish_id);
    writer.String("engine_audio_id");
    writer.Int(loadout.engine_audio_id);
    writer.String("trails_id");
    writer.Int(loadout.trails_id);
    writer.String("trails_paint_id");
    writer.Int(loadout.trails_paint_id);
    writer.String("goal_explosion_id");
    writer.Int(loadout.goal_explosion_id);
    writer.String("goal_explosion_paint_id");
    writer.Int(loadout.goal_explosion_paint_id);

    writer.EndObject();
  }

  template <>
  void write(writer_type & writer, const MatchConfig & config) {
    writer.StartObject();
    writer.String("type");
    writer.String("MatchConfig");

    writer.String("map");
    writer.String(config.map.c_str());
    writer.String("mode");
    writer.String(config.mode.c_str());
    writer.String("playtest");
    writer.String(config.playtest.c_str());
    writer.String("match_length");
    writer.String(config.match_length.c_str());
    writer.String("max_score");
    writer.String(config.max_score.c_str());
    writer.String("overtime");
    writer.String(config.overtime.c_str());
    writer.String("series");
    writer.String(config.series.c_str());
    writer.String("speed");
    writer.String(config.speed.c_str());
    writer.String("max_ball_speed");
    writer.String(config.max_ball_speed.c_str());
    writer.String("ball_shape");
    writer.String(config.ball_shape.c_str());
    writer.String("ball_weight");
    writer.String(config.ball_weight.c_str());
    writer.String("ball_size");
    writer.String(config.ball_size.c_str());
    writer.String("ball_bounciness");
    writer.String(config.ball_bounciness.c_str());
    writer.String("boost");
    writer.String(config.boost.c_str());
    writer.String("boost_strength");
    writer.String(config.boost_strength.c_str());
    writer.String("gravity");
    writer.String(config.gravity.c_str());
    writer.String("demolition");
    writer.String(config.demolition.c_str());
    writer.String("respawn");
    writer.String(config.respawn.c_str());

    writer.String("force_restart");
    writer.Bool(config.force_restart);

    writer.String("num_players");
    writer.Int(config.num_players);

    writer.String("num_cars");
    writer.Int(config.num_cars);

    writer.String("start_when");
    writer.String(config.start_when.c_str());

    writer.EndObject();
  }

  template <>
  void write(writer_type & writer, const Ready & msg) {
    writer.StartObject();
    writer.String("type");
    writer.String("Ready");

    writer.String("id");
    writer.Int(msg.id);

    writer.String("name");
    writer.String(msg.name.c_str());

    writer.String("team");
    writer.Int(msg.team);

    writer.String("multiplicity");
    writer.Int(msg.multiplicity);

    writer.String("loadout");
    write(writer, msg.loadout);

    writer.EndObject();
  }

  template <>
  void write(writer_type & writer, const Custom & custom) {
    writer.StartObject();
    writer.String("type");
    writer.String("Custom");

    writer.String("index");
    writer.Int(custom.index);

    writer.String("contents");
    writer.String(custom.contents.c_str());
    writer.EndObject();
  }

  template <>
  void write(writer_type & writer, const SetCarState & car) {
    writer.StartObject();
    writer.String("type");
    writer.String("SetCarState");

    writer.String("id");
    writer.Int(car.id);

    writer.String("position");
    write(writer, car.position);

    writer.String("velocity");
    write(writer, car.velocity);

    writer.String("euler_angles");
    write(writer, car.euler_angles);

    writer.String("angular_velocity");
    write(writer, car.angular_velocity);

    writer.String("boost");
    writer.Int(car.boost);
    writer.EndObject();
  }

  template <>
  void write(writer_type & writer, const SetBallState & ball) {
    writer.StartObject();
    writer.String("type");
    writer.String("SetBallState");

    writer.String("position");
    write(writer, ball.position);

    writer.String("velocity");
    write(writer, ball.velocity);

    writer.String("euler_angles");
    write(writer, ball.euler_angles);

    writer.String("angular_velocity");
    write(writer, ball.angular_velocity);
    writer.EndObject();
  }

  template <>
  void write(writer_type & writer, const Polyline3D & line) {
    writer.StartObject();

    writer.String("color");
    writer.StartArray();
    writer.Int(line.color.r);
    writer.Int(line.color.g);
    writer.Int(line.color.b);
    writer.EndArray();

    writer.String("points");
    writer.StartArray();
    for (uint16_t i = 0; i < line.points.size(); i++) {
      write(writer, line.points[i]);
    }
    writer.EndArray();

    writer.EndObject();
  }

  template <>
  void write(writer_type & writer, const Text2D & draw_text) {
    writer.StartObject();

    writer.String("color");
    writer.StartArray();
    writer.Int(draw_text.color.r);
    writer.Int(draw_text.color.g);
    writer.Int(draw_text.color.b);
    writer.EndArray();

    writer.String("position");
    writer.StartArray();
    writer.Double(draw_text.position[0]);
    writer.Double(draw_text.position[1]);
    writer.EndArray();

    writer.String("scale");
    writer.Double(draw_text.scale);

    writer.String("text");
    writer.String(draw_text.text.c_str());

    writer.EndObject();
  }

  template <>
  void write(writer_type & writer, const Draw & draw) {
    writer.StartObject();
    writer.String("type");
    writer.String("Draw");

    writer.String("id");
    writer.Int(draw.id);

    if (draw.lines.size()) {
      writer.String("lines");
      writer.StartArray();
      for (auto & line : draw.lines) {
        write(writer, line);
      }
      writer.EndArray();
    }

    if (draw.text.size()) {
      writer.String("text");
      writer.StartArray();
      for (auto & text : draw.text) {
        write(writer, text);
      }
      writer.EndArray();
    }

    writer.EndObject();
  }


  template <>
  void write(writer_type & writer, const Clear & clear) {
    writer.StartObject();
    writer.String("type");
    writer.String("Clear");

    writer.String("id");
    writer.Int(clear.id);

    writer.EndObject();
  }

}


namespace Outgoing {

  std::string serialize(const std::vector < Message > & packet) {

    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    writer.StartArray();
    for (auto message : packet) {
      std::visit([&writer](auto message){ 
        impl::write(writer, message);
      }, message);
    }
    writer.EndArray();

    return sb.GetString();
  }

}