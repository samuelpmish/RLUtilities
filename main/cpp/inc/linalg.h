#pragma once

#include <math.h>

#include "vec.h"
#include "mat.h"

template < int m, int n >
inline vec < n > dot(const vec < m > & v,
                     const mat < m, n > & A) {

  vec < n > vA;

  for(int i = 0; i < n; i++){
    vA(i) = 0;
    for(int j = 0; j < m; j++){
      vA(i) += v(j) * A(j,i);
    }
  }

  return vA;

}

template < int m, int n >
inline vec < m > dot(const mat < m, n > & A,
                     const vec < n > & v) {

  vec < m > Av;

  for(int i = 0; i < m; i++){
    Av(i) = 0;
    for(int j = 0; j < n; j++){
      Av(i) += A(i,j) * v(j);
    }
  }

  return Av;

}

template < int m, int n >
inline float dot(const vec < m > & u,
                 const mat < m, n > & A,
                 const vec < n > & v) {

  float uAv = 0;

  for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++){
      uAv += u(i) * A(i,j) * v(j);
    }
  }

  return uAv;

}

inline float sgn(float x) {
  return float((0.0f < x) - (x < 0.0f));
}

inline float clip(float x, float minimum, float maximum) {
  return fmax(fmin(x, maximum), minimum);
}

inline float angle_between(const vec < 2 > & a, const vec < 2 > & b) {
  return acos(clip(dot(normalize(a), normalize(b)), -1.0f, 1.0f));
}

inline float angle_between(const vec < 3 > & a, const vec < 3 > & b) {
  return acos(clip(dot(normalize(a), normalize(b)), -1.0f, 1.0f));
}

// angle between proper orthogonal matrices
inline float angle_between(const mat < 3, 3 > & U, const mat < 3, 3 > & V) {
  return acos(0.5f * (tr(dot(U, transpose(V))) - 1.0f));
}

inline vec < 3 > xy(const vec < 3 > & v) {
  return vec < 3 >{v[0], v[1], 0.0f};
}

inline mat < 2, 2 > rotation(const float theta) {
  return mat< 2, 2 >{
    {cos(theta), -sin(theta)},
    {sin(theta), cos(theta)}
  };
}

inline mat < 3, 3 > axis_rotation(const vec < 3 > & omega) {

  float theta = norm(omega);

  if (fabs(theta) < 0.000001f) {

    return eye< 3 >();

  } else {

    vec3 axis = normalize(omega);

    mat < 3, 3 > K = {
      {  0.0f  , -axis[2],  axis[1]},
      { axis[2],    0.0f , -axis[0]},
      {-axis[1],  axis[0],    0.0f }
    };

    return eye< 3 >() + sin(theta) * K + (1.0f - cos(theta)) * dot(K, K);

  }

}

inline vec < 3 > rotation_to_axis(const mat < 3, 3 > & R) {

  float theta = acos(clip(0.5f * (tr(R) - 1.0f), -1.0f, 1.0f));

  float scale;

  // for small angles, prefer series expansion to division by sin(theta) ~ 0
  if (fabs(theta) < 0.00001f) {
    scale = 0.5f + theta * theta / 12.0f;
  } else {
    scale = 0.5f * theta / sin(theta);
  }

  return vec3{R(2,1)-R(1,2), R(0,2)-R(2,0), R(1,0)-R(0,1)} * scale;

}

inline mat < 3, 3 > look_at(const vec < 3 > & direction, const vec < 3 > & up = vec3{0.0f, 0.0f, 1.0f}) {
  vec3 f = normalize(direction);  
  vec3 u = normalize(cross(f, cross(up, f)));
  vec3 l = normalize(cross(u, f));

  return mat3 {
    {f[0], l[0], u[0]},
    {f[1], l[1], u[1]},
    {f[2], l[2], u[2]}
  };
}

template < typename T >
inline T lerp(const T & a, const T & b, float t) {
  return a * (1.0f - t) + b * t;
}
