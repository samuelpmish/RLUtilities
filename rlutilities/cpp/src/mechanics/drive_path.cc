#include "mechanics/drive_path.h"

#include "misc/io.h"

#include <math.h>
#include <string>
#include <fstream>
#include <algorithm>

std::array < int, 3 > read_parameters(std::string);

float DrivePath::scale = -1.0f;
int DrivePath::nx = -1;
int DrivePath::ntheta = -1;
int DrivePath::nv = -1;

std::array < int, 3 > DrivePath::strides;
std::vector < float > DrivePath::time_LUT;
std::vector < uint32_t > DrivePath::path_LUT;

void DrivePath::read_files(std::string directory) {
  strides = read_parameters(directory + std::string("assets/parameters.txt"));
  time_LUT = read_binary < float >(directory + std::string("assets/times_planar.bin"));
  path_LUT = read_binary < uint32_t >(directory + std::string("assets/paths_planar.bin"));
}

DrivePath::DrivePath(Car & c) : car(c), drive(c), dodge(c), dash(c) {
  finished = false;
  controls = Input();

  target = vec3{NAN, NAN, NAN};
  arrival_time = NAN;
  arrival_speed = NAN;
  arrival_tangent = vec3{NAN, NAN, NAN};
}

void DrivePath::step(float dt) {

  float t_react = 0.4f;

  float speed = dot(car.v, car.forward());

  float s = path.find_nearest(car.x);
  float s_ahead = s - fmaxf(norm(car.v), 500.0f) * t_react;

  drive.target = path.point_at(s_ahead);
  
  if (isnormal(arrival_time)) {

    float T_min = 0.008f;
    float T = fmaxf(arrival_time - car.time, T_min);

    if (isnormal(arrival_speed)) {

      drive.speed = determine_speed_plan(s, T, dt);

    } else {

      drive.speed = s / T;

    }

    finished = (T <= T_min);

  } else {

    if (isnormal(arrival_speed)) {

      drive.speed = arrival_speed;

    } else {

      drive.speed = Drive::max_throttle_speed;

    }

    finished = (s <= 50.0f);

  }

  drive.speed = fminf(drive.speed, path.max_speed_at(s - 4.0f * speed * dt));

  drive.step(dt);
  controls = drive.controls;

}

// evaluates the quadratic function v(t)
// satisfying v(0) == v0, v(T) == vT, v'(T) == aT
float interpolate_quadratic(float v0, float vT, float aT, float t, float T) {
  float tau = t / T;
  float dv = aT * T;
  return v0 * (tau - 1.0f) * (tau - 1.0f) +
         dv * (tau - 1.0f) * tau +
         vT * (2.0f - tau) * tau;
}

float DrivePath::distance_error(float s0, float T, float dt, float v0, float vT, float aT) {

  int num_steps = int(T / dt);
  float s = s0;
  float v = v0;
  float v_prev = v0;

  for (int i = 0; i < num_steps; i++) {

    float t = (float(i) / float(num_steps)) * T;
    float v_ideal = interpolate_quadratic(v0, vT, aT, t, T);
    float v_limit = path.max_speed_at(s - v_prev * dt);

    v_prev = v;
    v = fminf(v_ideal, v_limit);

    s -= 0.5f * (v + v_prev) * dt;

  }

  return s;

}

float DrivePath::determine_speed_plan(float s, float T, float dt) {

  float v0 = norm(car.v);
  float vf = arrival_speed;

  float a = Drive::boost_accel + Drive::throttle_accel(vf);
  float error = distance_error(s, T, dt, v0, vf, a);

  float a_old = -Drive::brake_accel;
  float error_old = distance_error(s, T, dt, v0, vf, a_old);

  // try to find the right arrival acceleration
  // using a few iterations of secant method
  for (int i = 0; i < 16; i++) {

    if (fabs(error) < 0.5f) break;

    float new_a = (a_old * error - a * error_old) / (error - error_old);

    a_old = a;
    a = new_a;

    error_old = error;
    error = distance_error(s, T, dt, v0, vf, a);

  }

  arrival_accel = a;
  expected_error = error;

  expected_speed = interpolate_quadratic(v0, vf, a, 2 * dt, T);

  return expected_speed;

}

float DrivePath::recalculate_path() {

  const float k = ntheta / 6.28318530f;

  mat3 orientation = look_at(xy(car.forward()), vec3{0.0f, 0.0f, 1.0f});

  vec3 target_local = dot(target - car.x, orientation);
  vec3 tangent_local;

  int x = clip(int(roundf(target_local[0] / scale)), -nx, nx);
  int y = clip(int(roundf(target_local[1] / scale)), -nx, nx);

  // Careful with angular intervals
  int theta_min = 0;
  int theta_max = ntheta-1;

  if (isnormal(norm(arrival_tangent))) {
    tangent_local = dot(arrival_tangent, car.o);
    float angle = atan2(tangent_local[1], tangent_local[0]);
    int theta = int(roundf(k * angle));
    if (theta < 0) theta += ntheta;

    theta_min = theta_max = theta;
  }

  int v_min = 0;
  int v_max = nv-1;

  if (isnormal(arrival_speed)) {
    v_min = clip(int(floor(arrival_speed / 100.0f)), 0, nv-1);
  }

  int s_v = clip(int(floor(norm(car.v) / 100.0f)), 0, nv-1);

  int theta, v;
  float best_time = 100.0f;

  const std::vector < float > & times = time_LUT;
  const std::vector < uint32_t > & paths = path_LUT;
  for (int i = theta_min; i <= theta_max; i++) {
    for (int j = v_min; j <= v_max; j++) {

      if (times[to_id(x, y, i, j)] < best_time) {
        best_time = times[to_id(x, y, i, j)];
        theta = i;
        v = j;
      }

    }
  }

  std::vector < float > c(ntheta);
  std::vector < float > s(ntheta);
  for (int i = 0; i < ntheta; i++) {
    c[i] = cos(i / k);
    s[i] = sin(i / k);
  }

  std::vector < ControlPoint > nodes;

  for (int i = 0; i < 16; i++) {

    vec3 p = dot(orientation, vec3{x * scale, y * scale, 0.0f}) + car.x;
    vec3 t = dot(orientation, vec3{c[theta], s[theta], 0.0f});
    vec3 n = vec3{0.0f, 0.0f, 1.0f};

    if (i == 0) {
      p = target;
      if (isnormal(norm(arrival_tangent))) {
        t = arrival_tangent;
      }
    }

    nodes.push_back(ControlPoint{p, t, n});

    auto pack = from_id(paths[to_id(x, y, theta, v)]);
    x = pack[0]; y = pack[1]; theta = pack[2]; v = pack[3];

    if (x == 0 && y == 0 && theta == 0) break;

  }
  nodes.push_back(ControlPoint{car.x, car.forward(), vec3{0.0f, 0.0f, 1.0f}});

  std::reverse(nodes.begin(), nodes.end());

  path = Curve(nodes);

  path.calculate_max_speeds(Drive::max_speed, Drive::max_speed);

  return best_time;

};

uint32_t DrivePath::to_id(int x, int y, int theta, int v) {
  return (x + nx) * strides[0] + (y + nx) * strides[1] + theta * strides[2] + v;
}

std::array < int16_t, 4 > DrivePath::from_id(uint32_t id) {
  int16_t x     = int16_t((id / strides[0]) - nx); id %= strides[0];
  int16_t y     = int16_t((id / strides[1]) - nx); id %= strides[1];
  int16_t theta = int16_t(id / strides[2]);        id %= strides[2];
  int16_t v     = int16_t(id);

  return {x, y, theta, v};
}

std::array < int, 3 > read_parameters(std::string filename) {

  std::ifstream infile(filename);

  if (infile) {

    infile >> DrivePath::scale;
    infile >> DrivePath::nx;
    infile >> DrivePath::ntheta;
    infile >> DrivePath::nv;

    // TODO: read velocities

  } else {
    std::cout << "file not found: " << filename << std::endl;
  }

  infile.close();

  return { 
    DrivePath::nv * DrivePath::ntheta * (2 * DrivePath::nx + 1),
    DrivePath::nv * DrivePath::ntheta,
    DrivePath::nv
  };

}

float DrivePath::accel_buffer() {
  return drive.acceleration;  
};

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_drivepath(pybind11::module & m) {
  pybind11::class_<DrivePath>(m, "DrivePath")
    .def(pybind11::init<Car &>())
    .def_readwrite("target", &DrivePath::target)
    .def_readwrite("arrival_time", &DrivePath::arrival_time)
    .def_readwrite("arrival_speed", &DrivePath::arrival_speed)
    .def_readwrite("arrival_tangent", &DrivePath::arrival_tangent)
    .def_readwrite("path", &DrivePath::path)
    .def_readonly("finished", &DrivePath::finished)
    .def_readonly("controls", &DrivePath::controls)
    .def_readwrite("expected_error", &DrivePath::expected_error)
    .def_readwrite("expected_speed", &DrivePath::expected_speed)
    .def_readwrite("arrival_accel", &DrivePath::arrival_accel)
    .def("accel_buffer", &DrivePath::accel_buffer)
    .def("step", &DrivePath::step)
    .def("recalculate_path", &DrivePath::recalculate_path);
}
#endif
