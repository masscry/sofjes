#pragma once
#ifndef __SOFJES_WORLD_HEADER__
#define __SOFJES_WORLD_HEADER__

#include <stdint.h>
#include <SDL2/SDL.h>

namespace sj {

  const uint32_t MAP_WIDTH  = 24;
  const uint32_t MAP_HEIGHT = 24;
  
  const uint32_t WIN_WIDTH  = 640;
  const uint32_t WIN_HEIGHT = 480;

  int Cell(uint32_t x, uint32_t y);

  SDL_Surface* CreateWindow();
  void UpdateWindow();
  void CloseWindow();
  
  inline void PutPixel(SDL_Surface* scr, int y, int x, Uint8 r, Uint8 g, Uint8 b)
  {
    Uint32 *buffer = (Uint32*)((Uint8 *)scr->pixels + y * scr->pitch + x * 4);
    *buffer = SDL_MapRGB(scr->format, r, g, b);
  }

  struct vec2f_t {
    float x;
    float y;
  };

  void Render(vec2f_t pos, vec2f_t dir, vec2f_t plane);

  struct brick_t {
    uint32_t width;
    uint32_t height;
    uint32_t bytes_per_pixel; 
    uint8_t  pixel_data[16 * 16 * 3 + 1];
  };

  extern const brick_t brick;

}

#endif /* __SOFJES_WORLD_HEADER__ */