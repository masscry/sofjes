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
				case SDL_QUIT:
					loop = 0;
					break;
				}
			}

			const uint8_t* KEYS = SDL_GetKeyboardState(0);

			float frameTime = sj::FrameTime();

			float rotate = (float)(KEYS[SDL_SCANCODE_E] - KEYS[SDL_SCANCODE_Q]);
			float walk = (float)(KEYS[SDL_SCANCODE_W] - KEYS[SDL_SCANCODE_S]);
			float stride = (float)(KEYS[SDL_SCANCODE_D] - KEYS[SDL_SCANCODE_A]);

			sj::vec2f_t newPos = cam.pos + (cam.view.t[0]*walk + cam.view.t[1]*stride)*frameTime;

			if (sj::Cell(int(newPos.x), int(cam.pos.y)) == 0)
			{
				cam.pos.x = newPos.x;
			}
			if (sj::Cell(int(cam.pos.x), int(newPos.y)) == 0)
			{
				cam.pos.y = newPos.y;
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

