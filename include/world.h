#pragma once
#ifndef __SOFJES_WORLD_HEADER__
#define __SOFJES_WORLD_HEADER__

#include <cstdint>
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <vecmath.h>

namespace sj {

	const uint32_t MAP_WIDTH = 24;
	const uint32_t MAP_HEIGHT = 24;

	const uint32_t WIN_WIDTH = 1024;
	const uint32_t WIN_HEIGHT = 768;

	int Cell(uint32_t x, uint32_t y);

	void CreateWindow();
	void UpdateWindow(float frameTime);
	void CloseWindow();

	float FrameTime();

	void Render(vec2f_t pos, vec2f_t dir, vec2f_t plane);

}

#endif /* __SOFJES_WORLD_HEADER__ */