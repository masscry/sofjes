#include <random>
#include <cmath>
#include <unordered_map>
#include <world.h>
#include <stdexcept>
#include <iostream>
#include <vector>

int loop = 1;

sj::camera_t cam = {
	sj::matf_t{	
		{ 
			{ -1.0f, 0.0f },
			{ 0.0f, 1.0f } 
		} 
	},
	sj::vec2f_t{ 8.5f, 8.5f }
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

			if (sj::Cell(int(cam.pos.x + (walk * cam.view.t[0].x + stride * cam.view.t[1].x) * frameTime), int(cam.pos.y)) == 0)
			{
				cam.pos.x += (walk * cam.view.t[0].x + stride * cam.view.t[1].x) * frameTime;
			}
			if (sj::Cell(int(cam.pos.x), int(cam.pos.y + (walk * cam.view.t[0].y + stride * cam.view.t[1].y) * frameTime)) == 0)
			{
				cam.pos.y += (walk * cam.view.t[0].y + stride * cam.view.t[1].y) * frameTime;
			}

			cam.view = cam.view * sj::RotationMatrix(rotate*frameTime);

			sj::Render(cam, &wallDist);
			sj::RenderSprites(wallDist, cam, sprites, spritesTotal);
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

