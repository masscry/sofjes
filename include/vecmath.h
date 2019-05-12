#pragma once
#ifndef __SOFJES_VECMATH_HEADER__
#define __SOFJES_VECMATH_HEADER__

#include <cstdint>
#include <cmath>

namespace sj {

  template<typename T>
  union vec2_t {
    T v[2];
    struct {
      T x;
      T y;
    };

    vec2_t<T>& operator -= (vec2_t<T> b) {
      this->x -= b.x;
      this->y -= b.y;
      return *this;
    }

    vec2_t<T>& operator += (vec2_t<T> b) {
      this->x += b.x;
      this->y += b.y;
      return *this;
    }

  };

  template<typename T>
  union mat_t {
    T v[2][2];
    struct {
      vec2_t<T> t[2];
    };
  };

  template<typename T>
  T SqrDistance(vec2_t<T> a, vec2_t<T> b) {
    T dx = a.x - b.x;
    T dy = a.y - b.y;
    return dx*dx + dy*dy;
  }

  typedef vec2_t<float> vec2f_t;
  typedef vec2_t<int> vec2i_t;
  typedef vec2_t<uint32_t> vec2ui_t;
  typedef mat_t<float> matf_t;

  template<typename T>
  mat_t<T> operator * (mat_t<T> a, mat_t<T> b) {
    return mat_t<T> {
      {
        { a.v[0][0]*b.v[0][0] + a.v[0][1]*b.v[1][0], a.v[0][0]*b.v[0][1] + a.v[0][1]*b.v[1][1] },
        { a.v[1][0]*b.v[0][0] + a.v[1][1]*b.v[1][0], a.v[1][0]*b.v[0][1] + a.v[1][1]*b.v[1][1] }
      }
    };
  }

  template<typename T>
  vec2_t<T> operator * (mat_t<T> mat, vec2_t<T> vec) {
    return vec2_t<T>{
      vec.x*mat.v[0][0] + vec.y*mat.v[0][1],
      vec.x*mat.v[1][0] + vec.y*mat.v[1][1]
    };
  }

  template<typename T>
  vec2_t<T> operator * (vec2_t<T> a, T b) {
    return vec2_t<T>{ a.x * b, a.y * b };
  }

  template<typename T>
  vec2_t<T> operator + (vec2_t<T> a, vec2_t<T> b) {
    return vec2_t<T>{ a.x + b.x, a.y + b.y };
  }

  template<typename T>
  mat_t<T> RotationMatrix(T angle) {
    T cosa = std::cos(angle);
    T sina = std::sin(angle);
    return mat_t<T>{ { { cosa, -sina }, { sina, cosa } } };
  }

  template<typename T>
  T InverseDeterminant(mat_t<T> mat) {
    return ((T) 1.0) / (mat.v[1][0]*mat.v[0][1] - mat.v[0][0]*mat.v[1][1]);
  }

  template<typename T>
  mat_t<T> Inverse(mat_t<T> mat) {
    T invDet = InverseDeterminant(mat);

    return mat_t<T>{
      {
        { invDet*mat.v[0][1], -invDet*mat.v[0][0] },
        { -invDet*mat.v[1][1], invDet*mat.v[1][0] }
      }
    };
  }

}

#endif /* __SOFJES_VECMATH_HEADER__ */