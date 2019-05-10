#pragma once
#ifndef __SOFJES_VECMATH_HEADER__
#define __SOFJES_VECMATH_HEADER__

#include <cstdint>

namespace sj {

  template<typename T>
  struct vec2_t {
    T x;
    T y;
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

}

#endif /* __SOFJES_VECMATH_HEADER__ */