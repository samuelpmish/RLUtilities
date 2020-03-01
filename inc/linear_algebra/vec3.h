#pragma once
#include "vec.h"

template <> 
class vec<3> {
public:
  float x;
  float y;
  float z;

  vec() = default;

  explicit vec(float value) {
    x = value;
    y = value;
    z = value;
  }

  vec(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  vec(std::initializer_list<float> args) {
    int i = 0;
    for (float arg : args) {
      this->operator()(i++) = arg;
      if (i > 3)
        break;
    }
  }

  template <int m> 
  vec(const vec<m> &other) {
    for (int i = 0; i < 3; i++) {
		this->operator()(i) = (i < m) ? other[i] : 0.0f;
    }
  }

  constexpr float &operator[](const size_t i) { 
	  return this->operator()(i); 
  }

  constexpr float operator[](const size_t i) const {
    return this->operator()(i);
  }

  constexpr float &operator()(const size_t i) {
    switch (i) {
    default:
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return z;
    }
  }

  constexpr float operator()(const size_t i) const {
    switch (i) {
    default:
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return z;
    }
  }

  // elementwise addition
  vec<3> operator+(const vec<3> &other) const {
    return vec<3>(x + other.x, 
                  y + other.y, 
                  z + other.z);
  }

  // elementwise multiplication
  vec<3> operator*(const vec<3> &other) const {
    return vec<3>(x * other.x, 
                  y * other.y, 
                  z * other.z);
  }

  // in-place elementwise addition
  vec<3> &operator+=(const vec<3> &other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  // unary minus
  vec<3> operator-() const { 
    return vec<3>(-x, -y, -z); 
  }

  // elementwise subtraction
  vec<3> operator-(const vec<3> &other) const {
    return vec<3>(x - other.x, 
                  y - other.y, 
                  z - other.z);
  }

  // in-place elementwise subtraction
  vec<3> &operator-=(const vec<3> &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  // in-place scalar addition
  vec<3> &operator+=(const float other) {
    x += other;
    y += other;
    z += other;
    return *this;
  }

  // in-place scalar multiplication
  vec<3> &operator*=(const float other) {
    x *= other;
    y *= other;
    z *= other;
    return *this;
  }

  // in-place scalar division
  vec<3> &operator/=(const float other) {
    x /= other;
    y /= other;
    z /= other;
    return *this;
  }
};

typedef vec<3> vec3;