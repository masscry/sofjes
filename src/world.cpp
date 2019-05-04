#include <world.h>
#include <cmath>

namespace
{
	int map[sj::MAP_WIDTH][sj::MAP_HEIGHT] =
	{
	  {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
	  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
	  {4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
	  {4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
	  {4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
	  {4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
	  {4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
	  {4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
	  {4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1},
	  {4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
	  {4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
	  {4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
	  {6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
	  {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
	  {6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
	  {4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
	  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
	  {4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
	  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
	  {4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
	  {4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
	  {4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
	  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
	  {4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
	};

	SDL_Window* g_mainWin = nullptr;
	SDL_Surface* g_mainSurface = nullptr;
	uint32_t* g_mainBuffer = nullptr;

}


namespace sj {

	int Cell(uint32_t x, uint32_t y) {
		if ((x < MAP_WIDTH) && (y < MAP_HEIGHT))
		{
			return map[x][y];
		}
		return 1;
	}

	void CreateWindow() {
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

		g_mainWin = SDL_CreateWindow(
			"SofJes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN
		);

		g_mainSurface = SDL_GetWindowSurface(g_mainWin);
		g_mainBuffer = (uint32_t*)malloc(sizeof(uint32_t) * WIN_WIDTH * WIN_HEIGHT);
	}

	void UpdateWindow() {
		SDL_UpdateWindowSurface(g_mainWin);
	}

	void CloseWindow() {
		SDL_DestroyWindow(g_mainWin);
		SDL_Quit();
	}

	inline uint32_t RGBtoINT(uint8_t r, uint8_t g, uint8_t b) {
		return ((uint32_t)b) | (((uint32_t)g) << 8) | (((uint32_t)r) << 16);
	}

	inline void PutPixel(int y, int x, Uint8 r, Uint8 g, Uint8 b)
	{
		g_mainBuffer[y * WIN_WIDTH + x] = RGBtoINT(r, g, b);
	}

	void DrawBuffer(Uint32 * buffer)
	{
		uint32_t* bufp;
		bufp = (uint32_t*)g_mainSurface->pixels;

		for (int y = 0; y < WIN_HEIGHT; ++y)
		{
			for (int x = 0; x < WIN_WIDTH; ++x)
			{
				*bufp = *buffer++;
				bufp++;
			}
			bufp += g_mainSurface->pitch / 4;
			bufp -= WIN_WIDTH;
		}
	}

	void Render(vec2f_t pos, vec2f_t dir, vec2f_t plane) {
		for (int x = 0; x < sj::WIN_WIDTH; ++x) {
			float camX = 2 * x / (float)sj::WIN_WIDTH - 1;

			float rayDirX = dir.x + plane.x * camX;
			float rayDirY = dir.y + plane.y * camX;

			int mapX = int(pos.x);
			int mapY = int(pos.y);

			float sideDistX;
			float sideDistY;

			float deltaDistX = std::abs(1.0f / rayDirX);
			float deltaDistY = std::abs(1.0f / rayDirY);

			float perpWallDist;

			int stepX;
			int stepY;

			int hit = 0;
			int side;

			if (rayDirX < 0.0f)
			{
				stepX = -1;
				sideDistX = (pos.x - mapX) * deltaDistX;
			}
			else
			{
				stepX = 1;
				sideDistX = (mapX + 1.0f - pos.x) * deltaDistX;
			}

			if (rayDirY < 0.0f)
			{
				stepY = -1;
				sideDistY = (pos.y - mapY) * deltaDistY;
			}
			else
			{
				stepY = 1;
				sideDistY = (mapY + 1.0f - pos.y) * deltaDistY;
			}

			while (hit == 0)
			{
				if (sideDistX < sideDistY)
				{
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
				}
				else
				{
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
				}
				if (sj::Cell(mapX, mapY) > 0)
				{
					hit = 1;
				}
			}

			if (side == 0)
			{
				perpWallDist = (mapX - pos.x + (1 - stepX) / 2) / rayDirX;
			}
			else
			{
				perpWallDist = (mapY - pos.y + (1 - stepY) / 2) / rayDirY;
			}

			int lineHeight = (int)(sj::WIN_HEIGHT / perpWallDist);

			int drawStart = -lineHeight / 2 + sj::WIN_HEIGHT / 2;

			if (drawStart < 0)
			{
				drawStart = 0;
			}
			int drawEnd = lineHeight / 2 + sj::WIN_HEIGHT / 2;
			if (drawEnd >= sj::WIN_HEIGHT)
			{
				drawEnd = sj::WIN_HEIGHT - 1;
			}

			float wallX;

			if (side == 0) {
				wallX = pos.y + perpWallDist * rayDirY;
			}
			else {
				wallX = pos.x + perpWallDist * rayDirX;
			}
			wallX -= floor(wallX);

			int texX = (int)(wallX * ((double)brick.width));
			if (side == 0 && rayDirX > 0) {
				texX = brick.width - texX - 1;
			}
			if (side == 1 && rayDirY < 0) {
				texX = brick.width - texX - 1;
			}

			for (int y = drawStart; y <= drawEnd; ++y) {
				float r;
				float g;
				float b;

				int d = y * 256 - WIN_HEIGHT * 128 + lineHeight * 128;

				int texY = ((d * brick.height) / lineHeight) / 256;

				r = brick.pixel_data[(brick.height * texY + texX) * 3];
				g = brick.pixel_data[(brick.height * texY + texX) * 3 + 1];
				b = brick.pixel_data[(brick.height * texY + texX) * 3 + 2];

				if (perpWallDist <= 1.0f) {
					perpWallDist = 1.0f;
				}

				r /= perpWallDist;
				g /= perpWallDist;
				b /= perpWallDist;

				sj::PutPixel(y, x, (uint8_t)r, (uint8_t)g, (uint8_t)b);
			}

			float floorXWall;
			float floorYWall;

			if (side == 0 && rayDirX > 0)
			{
				floorXWall = (float)mapX;
				floorYWall = (float)mapY + wallX;
			}
			else if (side == 0 && rayDirX < 0)
			{
				floorXWall = mapX + 1.0f;
				floorYWall = mapY + wallX;
			}
			else if (side == 1 && rayDirY > 0)
			{
				floorXWall = (float)mapX + wallX;
				floorYWall = (float)mapY;
			}
			else
			{
				floorXWall = mapX + wallX;
				floorYWall = mapY + 1.0f;
			}

			float distWall;
			float distPlayer;
			float currentDist;

			distWall = perpWallDist;
			distPlayer = 0.0f;

			if (drawEnd < 0)
			{
				drawEnd = WIN_HEIGHT;
			}

			for (int y = drawEnd + 1; y < WIN_HEIGHT; ++y)
			{
				float r;
				float g;
				float b;

				currentDist = WIN_HEIGHT / (2.0f * y - WIN_HEIGHT);

				float weight = (currentDist - distPlayer) / (distWall - distPlayer);

				float currentFloorX = weight * floorXWall + (1.0f - weight) * pos.x;
				float currentFloorY = weight * floorYWall + (1.0f - weight) * pos.y;

				int floorTexX, floorTexY;
				floorTexX = int(currentFloorX * brick.width) % brick.width;
				floorTexY = int(currentFloorY * brick.height) % brick.height;

				r = brick.pixel_data[(brick.height * floorTexY + floorTexX) * 3];
				g = brick.pixel_data[(brick.height * floorTexY + floorTexX) * 3 + 1];
				b = brick.pixel_data[(brick.height * floorTexY + floorTexX) * 3 + 2];

				r /= currentDist;
				g /= currentDist;
				b /= currentDist;

				PutPixel(y, x, (uint8_t)r, (uint8_t)g, (uint8_t)b);
				PutPixel(WIN_HEIGHT - y, x, (uint8_t)r, (uint8_t)g, (uint8_t)b);
			}

		}

		DrawBuffer(g_mainBuffer);

	}


}