#include <inttypes.h>
#include <iostream>

#include <pybind11/buffer_info.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>
#include <bitset>
#include <algorithm>

#include "Car.h"
#include "linalg.h"

#include <omp.h>

#ifdef __linux__
// linux code goes here
#elif _WIN32
#include <intrin.h>
#endif

int offset_LUT[8][8] = {
    {0, 0, 1,  2,  3,  4,  5,  6},
    {0, 0, 7,  8,  9, 10, 11, 12},
    {0, 0, 0, 13, 14, 15, 16, 17},
    {0, 0, 0,  0, 18, 19, 20, 21},
    {0, 0, 0,  0,  0, 22, 23, 24},
    {0, 0, 0,  0,  0,  0, 25, 26},
    {0, 0, 0,  0,  0,  0,  0, 27},
    {0, 0, 0,  0,  0,  0,  0,  0}
};

int id_LUT[28][2] = {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7},
                     {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {2, 3},
                     {2, 4}, {2, 5}, {2, 6}, {2, 7}, {3, 4}, {3, 5}, {3, 6},
                     {3, 7}, {4, 5}, {4, 6}, {4, 7}, {5, 6}, {5, 7}, {6, 7}};

class CollisionReport {
 public:
  uint32_t mask_;
  uint32_t frame_;

  CollisionReport() {
    mask_ = 0;
    frame_ = 0;
  }

  void RecordCollision(int a, int b) {
    mask_ |= (uint32_t(1) << offset_LUT[a][b]);
  }

  int NumCollisions() {
#ifdef __linux__
    // linux code goes here
#elif _WIN32
    return __popcnt(mask_);
#endif
  }

  pybind11::tuple NextCollision() {
#ifdef __linux__
    // linux code goes here
#elif _WIN32
    int shift = 31 - __lzcnt(mask_);
    mask_ ^= (uint32_t(1) << shift);
    return pybind11::make_tuple(id_LUT[shift][0], id_LUT[shift][1]);
#endif
  }

  bool operator<(const CollisionReport & other) const {
    return frame_ < other.frame_;
  }
};

//pybind11::array_t<CollisionReport> detect_collisions_between_cars(
std::vector < CollisionReport > detect_collisions_between_cars(
    pybind11::array_t<float> info, int threads = 1) {
  pybind11::buffer_info info_buf = info.request();

  omp_set_num_threads(threads);

  float* fptr = reinterpret_cast<float*>(info_buf.ptr);

  std::vector<CollisionReport> report_list;

  auto stride = info_buf.shape[0];
  auto num_frames = info_buf.shape[1];
  int num_cars = 8;
  int fields_per_car = 7;

  #pragma omp parallel for
  for (int i = 0; i < num_frames; i++) {
    Car car;
    obb hitboxes[8];
    sphere spheres[8];

    CollisionReport report;

    for (int c = 0; c < num_cars; c++) {
      int offset = stride * i + c * fields_per_car;
      int type = static_cast<int>(fptr[offset + 0]);
      car.x[0] = fptr[offset + 1];
      car.x[1] = fptr[offset + 2];
      car.x[2] = fptr[offset + 3];
      car.o = euler_rotation(
          {fptr[offset + 4], fptr[offset + 5], fptr[offset + 6]});
      hitboxes[c] = car.hitbox();
      spheres[c].center = car.x;
      spheres[c].radius = 75.0f;
    }

    for (int a = 0; a < num_cars; a++) {
      for (int b = 0; b < a; b++) {
        if (intersect(spheres[a], spheres[b])) {
          if (intersect(hitboxes[a], hitboxes[b])) {
            report.RecordCollision(a, b);
          }
        }
      }
    }

    if (report.NumCollisions() > 0) {
      #pragma omp critical
      report_list.push_back(report);
    }
  }

  std::sort(report_list.begin(), report_list.end());

//  auto result = pybind11::array_t<CollisionReport>(report_list.size());
//  CollisionReport* result_ptr =
//      reinterpret_cast<CollisionReport*>(result.request().ptr);
//
//  std::copy(report_list.begin(), report_list.end(), result_ptr);

  return report_list;
}

PYBIND11_MODULE(DetectCollisions, m) {
  m.def("detect_collisions_between_cars", &detect_collisions_between_cars);

  pybind11::class_<CollisionReport>(m, "CollisionReport")
      .def(pybind11::init<>())
      .def_readwrite("mask", &CollisionReport::mask_)
      .def_readwrite("frame", &CollisionReport::frame_)
      .def("RecordCollision", &CollisionReport::RecordCollision)
      .def("NumCollisions", &CollisionReport::NumCollisions)
      .def("NextCollision", &CollisionReport::NextCollision);
}
