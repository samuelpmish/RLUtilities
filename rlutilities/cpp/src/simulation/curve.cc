#include "simulation/ogh.h"
#include "simulation/curve.h"

#include "mechanics/drive.h"

#include "misc/io.h"

float maximize_speed_with_throttle(float accel, float v0, float sf);
float maximize_speed_without_throttle(float accel, float v0, float sf);

void Curve::write_to_file(std::string prefix) {
  write(points, prefix + std::string("_points.txt"));
  write(tangents, prefix + std::string("_tangents.txt"));
  write(curvatures, prefix + std::string("_curvatures.txt"));
  write(distances, prefix + std::string("_distances.txt"));
  write(max_speeds, prefix + std::string("_max_speeds.txt"));
}

Curve::Curve() {
  length = -1.0f;
  points = std::vector < vec3 >();
  tangents = std::vector < vec3 >();
  distances = std::vector < float >();
}

Curve::Curve(const std::vector<vec3> &_points) {
  points = _points;
  calculate_distances();
  calculate_tangents();
}

Curve::Curve(std::vector< ControlPoint > info) {

  // the maximum smoothing correction (radians)
  // const float phi_max = 0.15f;

  int ndiv = 16;
  size_t num_segments = info.size() - 1;

  points.reserve(ndiv * num_segments + 2);
  tangents.reserve(ndiv * num_segments + 2);
  curvatures.reserve(ndiv * num_segments + 2);

  // apply some smoothing, since the paths that come
  // from the LUT contain some wiggling as a consequence
  // of coarse angular discretization
  for (int i = 1; i < num_segments-1; i++) {

    vec3 delta_before = normalize(info[i].p - info[i-1].p);
    vec3 delta_after = normalize(info[i+1].p - info[i].p);  

    float phi_before = asin(dot(cross(info[i].t, delta_before), info[i].n));
    float phi_after = asin(dot(cross(info[i].t, delta_after), info[i].n));

    // if reasonable, apply a small rotation to control point 
    // tangents to smooth out unnecessary extrema
    if (phi_before * phi_after > 0.0f) {

      float phi;
      if (fabs(phi_before) < fabs(phi_after)) {
        phi = phi_before;
      } else {
        phi = phi_after;
      }

      info[i].t = dot(axis_to_rotation(info[i].n * phi), info[i].t);

    }

  }

  for (int i = 0; i < num_segments; i++) {

    vec3 P0 = info[i].p;
    vec3 P1 = info[i+1].p;

    vec3 V0 = info[i].t;
    vec3 V1 = info[i+1].t;

    vec3 N0 = info[i].n;
    vec3 N1 = info[i+1].n;

    OGH piece(P0, V0, P1, V1); 

    int is_last = (i == num_segments - 1);

    for (int j = 0; j < (ndiv + is_last); j++) { 

      float t = float(j) / float(ndiv); 

      vec3 g = piece.evaluate(t);
      vec3 dg = piece.tangent(t);
      vec3 d2g = piece.acceleration(t);

      // In rocket league, the only curvature that we care about 
      // is the component along the surface normal
      float kappa = dot(cross(dg, d2g), normalize((1.0f - t) * N0 + t * N1)) / 
                             (norm(dg) * norm(dg) * norm(dg));
    
      points.push_back(g);
      tangents.push_back(normalize(dg));
      curvatures.push_back(kappa);
    
    }

  }

  calculate_distances();

}

vec3 Curve::point_at(float s) {
  s = clip(s, 0, distances[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    if (distances[i] >= s && s >= distances[i + 1]) {
      float u = (s - distances[i + 1]) / (distances[i] - distances[i + 1]);
      return lerp(points[i + 1], points[i], u);
    }
  }

  return vec3{ 0.0f, 0.0f, 0.0f };
}

vec3 Curve::tangent_at(float s) {
  s = clip(s, 0, distances[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    if (distances[i] >= s && s >= distances[i + 1]) {
      float u = (s - distances[i + 1]) / (distances[i] - distances[i + 1]);
      return normalize(lerp(tangents[i + 1], tangents[i], u));
    }
  }

  return vec3{ 0.0f, 0.0f, 0.0f };
}

float Curve::curvature_at(float s) {
  s = clip(s, 0, distances[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    if (distances[i] >= s && s >= distances[i + 1]) {
      float delta_theta = angle_between(tangents[i + 1], tangents[i]);
      float delta_s = distances[i] - distances[i + 1];
      return delta_theta / delta_s;
    }
  }

  return 0.0f;
}

float Curve::max_speed_at(float s) {
  s = clip(s, 0, distances[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    if (distances[i] >= s && s >= distances[i + 1]) {
      float u = (s - distances[i + 1]) / (distances[i] - distances[i + 1]);
      return lerp(max_speeds[i + 1], max_speeds[i], u);
    }
  }

  return 0.0f;
}

float Curve::find_nearest(const vec3 &c) {
  float s = length;
  float min_distance = norm(c - points[0]);

  for (int i = 0; i < (points.size() - 1); i++) {
    vec3 a = points[i];
    vec3 b = points[i + 1];

    float alpha = clip(dot(b - a, c - a) / dot(b - a, b - a), 0.0f, 1.0f);

    float distance = norm(c - (a + alpha * (b - a)));

    if (distance < min_distance) {
      min_distance = distance;
      s = lerp(distances[i], distances[i + 1], alpha);
    }
  }

  return s;
}

void Curve::pop_front() {
  if (points.size() > 0) {
    points.erase(points.begin());
    tangents.erase(tangents.begin());
    distances.erase(distances.begin());
    length = distances[0];
  }
}

void Curve::calculate_distances() {

  distances = std::vector<float>(points.size(), 0.0f);
  int last = int(points.size() - 1);

  // measure distances from the end of the curve
  for (int i = last - 1; i >= 0; i--) {
    distances[i] = distances[i + 1] + norm(points[i + 1] - points[i]);
  }

  length = distances[0];

}

void Curve::calculate_tangents() {

  tangents = std::vector<vec3>(points.size());
  size_t last = tangents.size() - 1;

  tangents[0] = normalize(points[1] - points[0]);
  for (size_t i = 1; i < last; i++) {
    tangents[i] = normalize(points[i + 1] - points[i - 1]);
  }
  tangents[last] = normalize(points[last] - points[last - 1]);

}

float Curve::calculate_max_speeds(float v0, float vf) {

  max_speeds = std::vector < float >(curvatures.size());

  for (int i = 0; i < curvatures.size(); i++) {
    max_speeds[i] = Drive::max_turning_speed(curvatures[i]);
  }

  max_speeds[0] = fminf(v0, max_speeds[0]);
  max_speeds[max_speeds.size() - 1] = fminf(vf, max_speeds[max_speeds.size() - 1]);

  for (int i = 1; i < curvatures.size(); i++) {
    float ds = distances[i-1] - distances[i]; 
    float attainable_speed = maximize_speed_with_throttle(
      Drive::boost_accel, max_speeds[i-1], ds);
    max_speeds[i] = fminf(max_speeds[i], attainable_speed);
  }

  float time = 0.0f;

  for (int i = int(curvatures.size() - 2); i >= 0; i--) {
    float ds = distances[i] - distances[i+1]; 
    float attainable_speed = maximize_speed_without_throttle(
      Drive::brake_accel, max_speeds[i+1], ds);
    max_speeds[i] = fminf(max_speeds[i], attainable_speed);
    time += ds / (0.5f * (max_speeds[i] + max_speeds[i+1]));
  }

  return time;

}

float maximize_speed_with_throttle(float accel, float v0, float sf) {
  float dt = 0.008333f;
  float s = 0.0f;
  float v = v0;
  for (int i = 0; i < 100; i++) {
    float dv = (Drive::throttle_accel(v) + accel) * dt;
    float ds = (v + 0.5f * dv) * dt;
    v += dv;
    s += ds;
    if (s > sf) {
      v -= (s - sf) * (dv / ds);
      break;
    }
  }
  return v;
}

float maximize_speed_without_throttle(float accel, float v0, float sf) {
  float dt = 0.008333f;
  float s = 0.0f;
  float v = v0;
  float dv = accel * dt;
  for (int i = 0; i < 100; i++) {
    float ds = (v + 0.5f * dv) * dt;
    v += dv;
    s += ds;
    if (s > sf) {
      v -= (s - sf) * (dv / ds);
      break;
    }
  }
  return v;
}
