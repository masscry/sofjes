#include "targa.h"

#include <cstring>
#include <cstdlib>
#include <stdexcept>


#pragma pack(push, 1)

typedef struct tgaColorMapSpec {
  uint16_t first;
  uint16_t len;
  uint8_t  size;
} tgaColorMapSpec;

typedef struct tgaImageSpec {
  uint16_t xorig;
  uint16_t yorig;
  uint16_t width;
  uint16_t height;
  uint8_t  depth;
  uint8_t  alpha:4;
  uint8_t  orig:2;
  uint8_t  _res:2;
} tgaImageSpec;

typedef struct tgaHeader {
  uint8_t         idlen;
  uint8_t         cmtype;
  uint8_t         imtype;
  tgaColorMapSpec cms;
  tgaImageSpec    is;
} tgaHeader;

typedef struct tgaFooter {
  uint32_t extoff;
  uint32_t devoff;
  char     sig[18];
} tgaFooter;

#pragma pack(pop)

#define TGA_SIGNATURE ("TRUEVISION-XFILE.")

namespace sj {

  texture_t tgaLoad(FILE* input) {
    tgaHeader head;
    tgaFooter foot;
    int32_t  len;
    std::vector<uint8_t> pixels;
    uint8_t* bgra;
    
    if (fseek(input, 0, SEEK_END) != 0) {
      throw std::runtime_error("fseek failed");
    }

    len = ftell(input);
    if (len < 0) {
      throw std::runtime_error("ftell failed");
    }

    if (fseek(input, len - sizeof(tgaFooter), SEEK_SET) != 0) {
      throw std::runtime_error("fseek failed");
    }

    if (fread(&foot, sizeof(tgaFooter), 1, input) != 1) {
      throw std::runtime_error("fread failed");
    }

    if (strcmp(foot.sig, TGA_SIGNATURE) != 0) {
      throw std::runtime_error("invalid signature");
    }

    clearerr(input);
    if (fseek(input, 0, SEEK_SET) != 0) {
      throw std::runtime_error("fseek failed");
    }

    if (fread(&head, sizeof(tgaHeader), 1, input) != 1) {
      throw std::runtime_error("fread failed");
    }

    if (head.idlen != 0) {
      throw std::runtime_error("invalid idlen");
    }

    if (head.cmtype != 0) {
      throw std::runtime_error("invalid cmtype");
    }

    if (head.imtype != 2) {
      throw std::runtime_error("invalid imtype");
    }

    if (head.cms.size != 0) {
      throw std::runtime_error("invalid cms.size");
    }

    if (head.is.depth != 32) {
      throw std::runtime_error("invalid bit depth");
    }

    pixels.resize(head.is.width*head.is.height*(head.is.depth/8));

    if (fread(pixels.data(), 4, head.is.width*head.is.height, input)
      != head.is.width*head.is.height) {
      throw std::runtime_error("fread failed");
    }

    bgra = pixels.data();

    for (uint32_t index = 0; index < head.is.width*head.is.height; ++index) {
      uint8_t temp = bgra[index * 4];
      bgra[index * 4] = bgra[index * 4 + 2];
      bgra[index * 4 + 2] = temp;
    }

    return texture_t(head.is.width, head.is.height, head.is.depth/8, std::move(pixels));
  }

}
