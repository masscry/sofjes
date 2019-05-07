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

	struct brick_t {
		uint32_t width;
		uint32_t height;
		uint32_t bytes_per_pixel;
		uint8_t  pixel_data[16 * 16 * 3 + 1];
	};

	extern const brick_t brick;

}

#endif /* __SOFJES_WORLD_HEADER__ */