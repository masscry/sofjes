#include <cmath>
#include <vector>
#include <cstdio>
#include <cassert>
#include <set>
#include <algorithm>

#include <world.h>
#include <targa.h>

namespace
{
	int map[sj::MAP_WIDTH][sj::MAP_HEIGHT] =
	{
		{416, 416, 416, 416, 416, 416, 416, 416, 416, 416},
		{416,   0,   0,   0,  19,   0,   0,   0,   0, 416},
		{416,  19,  19,   0,  19,   0,  19,   0,   0, 416},
		{416,   0,   0,   0,   0,   0,   0,   0,   0, 416},
		{416,  19,  19,   0,   0,   0,  19,   0,   0, 416},
		{416,   0,   0,   0,   0,   0,   0,   0,   0, 416},
		{416,  19,  19,   0,  19,   0,  19,   0,   0, 416},
		{416,   0,   0,   0,  19,   0,   0,   0,   0, 416},
		{416,   0,   0,   0,   0,   0,   0,   0,   0, 416},
		{416, 416, 416, 416, 416, 416, 416, 416, 416, 416}
	};

	SDL_Window* g_mainWin = nullptr;
	SDL_Surface* g_mainSurface = nullptr;
	uint32_t* g_mainBuffer = nullptr;
	sj::texture_t g_mainTexture;

	std::vector<sj::texture_t> g_mainAtlas;

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

		FILE* input = fopen("./resources/colored.tga", "rb");
		g_mainTexture = tgaLoad(input);
		fclose(input);

		g_mainAtlas = g_mainTexture.GenerateAtlas(vec2i_t{32, 32}, vec2i_t{1, 1});
	}

	void DrawBuffer(Uint32 * buffer);

	void UpdateWindow(float frameTime) {
		char text[256];
		DrawBuffer(g_mainBuffer);
		SDL_UpdateWindowSurface(g_mainWin);
		snprintf(text, 256, "FPS: %3.1f", 1.0f / frameTime);
		SDL_SetWindowTitle(g_mainWin, text);
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

	inline void PutPixel(int y, int x, uint32_t col)
	{
		g_mainBuffer[y * WIN_WIDTH + x] = col;
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

	float FrameTime() {
		static uint32_t oldTime = 0;
		static uint32_t time = 0;

		oldTime = time;
		time = SDL_GetTicks();
		return (time - oldTime) / 1000.0f;
	}

	struct hit_t {
		float dist;
		int side;
		vec2i_t map;
	};

	hit_t CastRay(vec2f_t pos, vec2f_t rayDir, int x) {

		float sideDistX;
		float sideDistY;

		float deltaDistX = std::abs(1.0f / rayDir.x);
		float deltaDistY = std::abs(1.0f / rayDir.y);

		int stepX;
		int stepY;

		int hit = 0;
		int side;

		vec2i_t map{ int(pos.x), int(pos.y) };

		if (rayDir.x < 0.0f)
		{
			stepX = -1;
			sideDistX = (pos.x - map.x) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (map.x + 1.0f - pos.x) * deltaDistX;
		}

		if (rayDir.y < 0.0f)
		{
			stepY = -1;
			sideDistY = (pos.y - map.y) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (map.y + 1.0f - pos.y) * deltaDistY;
		}

		while (hit == 0)
		{
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				map.x += stepX;
				side = 0;
			}
			else
			{
				sideDistY += deltaDistY;
				map.y += stepY;
				side = 1;
			}
			if (sj::Cell(map.x, map.y) > 0)
			{
				hit = 1;
			}
		}

		if (side == 0)
		{
			return hit_t{ (map.x - pos.x + (1 - stepX) / 2) / rayDir.x, side, map };
		}
		else
		{
			return hit_t{ (map.y - pos.y + (1 - stepY) / 2) / rayDir.y, side, map };
		}
	}

	void SampleWall(const texture_t & tex, float xCoord, uint32_t* pTexels, float lineHeight, int totalTexels, float dist) {
		float step = 1.0f/lineHeight;
		float toffset = (lineHeight - totalTexels)/2.0f;
		for (int i = 0; i < totalTexels; i+=2) {
			float u = xCoord;
			float v = step*(i+toffset);
			const uint8_t* smpx = tex.Sample(u, v);

			pTexels[i] = RGBtoINT(
				(uint8_t)((smpx[0])/dist),
				(uint8_t)((smpx[1])/dist),
				(uint8_t)((smpx[2])/dist)
			);
			pTexels[i + 1] = pTexels[i];
		}
	}

	struct wall_t {
		float x;
		int start;
		int end;
	};

	wall_t RenderWalls(int x, hit_t hit, vec2f_t pos, vec2f_t rayDir) {
		uint32_t texLine[WIN_HEIGHT];
		float lineHeight = sj::WIN_HEIGHT / hit.dist;

		int lineStart = (int)(-lineHeight / 2.0f + sj::WIN_HEIGHT / 2.0f);
		int drawStart = lineStart;
		if (drawStart < 0)
		{
			drawStart = 0;
		}

		int drawEnd = (int)(lineHeight / 2.0f + sj::WIN_HEIGHT / 2.0f);
		if (drawEnd >= sj::WIN_HEIGHT)
		{
			drawEnd = sj::WIN_HEIGHT - 1;
		}

		float wallX;
		float wallTexX;

		if (hit.side == 0) {
			wallX = pos.y + hit.dist * rayDir.y;
		}
		else {
			wallX = pos.x + hit.dist * rayDir.x;
		}
		wallX -= floor(wallX);

		if ((hit.side == 0 && rayDir.x > 0.0f) || (hit.side == 1 && rayDir.y < 0.0f)) {
			wallTexX = 1.0f - wallX;
		}
		else {
			wallTexX = wallX;
		}

		if (hit.dist <= 1.0f) {
			hit.dist = 1.0f;
		}

		SampleWall(g_mainAtlas[Cell(hit.map.x,hit.map.y)], wallTexX, texLine, lineHeight, drawEnd-drawStart+1, hit.dist);

		for (int y = drawStart; y <= drawEnd; ++y) {
			sj::PutPixel(y, x * 4, texLine[y - drawStart]);
			sj::PutPixel(y, x * 4 + 1, texLine[y - drawStart]);
			sj::PutPixel(y, x * 4 + 2, texLine[y - drawStart]);
			sj::PutPixel(y, x * 4 + 3, texLine[y - drawStart]);
		}
		return wall_t{ wallX, drawStart, drawEnd };
	}

	vec2f_t FloorCast(hit_t hit, vec2f_t rayDir, wall_t wall) {
		if (hit.side == 0 && rayDir.x > 0)
		{
			return vec2f_t{ (float)hit.map.x, (float)hit.map.y + wall.x };
		}
		else if (hit.side == 0 && rayDir.x < 0)
		{
			return vec2f_t{ hit.map.x + 1.0f, hit.map.y + wall.x };
		}
		else if (hit.side == 1 && rayDir.y > 0)
		{
			return vec2f_t{ (float)hit.map.x + wall.x, (float)hit.map.y };
		}	
		return vec2f_t{ hit.map.x + wall.x, hit.map.y + 1.0f };
	}

	void PutSideLine(const texture_t& tex, float currentDist, float distWall, vec2f_t floorWall, vec2f_t pos, int x, int y)
	{
			float weight = currentDist / distWall;

			float currentFloorX = weight * floorWall.x + (1.0f - weight) * pos.x;
			float currentFloorY = weight * floorWall.y + (1.0f - weight) * pos.y;

			const uint8_t* txsmp = tex.Sample(
				currentFloorX - floor(currentFloorX), 
				currentFloorY - floor(currentFloorY)
			);

			uint32_t col = RGBtoINT(
				(uint8_t)(txsmp[0] / currentDist),
				(uint8_t)(txsmp[1] / currentDist),
				(uint8_t)(txsmp[2] / currentDist)
			);

			PutPixel(y, x * 4, col);
			PutPixel(y, x * 4 + 1, col);
			PutPixel(y, x * 4 + 2, col);
			PutPixel(y, x * 4 + 3, col);
	}

	void RenderFloor(const texture_t& tex, vec2f_t floorWall, float distWall, int wallEnd, vec2f_t pos, int x) {
		for (int y = wallEnd + 1; y < WIN_HEIGHT; ++y)
		{
			float currentDist = WIN_HEIGHT / (2.0f * y - WIN_HEIGHT);
			PutSideLine(tex, currentDist, distWall, floorWall, pos, x, y);
		}
	}

	void RenderCelling(const texture_t& tex, vec2f_t ceilWall, float distWall, int wallStart, vec2f_t pos, int x) {
		for (int y = 0; y < wallStart; ++y)
		{
			float currentDist = WIN_HEIGHT / (WIN_HEIGHT - 2.0f * y);
			PutSideLine(tex, currentDist, distWall, ceilWall, pos, x, y);
		}
	}

	void Render(vec2f_t pos, vec2f_t dir, vec2f_t plane, std::vector<float>* wallDist) {
		float camPart = 2.0f / sj::WIN_WIDTH;

		for (int x = 0; x < sj::WIN_WIDTH / 4; ++x) {
			float camX = camPart * x * 4.0f - 1;
			vec2f_t rayDir = { dir.x + plane.x * camX, dir.y + plane.y * camX };

			hit_t hit = CastRay(pos, rayDir, x);
			(*wallDist)[x] = hit.dist;
			wall_t wall = RenderWalls(x, hit, pos, rayDir);
			vec2f_t floorWall = FloorCast(hit, rayDir, wall);
			RenderFloor(g_mainAtlas[16], floorWall, hit.dist, wall.end, pos, x);
			RenderCelling(g_mainAtlas[17], floorWall, hit.dist, wall.start, pos, x);
		}
	}

	class CompareSpriteDistance {
		vec2f_t pos;
	public:

		bool operator()(const sprite_t* a, const sprite_t* b) {
			return SqrDistance(a->pos, this->pos) > SqrDistance(b->pos, this->pos);
		}

		CompareSpriteDistance(vec2f_t pos):pos(pos) {
			;
		}

	};


	void RenderSprites(const std::vector<float>& wallDist, vec2f_t pos, vec2f_t dir, vec2f_t plane, const sprite_t* sprites, uint32_t spritesTotal) {
		CompareSpriteDistance compDist(pos);
		std::vector<const sprite_t*> sortedSprites;

		sortedSprites.reserve(spritesTotal);
		for (uint32_t i = 0; i < spritesTotal; ++i) {
			sortedSprites.push_back(sprites+i);
		}
		std::sort(sortedSprites.begin(), sortedSprites.end(), compDist);

		for (std::vector<const sprite_t*>::iterator i = sortedSprites.begin(), e = sortedSprites.end(); i!=e; ++i)
		{
			sprite_t tmp = *(*i);
			tmp.pos.x -= pos.x;
			tmp.pos.y -= pos.y;

			float invDet = 1.0f / (plane.x*dir.y - dir.x*plane.y);

			vec2f_t transform{
				invDet * (dir.y * tmp.pos.x - dir.x * tmp.pos.y),
				invDet * (-plane.y * tmp.pos.x + plane.x * tmp.pos.y)
			};

			int sprScreenX = int((WIN_WIDTH / 2.0f) * (1.0f + transform.x / transform.y));

			int sprHeight = std::abs(int(WIN_HEIGHT/1.5f / (transform.y)));
			
			int drawStartY = -sprHeight / 2 + WIN_HEIGHT / 2 + 100.0f/transform.y;
			int drawOffsetY = 0;
			if(drawStartY < 0)
			{
				drawOffsetY = -drawStartY;
				drawStartY = 0;
			}

			int drawEndY = sprHeight / 2 + WIN_HEIGHT / 2 + 100.0f/transform.y;
			if(drawEndY >= (int)WIN_HEIGHT)
			{
				drawEndY = WIN_HEIGHT - 1;
			}

			int sprWidth = std::abs(int(WIN_HEIGHT/2.0f / (transform.y)));
			int drawStartX = -sprWidth / 2 + sprScreenX;
			int drawOffsetX = 0;
			if(drawStartX < 0)
			{
				drawOffsetX = -drawStartX;
				drawStartX = 0;
			}
			
			int drawEndX = sprWidth / 2 + sprScreenX;
			if(drawEndX >= (int)WIN_WIDTH)
			{
				drawEndX = WIN_WIDTH - 1;
			}

			float txStep = 1.0f/sprWidth;
			float tyStep = 1.0f/sprHeight;

			float cdist = (transform.y<1.0f)?1.0f:transform.y;

			for(int stripe = drawStartX; stripe < drawEndX; stripe+=4)
			{
				if(transform.y > 0.0f && stripe > 0 && stripe < WIN_WIDTH && transform.y < wallDist[stripe/4])
				{
					for(int y = drawStartY; y < drawEndY; ++y)
					{
						const uint8_t* smp = g_mainAtlas[30].Sample(txStep*(stripe-drawStartX+drawOffsetX), tyStep*(y-drawStartY+drawOffsetY));
						if (smp[3] != 0) {
							uint32_t color = RGBtoINT(
								smp[0]/cdist,
								smp[1]/cdist,
								smp[2]/cdist
							);
							PutPixel(y, stripe, color);
							PutPixel(y, stripe+1, color);
							PutPixel(y, stripe+2, color);
							PutPixel(y, stripe+3, color);
						}
					}
				}
			}
		}
	}

}