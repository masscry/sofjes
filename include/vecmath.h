#pragma once
#ifndef __SOFJES_VECMATH_HEADER__
#define __SOFJES_VECMATH_HEADER__

namespace sj {

  template<typename T>
  struct vec2_t {
    T x;
    T y;
  };

  typedef vec2_t<float> vec2f_t;
  typedef vec2_t<int> vec2i_t;

}

#endif /* __SOFJES_VECMATH_HEADER__ */