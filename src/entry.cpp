#include <random>
#include <cmath>
#include <unordered_map>
#include <world.h>
#include <stdexcept>
#include <iostream>
#include <vector>

int loop = 1;

sj::vec2f_t pos = { 8.5f, 8.5f };

sj::matf_t xdir{ 
	{ 
		{ -1.0f, 0.0f },
		{ 0.0f, 1.0f } 
	} 
};

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

			float rotate = (float)(KEYS[SDLK_e] - KEYS[SDLK_q]);
			float walk = (float)(KEYS[SDLK_w] - KEYS[SDLK_s]);
			float stride = (float)(KEYS[SDLK_d] - KEYS[SDLK_a]);

			if (sj::Cell(int(pos.x + (walk * xdir.t[0].x + stride * xdir.t[1].x) * frameTime), int(pos.y)) == 0)
			{
				pos.x += (walk * xdir.t[0].x + stride * xdir.t[1].x) * frameTime;
			}
			if (sj::Cell(int(pos.x), int(pos.y + (walk * xdir.t[0].y + stride * xdir.t[1].y) * frameTime)) == 0)
			{
				pos.y += (walk * xdir.t[0].y + stride * xdir.t[1].y) * frameTime;
			}

			xdir = xdir * sj::RotationMatrix(rotate*frameTime);

			sj::Render(pos, xdir, &wallDist);
			sj::RenderSprites(wallDist, pos, xdir, sprites, spritesTotal);
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

