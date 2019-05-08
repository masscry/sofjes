#pragma once
#ifndef __SOFJES_WORLD_HEADER__
#define __SOFJES_WORLD_HEADER__

#include <stdint.h>
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

namespace sj {

	const uint32_t MAP_WIDTH = 24;
	const uint32_t MAP_HEIGHT = 24;

	const uint32_t WIN_WIDTH = 1024;
	const uint32_t WIN_HEIGHT = 768;

	int Cell(uint32_t x, uint32_t y);

	void CreateWindow();
	void UpdateWindow(float frameTime);
	void CloseWindow();

	struct vec2f_t {
		float x;
		float y;
	};

	struct vec2i_t {
		int x;
		int y;
	};

	float FrameTime();

	void Render(vec2f_t pos, vec2f_t dir, vec2f_t plane);

	class brick_t {
		uint32_t width;
		uint32_t height;
		uint32_t bpp;
		const uint8_t* pixel_data;
	public:

		uint32_t Height() const {
			return this->height;
		}

		const uint8_t* Sample(float u, float v) const {
			u = (u > 1.0f)?1.0f:u;
			v = (v > 1.0f)?1.0f:v;
			u = (u < 0.0f)?0.0f:u;
			v = (v < 0.0f)?0.0f:v;
			u = floor(u*this->width);
			v = floor(v*this->height);
			return pixel_data+(((int)v)*this->width + ((int)u))*this->bpp;
		}

		brick_t(uint32_t width, uint32_t height, uint32_t bpp, const uint8_t* pixel_data)
		:width(width), height(height), bpp(bpp), pixel_data(pixel_data) {
			;
		}

	};

	extern const brick_t brick;

}

#endif /* __SOFJES_WORLD_HEADER__ */