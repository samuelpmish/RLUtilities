#include "misc/logging.h"

void to_json(nlohmann::json & obj, vec2 v) {
  obj = nlohmann::json{v[0], v[1]};
}

void to_json(nlohmann::json & obj, vec3 v) {
  obj = nlohmann::json{v[0], v[1], v[2]};
}

void to_json(nlohmann::json & obj, mat3 A) {
  obj = nlohmann::json{
    {A(0, 0), A(0, 1), A(0, 2)},
    {A(1, 0), A(1, 1), A(1, 2)}, 
    {A(2, 0), A(2, 1), A(2, 2)}
  };
}

template < typename T >
void to_json(nlohmann::json & obj, Interpolation<T> i) {
  obj = nlohmann::json{i.values};
}

nlohmann::json to_json(vec3 v) {
  return nlohmann::json{v[0], v[1], v[2]};
}



nlohmann::json to_json(Car c) {

  nlohmann::json obj;
  obj["x"] = c.position;
  obj["v"] = c.velocity;
  obj["w"] = c.angular_velocity;
  obj["o"] = c.orientation;
  obj["supersonic"] = c.supersonic;
  obj["jumped"] = c.jumped;
  obj["double_jumped"] = c.double_jumped;
  obj["on_ground"] = c.on_ground;
  obj["boost_left"] = c.boost;
  obj["jump_timer"] = c.jump_timer;
  obj["dodge_timer"] = c.dodge_timer;
  obj["dodge_dir"] = {c.dodge_dir[0], c.dodge_dir[1]};
  obj["time"] = c.time;
  return obj; 
  
}

nlohmann::json to_json(Ball b) {

  nlohmann::json obj;
  obj["x"] = b.position;
  obj["v"] = b.velocity;
  obj["w"] = b.angular_velocity;
  return obj;
}

nlohmann::json to_json(FollowPath f) {

  nlohmann::json obj;
  obj["arrival_time"] = f.arrival_time; 
  obj["arrival_speed"] = f.arrival_speed; 
  obj["plan"] = f.plan;
  return obj; 
  
}

nlohmann::json to_json(Curve c) {

  nlohmann::json obj;
  obj["points"] = c.points; 
  obj["tangents"] = c.tangents; 
  obj["distances"] = c.distances; 
  obj["curvatures"] = c.curvatures; 
  obj["max_speeds"] = c.max_speeds; 
  return obj;
  
}

nlohmann::json to_json(Input i) {

  nlohmann::json obj;
  obj["steer"] = i.steer;
  obj["roll"] = i.roll;
  obj["pitch"] = i.pitch;
  obj["yaw"] = i.yaw;
  obj["throttle"] = i.throttle;
  obj["jump"] = i.jump;
  obj["boost"] = i.boost;
  obj["handbrake"] = i.handbrake;
  return obj;

}

nlohmann::json to_json(Dodge d) {

  nlohmann::json obj;
  obj["type"] = "Dodge";
  obj["direction"] = d.direction;
  obj["preorientation"] = d.preorientation;
  obj["postorientation"] = d.postorientation;
  obj["jump_duration"] = d.jump_duration;
  obj["delay"] = d.delay;
  obj["timer"] = d.timer;
  obj["finished"] = d.finished;
  return obj;

}

nlohmann::json to_json(Aerial a) {
 
  mat3 o = a.target_orientation;

  nlohmann::json obj;
  obj["type"] = "Aerial";
  obj["arrival_time"] = a.arrival_time;
  obj["target_position"] = a.target_position;
  obj["target_orientation"] = a.target_orientation;
  obj["up"] = a.up;
  obj["angle_threshold"] = a.angle_threshold;
  obj["reorient_distance"] = a.reorient_distance;
  obj["throttle_distance"] = a.throttle_distance;
  return obj;
  
}
