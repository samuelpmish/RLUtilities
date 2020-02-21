#pragma once

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <math.h>

template <int n> 
class vec {
  float data[n];

public:
  vec() {}

  explicit vec(float value) {
    for (int i = 0; i < n; i++) {
      data[i] = value;
    }
  }

  vec(std::initializer_list<float> args) {
    int i = 0;
    for (float arg : args) {
      data[i++] = arg;
      if (i > n)
        break;
    }
  }

  template <int m> vec(const vec<m> &other) {
    for (int i = 0; i < n; i++) {
      data[i] = (i < m) ? other[i] : 0.0f;
    }
  }

  constexpr float &operator[](const size_t i) { return data[i]; }
  constexpr float operator[](const size_t i) const { return data[i]; }
  constexpr float &operator()(const size_t i) { return data[i]; }
  constexpr float operator()(const size_t i) const { return data[i]; }

  // elementwise addition
  vec<n> operator+(const vec<n> &other) const {
    vec<n> v;
    for (int i = 0; i < n; i++) {
      v[i] = data[i] + other[i];
    }
    return v;
  }

  // elementwise multiplication
  vec<n> operator*(const vec<n> &other) const {
    vec<n> v;
    for (int i = 0; i < n; i++) {
      v[i] = data[i] * other[i];
    }
    return v;
  }

  // in-place elementwise addition
  vec<n> &operator+=(const vec<n> &other) {
    for (int i = 0; i < n; i++) {
      data[i] += other[i];
    }
    return *this;
  }

  // unary minus
  vec<n> operator-() const {
    vec<n> v;
    for (int i = 0; i < n; i++) {
      v[i] = -data[i];
    }
    return v;
  }

  // elementwise subtraction
  vec<n> operator-(const vec<n> &other) const {
    vec<n> v;
    for (int i = 0; i < n; i++) {
      v[i] = data[i] - other[i];
    }
    return v;
  }

  // in-place elementwise subtraction
  vec<n> &operator-=(const vec<n> &other) {
    for (int i = 0; i < n; i++) {
      data[i] -= other[i];
    }
    return *this;
  }

  // in-place scalar addition
  vec<n> &operator+=(const float other) {
    for (int i = 0; i < n; i++) {
      data[i] += other;
    }
    return *this;
  }

  // in-place scalar multiplication
  vec<n> &operator*=(const float other) {
    for (int i = 0; i < n; i++) {
      data[i] *= other;
    }
    return *this;
  }

  // in-place scalar division
  vec<n> &operator/=(const float other) {
    for (int i = 0; i < n; i++) {
      data[i] /= other;
    }
    return *this;
  }
};

typedef vec<4> vec4;