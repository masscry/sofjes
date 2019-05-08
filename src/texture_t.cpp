#include <stdexcept>
#include <texture_t.h>

namespace sj {

  texture_t& texture_t::operator = (texture_t&& move) {
	  if (this != &move) {
			this->width = move.width;
			this->height = move.height;
			this->bpp = move.bpp;
			this->pixel = std::move(move.pixel);
		}
		return *this;
	}

	texture_t::texture_t(texture_t&& move):width(move.width),height(move.height),bpp(move.bpp),pixel(std::move(move.pixel)){
    ;
	}

	texture_t::texture_t():width(0),height(0),bpp(0),pixel() {
		;
	}

	texture_t::texture_t(uint32_t width, uint32_t height, uint32_t bpp, const uint8_t* pixel_data)
		:width(width), height(height), bpp(bpp), pixel() {
		this->pixel.resize(this->width*this->height*this->bpp);
		std::copy(pixel_data, pixel_data+this->width*this->height*this->bpp, this->pixel.begin());
	}

	texture_t::texture_t(uint32_t width, uint32_t height, uint32_t bpp, std::vector<uint8_t>&& pixel_data)
		:width(width), height(height), bpp(bpp), pixel(std::move(pixel_data)) {
		;
	}

  texture_t::texture_t(const texture_t& copy):width(copy.width),height(copy.height),bpp(copy.bpp),pixel(copy.pixel){
    ;
  }

  texture_t& texture_t::operator = (const texture_t& copy) {
    if (this != &copy) {
      this->width = copy.width;
      this->height = copy.height;
      this->bpp = copy.bpp;
      this->pixel = copy.pixel;
    }
    return *this;
  }

	void texture_t::Assign(uint32_t width, uint32_t height, uint32_t bpp, std::vector<uint8_t>&& pixel_data)
	{
		this->width = width;
		this->height = height;
		this->bpp = bpp;
		this->pixel = std::move(pixel_data);
	}

	texture_t::~texture_t() {
		;
	}

  std::vector<texture_t> texture_t::GenerateAtlas(vec2i_t tiles, vec2i_t stride) {

    if ((tiles.x == 0) || (tiles.y == 0)) {
      throw std::runtime_error("Invalid arguments");
    }

    std::vector<texture_t> result;
    result.reserve(tiles.x*tiles.y);

    vec2ui_t spSize;

    spSize.x = (this->width-stride.x*(tiles.x-1))/tiles.x;
    spSize.y = (this->height-stride.y*(tiles.y-1))/tiles.y;

    for (int tileY = 0; tileY < tiles.y; ++tileY)
    {
      for (int tileX = 0; tileX < tiles.x; ++tileX)
      {
        std::vector<uint8_t> smpixels;
        smpixels.reserve(spSize.x*spSize.y*this->bpp);

        for (uint32_t ty = 0; ty < spSize.y; ++ty) 
        {
          for (uint32_t tx = 0; tx < spSize.x; ++tx) 
          {
            const uint8_t* pixdt = this->Sample(
              tx + tileX*(stride.x+spSize.x),
              ty + tileY*(stride.y+spSize.y)
            ); 
            for (uint32_t tbpp = 0; tbpp < this->bpp; ++tbpp) {
              smpixels.push_back(pixdt[tbpp]);
            }
          }
        }
        result.push_back(texture_t(spSize.x, spSize.y, this->bpp, std::move(smpixels)));
      }
    }

    return std::move(result);

  }

}
