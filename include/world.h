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

	const uint32_t MAP_WIDTH = 10;
	const uint32_t MAP_HEIGHT = 10;

	const uint32_t WIN_WIDTH = 1024;
	const uint32_t WIN_HEIGHT = 768;

	int Cell(uint32_t x, uint32_t y);

	struct sprite_t {
		vec2f_t pos;
		int tex;
	};

	void CreateWindow();
	void UpdateWindow(float frameTime);
	void CloseWindow();

	float FrameTime();

	void Render(vec2f_t pos, matf_t view, std::vector<float>* wallDist);

	void RenderSprites(const std::vector<float>& wallDist, vec2f_t pos, matf_t view, const sprite_t* sprites, uint32_t spritesTotal);

}

#endif /* __SOFJES_WORLD_HEADER__ */