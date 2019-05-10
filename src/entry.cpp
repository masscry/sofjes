#include <random>
#include <cmath>
#include <unordered_map>
#include <world.h>
#include <stdexcept>
#include <iostream>
#include <vector>

int loop = 1;

sj::vec2f_t pos = { 8.5f, 8.5f };
sj::vec2f_t dir = { -1.0f,  0.0f };
sj::vec2f_t perdir = { 0.0f, 1.0f  };
sj::vec2f_t plane = { 0.0f, 0.66f };

std::unordered_map<SDL_Keycode, bool> KEYS;

const uint32_t spritesTotal = 1;

sj::sprite_t sprites[spritesTotal] = {
	{5.5, 5.5, 10},
};

int main(int argc, char* argv[]) {

	try {

		sj::CreateWindow();
		std::vector<float> wallDist;
		wallDist.resize(sj::WIN_WIDTH/4);

		while (loop != 0) {
			SDL_Event evt;

			while (SDL_PollEvent(&evt) != 0)
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

			float frameTime = sj::FrameTime();

			float rotate = (float)(KEYS[SDLK_q] - KEYS[SDLK_e]);
			float walk = (float)(KEYS[SDLK_w] - KEYS[SDLK_s]);
			float stride = (float)(KEYS[SDLK_d] - KEYS[SDLK_a]);

			if (sj::Cell(int(pos.x + (walk * dir.x + stride * perdir.x) * frameTime), int(pos.y)) == 0)
			{
				pos.x += (walk * dir.x + stride * perdir.x) * frameTime;
			}
			if (sj::Cell(int(pos.x), int(pos.y + (walk * dir.y + stride * perdir.y) * frameTime)) == 0)
			{
				pos.y += (walk * dir.y + stride * perdir.y) * frameTime;
			}

			float oldDirX = dir.x;

			dir.x = dir.x * cos(rotate * frameTime) - dir.y * sin(rotate * frameTime);
			dir.y = oldDirX * sin(rotate * frameTime) + dir.y * cos(rotate * frameTime);

			perdir.x = dir.y;
			perdir.y = -dir.x;

			float oldPlaneX = plane.x;

			plane.x = plane.x * std::cos(rotate * frameTime) - plane.y * std::sin(rotate * frameTime);
			plane.y = oldPlaneX * std::sin(rotate * frameTime) + plane.y * std::cos(rotate * frameTime);

			sj::Render(pos, dir, plane, &wallDist);
			sj::RenderSprites(wallDist, pos, dir, plane, sprites, spritesTotal);
			sj::UpdateWindow(frameTime);
		}
	}
	catch(std::runtime_error& err) {
		std::cerr << "Runtime Error:" << std::endl;
		std::cerr << "\t" <<err.what() << std::endl;
	}

	sj::CloseWindow();

	return 0;
}

