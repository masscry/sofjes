/**
 * @file texture_t.h
 * @author masscry
 * @date 2019-05-13
 * 
 * Basic texture class definition.
 * 
 */

#pragma once
#ifndef __SOFJES_TEXTURE_T_HEADER__
#define __SOFJES_TEXTURE_T_HEADER__

#include <cstdint>
#include <vector>
#include <cmath>
#include <world.h>

namespace sj {

	/**
	 * Basic texture class.
	 */
	class texture_t {
		uint32_t width;
		uint32_t height;
		uint32_t bpp;
		std::vector<uint8_t> pixel;
	public:

		/**
		 * Get texture width in pixels.
		 */
		uint32_t Width() const;

		/**
		 * Get texture height in pixels.
		 */
		uint32_t Height() const;

		/**
		 * Get texture bytes per pixel.
		 */
		uint32_t BytesPerPixel() const;

		/**
		 * Get sampled pixel data.
		 * 
		 * @param u texture coordinate between 0.0 and 1.0
		 * @param v texture coordinate between 0.0 and 1.0
		 * @return pointer to first pixel byte
		 */
		const uint8_t* Sample(float u, float v) const;

		/**
		 * Get sampled pixel data.
		 * 
		 * @param tx x pixel coordinate
		 * @param ty y pixel coordinate
		 * @return pointer to first pixel byte
		 */
		const uint8_t* Sample(uint32_t tx, uint32_t ty) const;

		/**
		 * Store pixel data in texture
		 * 
		 * @param width texture width in pixels
		 * @param height texture height in pixels
		 * @param bpp texture bytes per pixel
		 * @param pixel_data pixel data itself
		 */
		void Assign(uint32_t width, uint32_t height, uint32_t bpp, std::vector<uint8_t>&& pixel_data);

		texture_t& operator = (texture_t&& move);
    texture_t& operator = (const texture_t& copy);

		texture_t();
		texture_t(texture_t&& move);
    texture_t(const texture_t& copy);
		texture_t(uint32_t width, uint32_t height, uint32_t bpp, const uint8_t* pixel_data);
		texture_t(uint32_t width, uint32_t height, uint32_t bpp, std::vector<uint8_t>&& pixel_data);

		~texture_t();

		/**
		 * Generate texture atlas array from spritesheet.
		 * 
		 * @param tiles total sprite count on X and Y axes.
		 * @param stride size of empty space between sprites on X and Y axes
		 * @return texture atlas array of by rows top left has index 0, bottom right has index X*Y-1 
		 */
		std::vector<texture_t> GenerateAtlas(vec2i_t tiles, vec2i_t stride);

	};

	inline uint32_t texture_t::Width() const {
		return this->width;
	}

	inline uint32_t texture_t::Height() const {
    return this->height;
	}

	inline uint32_t texture_t::BytesPerPixel() const {
		return this->bpp;
	}

  inline const uint8_t* texture_t::Sample(uint32_t tx, uint32_t ty) const {
    tx %= this->width;
    ty %= this->height;
    return pixel.data()+(ty*this->width+tx)*this->bpp;
  }

	inline const uint8_t* texture_t::Sample(float u, float v) const {
		u = (u > 1.0f)?1.0f:u;
		v = (v > 1.0f)?1.0f:v;
		u = (u < 0.0f)?0.0f:u;
		v = (v < 0.0f)?0.0f:v;
		return pixel.data()+(((int)(v * this->height))*this->width + ((int)(u * this->width)))*this->bpp;
	}

}

#endif /* __SOFJES_TEXTURE_T_HEADER__ */