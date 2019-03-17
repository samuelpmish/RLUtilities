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
  length = 0.0f;
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
  const float phi_max = 0.15f;

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

#if 0
    vec3 c1 = info[i].p;
    vec3 c4 = info[i+1].p;

    vec3 t1 = info[i].t;
    vec3 t2 = info[i+1].t;

    vec3 n1 = info[i].n;
    vec3 n2 = info[i+1].n;
  
    vec3 n_avg = normalize(n1 + n2);
    vec3 t1_avg = normalize(t1 - dot(t1, n_avg) * n_avg);
    vec3 t2_avg = normalize(t2 - dot(t2, n_avg) * n_avg);
  
    float phi = acos(clip(dot(t1_avg, t2_avg), -1.0f, 1.0f));
    float t_scale = ((1.0f / 3.0f) + (phi * phi) / 18.0f) * norm(c4 - c1);
  
    vec3 c2 = c1 + t_scale * t1;
    vec3 c3 = c4 - t_scale * t2;
#else
    vec3 P0 = info[i].p;
    vec3 P1 = info[i+1].p;

    vec3 V0 = info[i].t;
    vec3 V1 = info[i+1].t;

    vec3 N0 = info[i].n;
    vec3 N1 = info[i+1].n;

    vec3 dP = P1 - P0;
    float V0dotV1 = dot(V0, V1);
    float denom = 4.0f - V0dotV1 * V0dotV1;
  
    float a0 = (6.0f * dot(dP, V0) - 3.0f * dot(dP, V1) * V0dotV1) / denom;
    float a1 = (6.0f * dot(dP, V1) - 3.0f * dot(dP, V0) * V0dotV1) / denom;
#endif

    int is_last = (i == num_segments - 1);

    for (int j = 0; j < (ndiv + is_last); j++) { 

      float t = float(j) / float(ndiv); 

      vec3 g = ((2.0f * t + 1.0f) * (t - 1.0f) * (t - 1.0f)) * P0 +
               ((t - 1.0f) * (t - 1.0f) * t) * a0 * V0 +
               ((3.0f - 2.0f * t) * t * t) * P1 +
               ((t - 1.0f) * t * t) * a1 * V1;

      vec3 dg = (6.0f * t * t - 6.0f * t) * P0 + 
                (1.0f - 4.0f * t + 3.0f * t * t) * a0 * V0 +
                (6.0f * t - 6.0f * t * t) * P1 + 
                (3.0f * t * t - 2.0f * t) * a1 * V1;

      vec3 d2g = (12.0f * t - 6.0f) * P0 + 
                 ( 6.0f * t - 4.0f) * a0 * V0 - 
                 (12.0f * t - 6.0f) * P1 + 
                 ( 6.0f * t - 2.0f) * a1 * V1;

      float kappa = dot(cross(dg, d2g), normalize((1.0f - t) * N0 + t * N1)) / 
                             (norm(dg) * norm(dg) * norm(dg));
#if 0
      vec3 g = c1 * (v * v * v) +
               c2 * (v * v * u * 3.0f) + 
               c3 * (v * u * u * 3.0f) +
               c4 * (u * u * u);

      vec3 dg = c1 * (-3.0f * v * v) +
                c2 * ( 3.0f * v * v - 6.0f * u * v) +
                c3 * (-3.0f * u * u + 6.0f * u * v) +
                c4 * ( 3.0f * u * u);

      vec3 d2g = c1 * (6.0f * v) +
                 c2 * (6.0f * u - 12.0f * v) + 
                 c3 * (6.0f * v - 12.0f * u) +
                 c4 * (6.0f * u);

      float kappa = dot(cross(dg, d2g), normalize(v * n1 + u * n2)) / 
                            (norm(dg) * norm(dg) * norm(dg));
#endif
    
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

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
void init_curve(pybind11::module & m) {
	pybind11::class_<ControlPoint>(m, "ControlPoint")
		.def(pybind11::init())
		.def(pybind11::init< vec<3>, vec<3>, vec<3> >())
		.def_readwrite("p", &ControlPoint::p)
		.def_readwrite("t", &ControlPoint::t)
		.def_readwrite("n", &ControlPoint::n);

  pybind11::class_<Curve>(m, "Curve")
    .def(pybind11::init< std::vector < vec<3> > >())
    .def(pybind11::init< std::vector < ControlPoint > >())
    .def("point_at", &Curve::point_at)
    .def("tangent_at", &Curve::tangent_at)
    .def("curvature_at", &Curve::curvature_at)
    .def("max_speed_at", &Curve::max_speed_at)
    .def("find_nearest", &Curve::find_nearest)
    .def("pop_front", &Curve::pop_front)
    .def("calculate_distances", &Curve::calculate_distances)
    .def("calculate_tangents", &Curve::calculate_tangents)
    .def("calculate_max_speeds", &Curve::calculate_max_speeds)
    .def("write_to_file", &Curve::write_to_file)
    .def_readwrite("points", &Curve::points)
    .def_readonly("length", &Curve::length);
}
#endif
