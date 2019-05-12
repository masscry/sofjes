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

	inline uint32_t RGBtoINT(uint32_t r, uint32_t g, uint32_t b) {
		return (b) | (g << 8) | (r << 16);
	}

	inline void PutPixel(int y, int x, uint32_t r, uint32_t g, uint32_t b)
	{
		g_mainBuffer[y * WIN_WIDTH + x] = RGBtoINT(r, g, b);
	}

	inline void PutPixel(int y, int x, uint32_t col)
	{
		g_mainBuffer[y * WIN_WIDTH + x] = col;
	}

	template<int pixcnt>
	inline void PutPixelRow(int y, int x, uint32_t col)
	{
		g_mainBuffer[y * WIN_WIDTH + x] = col;
		PutPixelRow<pixcnt - 1>(y, x + 1, col);
	}

	template<>
	inline void PutPixelRow<0>(int y, int x, uint32_t col)
	{
		;
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
			bufp += g_mainSurface->pitch / sizeof(uint32_t);
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

	hit_t CastRay(vec2f_t pos, vec2f_t rayDir) {

		vec2f_t sideDist;
		vec2f_t deltaDist{
			std::abs(1.0f / rayDir.x),
			std::abs(1.0f / rayDir.y)
		};

		vec2i_t step;
		vec2i_t map{ int(pos.x), int(pos.y) };

		int hit = 0;
		int side;

		step.x = (rayDir.x < 0.0f)?(-1):(1);
		sideDist.x = ((rayDir.x < 0.0f)?(pos.x - map.x):(map.x + 1.0f - pos.x))*deltaDist.x;

		step.y = (rayDir.y < 0.0f)?(-1):(1);
		sideDist.y = ((rayDir.y < 0.0f)?(pos.y - map.y):(map.y + 1.0f - pos.y))*deltaDist.y;

		while (hit == 0)
		{
			if (sideDist.x < sideDist.y)
			{
				sideDist.x += deltaDist.x;
				map.x += step.x;
				side = 0;
			}
			else
			{
				sideDist.y += deltaDist.y;
				map.y += step.y;
				side = 1;
			}
			if (sj::Cell(map.x, map.y) > 0)
			{
				hit = 1;
			}
		}

		if (side == 0)
		{
			return hit_t{ (map.x - pos.x + (1.0f - step.x) / 2.0f) / rayDir.x, side, map };
		}
		else
		{
			return hit_t{ (map.y - pos.y + (1.0f - step.y) / 2.0f) / rayDir.y, side, map };
		}
	}

	static void SampleWall(const texture_t & tex, float xCoord, uint32_t* pTexels, float lineHeight, int totalTexels, float invDist) {
		float step = 1.0f/lineHeight;
		float toffset = (lineHeight - totalTexels)/2.0f;
		for (int i = 0; i < totalTexels; i+=2) {
			float u = xCoord;
			float v = step*(i+toffset);
			const uint8_t* smpx = tex.Sample(u, v);

			pTexels[i] = RGBtoINT(
				(uint32_t)(smpx[0] * invDist),
				(uint32_t)(smpx[1] * invDist),
				(uint32_t)(smpx[2] * invDist)
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

		SampleWall(g_mainAtlas[Cell(hit.map.x,hit.map.y)], wallTexX, texLine, lineHeight, drawEnd-drawStart+1, 1.0f/hit.dist);

		for (int y = drawStart; y <= drawEnd; ++y) 
		{
			sj::PutPixelRow<4>(y, x * 4, texLine[y - drawStart]);
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

	inline uint32_t PutSideLine(const texture_t& tex, float invCurrentDist, float distWall, vec2f_t floorWall, vec2f_t pos, int x, int y)
	{
			float weight = 1.0f / (invCurrentDist * distWall);

			float currentFloorX = weight * floorWall.x + (1.0f - weight) * pos.x;
			float currentFloorY = weight * floorWall.y + (1.0f - weight) * pos.y;

			currentFloorX -= (int)(currentFloorX);
			currentFloorY -= (int)(currentFloorY);

			const uint8_t* txsmp = tex.Sample(currentFloorX, currentFloorY);
			
			uint32_t col = RGBtoINT(
				(uint32_t)(txsmp[0] * invCurrentDist),
				(uint32_t)(txsmp[1] * invCurrentDist),
				(uint32_t)(txsmp[2] * invCurrentDist)
			);

			PutPixelRow<4>(y, x * 4, col);
			return col;
	}

	void RenderFloor(const texture_t& tex, vec2f_t floorWall, float distWall, int wallEnd, vec2f_t pos, int x) {
		for (int y = wallEnd + 1; y < WIN_HEIGHT; y+=2)
		{
			float currentDist = (2.0f * y - WIN_HEIGHT) / WIN_HEIGHT;
			uint32_t cache = PutSideLine(tex, currentDist, distWall, floorWall, pos, x, y);
			if (y + 1 < WIN_HEIGHT)
			{
				PutPixelRow<4>(y + 1, x * 4, cache);
			}
		}
	}

	void RenderCelling(const texture_t& tex, vec2f_t ceilWall, float distWall, int wallStart, vec2f_t pos, int x) {
		for (int y = 0; y < wallStart; y+=2)
		{
			float currentDist = (WIN_HEIGHT - 2.0f * y) / WIN_HEIGHT;
			uint32_t cache = PutSideLine(tex, currentDist, distWall, ceilWall, pos, x, y);
			if (y + 1 < wallStart)
			{
				PutPixelRow<4>(y + 1, x * 4, cache);
			}
		}
	}

	void Render(camera_t cam, std::vector<float>* wallDist) {
		float camPart = 2.0f / sj::WIN_WIDTH;

		for (int x = 0; x < sj::WIN_WIDTH / 4; ++x) {
			float camX = camPart * x * 4.0f - 1;
			vec2f_t rayDir = cam.view.t[0] + cam.view.t[1]*camX;

			hit_t hit = CastRay(cam.pos, rayDir);
			(*wallDist)[x] = hit.dist;
			wall_t wall = RenderWalls(x, hit, cam.pos, rayDir);
			vec2f_t floorWall = FloorCast(hit, rayDir, wall);
			RenderFloor(g_mainAtlas[16], floorWall, hit.dist, wall.end, cam.pos, x);
			RenderCelling(g_mainAtlas[17], floorWall, hit.dist, wall.start, cam.pos, x);
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


	void RenderSprites(const std::vector<float>& wallDist, camera_t cam, const sprite_t* sprites, uint32_t spritesTotal) {
		CompareSpriteDistance compDist(cam.pos);
		std::vector<const sprite_t*> sortedSprites;

		sortedSprites.reserve(spritesTotal);
		for (uint32_t i = 0; i < spritesTotal; ++i) {
			sortedSprites.push_back(sprites+i);
		}
		std::sort(sortedSprites.begin(), sortedSprites.end(), compDist);

		matf_t invView = Inverse(cam.view);

		for (std::vector<const sprite_t*>::iterator i = sortedSprites.begin(), e = sortedSprites.end(); i!=e; ++i)
		{
			sprite_t tmp = *(*i);
			tmp.pos -= cam.pos;

			vec2f_t transform = invView*tmp.pos;
			
			int sprScreenX = int((WIN_WIDTH / 2.0f) * (1.0f + transform.x / transform.y));

			int sprHeight = std::abs(int(WIN_HEIGHT/1.5f / (transform.y)));
			
			int drawStartY = (int)(-sprHeight / 2 + WIN_HEIGHT / 2 + 100.0f/transform.y);
			int drawOffsetY = 0;
			if(drawStartY < 0)
			{
				drawOffsetY = -drawStartY;
				drawStartY = 0;
			}

			int drawEndY = (int)(sprHeight / 2 + WIN_HEIGHT / 2 + 100.0f/transform.y);
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

			float invCurrentDist = (transform.y<1.0f)?1.0f:(1.0f/transform.y);

			for(int stripe = drawStartX; stripe < drawEndX; stripe+=4)
			{
				if(transform.y > 0.0f && stripe > 0 && stripe < WIN_WIDTH && transform.y < wallDist[stripe/4])
				{
					for(int y = drawStartY; y < drawEndY; ++y)
					{
						const uint8_t* smp = g_mainAtlas[30].Sample(txStep*(stripe-drawStartX+drawOffsetX), tyStep*(y-drawStartY+drawOffsetY));
						if (smp[3] != 0) {
							uint32_t color = RGBtoINT(
								(uint32_t)(smp[0] * invCurrentDist),
								(uint32_t)(smp[1] * invCurrentDist),
								(uint32_t)(smp[2] * invCurrentDist)
							);
							PutPixelRow<4>(y, stripe, color);
						}
					}
				}
			}
		}
	}

}