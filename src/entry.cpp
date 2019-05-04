#include <SDL2/SDL.h>
#include <random>
#include <cmath>
#include <unordered_map>
#include <world.h>

int loop = 1;

sj::vec2f_t pos = {22.0f, 11.5f};
sj::vec2f_t dir = {-1.0f,  0.0f};
sj::vec2f_t plane = {0.0f, 0.66f};

std::unordered_map<SDL_Keycode, bool> KEYS;

int main(int argc, char* argv[]) {

  SDL_Surface* scr = sj::CreateWindow();

  while(loop != 0){
    SDL_Event evt;

    while(SDL_PollEvent(&evt) != 0)
    {
      switch (evt.type)
      {
        case SDL_KEYDOWN:
          KEYS[evt.key.keysym.sym] = true;
          break;
        case SDL_KEYUP:
          KEYS[evt.key.keysym.sym] = false;
          break;
        case SDL_QUIT:
          loop = 0;
          break;
      }
    }

    float rotate = KEYS[SDLK_LEFT] - KEYS[SDLK_RIGHT];
    float walk = KEYS[SDLK_UP] - KEYS[SDLK_DOWN];

    if(sj::Cell(int(pos.x + walk * dir.x * 0.01),int(pos.y)) == 0)
    {
      pos.x += walk * dir.x * 0.01;
    }
    
    if(sj::Cell(int(pos.x),int(pos.y + walk * dir.y * 0.01)) == 0)
    {
      pos.y += walk * dir.y * 0.01;
    }

    float oldDirX = dir.x;
    
    dir.x = dir.x * cos(rotate*0.01) - dir.y * sin(rotate*0.01);
    dir.y = oldDirX * sin(rotate*0.01) + dir.y * cos(rotate*0.01);
    
    double oldPlaneX = plane.x;
    
    plane.x = plane.x * cos(rotate*0.01) - plane.y * sin(rotate*0.01);
    plane.y = oldPlaneX * sin(rotate*0.01) + plane.y * cos(rotate*0.01);

    sj::Render(pos, dir, plane);
    sj::UpdateWindow();
  }

  sj::CloseWindow();

  return 0;
}

