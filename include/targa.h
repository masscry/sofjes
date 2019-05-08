/**
 * @file targa.h
 * @author timur
 *
 * TGA format load/save routines.
 *
 */

#pragma once
#ifndef __TARGA_HEADER__
#define __TARGA_HEADER__

#include <cstdint>
#include <texture_t.h>

namespace sj {
  
  enum tgaErrors {
    TGA_OK  = 0, /**< No errors */
    TGA_IO  = 1, /**< IO error  */
    TGA_SIG = 2
  };

  /**
   * Load tga image.
   *
   * @param input input file stream
   * @param img image to store pixels data
   *
   * @return non-zero on errors
   */
  texture_t tgaLoad(FILE* input);

}

#endif /* __TARGA_HEADER__ */
