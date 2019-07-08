#include "simulation/curve.h"

#include <algorithm>

#include "simulation/ogh.h"
#include "simulation/game.h"

#include "mechanics/drive.h"

#include "misc/io.h"

float maximize_speed_with_throttle(float accel, float v0, float sf);
float maximize_speed_without_throttle(float accel, float v0, float sf);

class CubicHermite {

 public:

  // starting point and tangent
  vec3 x0, t0;

  // ending point and tangents
  vec3 x1, t1;

  // length
  float L;

  vec3 operator()(float s) {
    float u = s / L;
    return x0 * (1.0f - u) * (1.0f - u) * (1.0f + 2.0f * u) + 
           t0 * L * (1.0f - u) * (1.0f - u) * u + 
           x1 * (3.0f - 2.0f * u) * u * u + 
           t1 * L * (u - 1.0f) * u * u;
  }

};

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

Curve::Curve(std::vector< ControlPoint > info,
      vec3 dx0, vec3 dt0, vec3 dx1, vec3 dt1, 
      vec3 start, vec3 end) {

  int ndiv = 16;
  size_t num_segments = info.size() - 1;

  points.reserve(ndiv * num_segments + 2);
  tangents.reserve(ndiv * num_segments + 2);

  std::vector < vec3 > normals;
  std::vector < int > segment_ids;

  normals.reserve(ndiv * num_segments + 2);
  segment_ids.reserve(ndiv * num_segments + 2);

  for (int i = 0; i < num_segments; i++) {

    vec3 P0 = info[i].p;
    vec3 P1 = info[i+1].p;

    vec3 V0 = info[i].t;
    vec3 V1 = info[i+1].t;

    vec3 N0 = info[i].n;
    vec3 N1 = info[i+1].n;

    OGH piece(P0, V0, P1, V1); 
    //CubicHermite piece{P0, V0, P1, V1, norm(P0 - P1)};

    int is_last = (i == num_segments - 1);

    for (int j = 0; j < (ndiv + is_last); j++) { 

      float t = float(j) / float(ndiv); 

      //vec3 p = piece(t * norm(P0 - P1));
      vec3 p = piece.evaluate(t);
      vec3 n = normalize((1.0f - t) * N0 + t * N1);
    
      if (dot(p - P0, N0) < 0.0f) {
        p -= dot(p - P0, N0) * N0;
      }
      
      if (dot(p - P1, N1) < 0.0f) {
        p -= dot(p - P1, N1) * N1;
      }
      
      points.push_back(p);
      normals.push_back(n);
      segment_ids.push_back(i);
    
    }

  }

  calculate_distances();

  CubicHermite correction{dx0, dt0, dx1, dt1, length};

  for (int i = 0; i < points.size(); i++) {
    vec3 n = normals[i];
    vec3 dx = correction(length - distances[i]);
    points[i] += dx - dot(dx, n) * n;
  }

  for (int i = 1; i < points.size()-1; i++) {

    float s0 = distances[i];
    vec3 p0 = points[i];
    vec3 n0 = normals[i];
    vec3 t0 = normalize(points[i+1] - points[i-1]);

    for (int j = points.size() - 2; j > i + 1; j--) {

      if (segment_ids[i] == segment_ids[j]) break;

      float s1 = distances[j];
      vec3 p1 = points[j];
      vec3 n1 = normals[j];
      vec3 t1 = normalize(points[j+1] - points[j-1]);

      float inplane = dot(n0, n1);
      float collinearity = dot(p1-p0, t0) / norm(p1-p0);
      float avg_curvature = dot(cross(t0, t1), n1) / (s1-s0);

      //if (inplane > 0.98f && 
      //    collinearity > 0.0 && 
      //    fabs(avg_curvature) < 0.00015f) {
      //  OGH replacement(p0, t0, p1, t1); 
      //  std::cout << "replacing from " << i << " to " << j << std::endl;
      //  for (int k = i; k < j; k++) { 
      //    float t = float(k - i) / float(j - i); 
      //    vec3 dx = replacement.evaluate(t) - points[k];
      //    points[k] += dx - dot(dx, n0) * n0;
      //  }
      //  i = j;
      //  break;
      //}

    }

  }

  if (norm(start - points.front()) > 1.0f) {
    points.insert(points.begin(), start);
    normals.insert(normals.begin(), normals[0]);
  }
  if (norm(end - points.back()) > 1.0f) {
    points.push_back(end);
    normals.push_back(normals.back());
  }

  calculate_distances();
  calculate_tangents();

  curvatures = std::vector< float >(points.size());
  size_t last = curvatures.size() - 1;

  vec3 m, n;

  // how much of the curvature estimate comes from curvature
  // along the car's up direction versus in the in the plane
  float in_plane_weight = 0.5f;

  // maximum out-of-plane curvature contribution
  float kappa_max = 0.004f;

  float kappa1, kappa2, ds;

  // treat the interior points with a center difference stencil
  for (size_t i = 1; i < last; i++) {
    m = cross(tangents[i+1], tangents[i-1]);
    n = normals[i];
    ds = (distances[i-1] - distances[i+1]);
    kappa1 = clip(asin(norm(m)) / ds, 0.0f, kappa_max);
    kappa2 = asin(fabs(dot(m, n))) / ds;
    curvatures[i] = lerp(kappa1, kappa2, in_plane_weight);
  }

  // the start and end points use a 1-sided stencil instead
  m = cross(tangents[1], tangents[0]);
  n = normals[0];
  ds = (distances[0] - distances[1]);
  kappa1 = clip(asin(norm(m)) / ds, 0.0f, kappa_max);
  kappa2 = asin(fabs(dot(m, n))) / ds;
  curvatures[0] = lerp(kappa1, kappa2, in_plane_weight);

  m = cross(tangents[last], tangents[last-1]);
  n = normals[last];
  ds = (distances[last-1] - distances[last]);
  kappa1 = clip(asin(norm(m)) / ds, 0.0f, kappa_max);
  kappa2 = asin(fabs(dot(m, n))) / ds;
  curvatures[last] = lerp(kappa1, kappa2, in_plane_weight);

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

  tangents[0] = normalize(-3.0f * points[0] + 4.0f * points[1] - points[2]);
  for (size_t i = 1; i < last; i++) {
    tangents[i] = normalize(points[i + 1] - points[i - 1]);
  }
  tangents[last] = normalize(3.0f * points[last] - 4.0f * points[last-1] + points[last-2]);

}

float Curve::calculate_max_speeds(float v0, float vf) {

  vec3 g{0.0f, 0.0f, Game::gravity};
  max_speeds = std::vector < float >(curvatures.size());

  for (int i = 0; i < curvatures.size(); i++) {
    max_speeds[i] = Drive::max_turning_speed(curvatures[i] * 1.1f);
  }

  max_speeds[0] = fminf(v0, max_speeds[0]);
  max_speeds[max_speeds.size() - 1] = fminf(vf, max_speeds[max_speeds.size() - 1]);

  for (int i = 1; i < curvatures.size(); i++) {
    float ds = distances[i-1] - distances[i]; 
    vec3 t = normalize(tangents[i] + tangents[i-1]);
    float attainable_speed = maximize_speed_with_throttle(
      0.9f * Drive::boost_accel + dot(g, t), max_speeds[i-1], ds);
    max_speeds[i] = fminf(max_speeds[i], attainable_speed);
  }

  float time = 0.0f;

  for (int i = int(curvatures.size() - 2); i >= 0; i--) {
    float ds = distances[i] - distances[i+1]; 
    vec3 t = normalize(tangents[i] + tangents[i+1]);
    float attainable_speed = maximize_speed_without_throttle(
      Drive::brake_accel - dot(g, t), max_speeds[i+1], ds);
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
