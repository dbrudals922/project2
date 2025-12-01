#ifndef PNG_UTILS_HPP
#define PNG_UTILS_HPP

#include <png.h>
#include <vector>
#include <string>
#include <SDL2/SDL.h>

struct PNGImage {
    int width;
    int height;
    png_byte color_type;
    png_byte bit_depth;
    std::vector<png_bytep> row_pointers;
};

bool LoadPNG(const std::string& filename, PNGImage& out);
SDL_Texture* PNGToTexture(SDL_Renderer* renderer, const PNGImage& img);
void ApplyFixedColorStyle(PNGImage& img, int style);

#endif