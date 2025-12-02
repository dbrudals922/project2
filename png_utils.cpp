#include "png_utils.hpp"
#include <cstdio>
#include <cstdlib>

bool LoadPNG(const std::string& filename, PNGImage& out)
{
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) return false;

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    if (setjmp(png_jmpbuf(png))) return false;

    png_init_io(png, fp);
    png_read_info(png, info);

    out.width = png_get_image_width(png, info);
    out.height = png_get_image_height(png, info);
    out.color_type = png_get_color_type(png, info);
    out.bit_depth = png_get_bit_depth(png, info);

    if (out.bit_depth == 16) png_set_strip_16(png);
    if (out.color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (out.color_type == PNG_COLOR_TYPE_GRAY && out.bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if (out.color_type == PNG_COLOR_TYPE_RGB || out.color_type == PNG_COLOR_TYPE_GRAY || out.color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if (out.color_type == PNG_COLOR_TYPE_GRAY || out.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    out.row_pointers.resize(out.height);
    for (int y = 0; y < out.height; y++)
        out.row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png, info));

    png_read_image(png, out.row_pointers.data());

    fclose(fp);
    png_destroy_read_struct(&png, &info, NULL);
    return true;
}

SDL_Texture* PNGToTexture(SDL_Renderer* renderer, const PNGImage& img)
{
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, img.width, img.height, 32, SDL_PIXELFORMAT_RGBA32);
    for (int y = 0; y < img.height; y++)
    {
        png_bytep row = img.row_pointers[y];
        for (int x = 0; x < img.width; x++)
        {
            png_bytep px = &(row[x*4]);
            uint32_t* dest = (uint32_t*)((uint8_t*)surf->pixels + y*surf->pitch + x*4);
            *dest = SDL_MapRGBA(surf->format, px[0], px[1], px[2], px[3]);
        }
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return tex;
}

void ApplyFixedColorStyle(PNGImage& img, int style)
{
    for(int y=0; y<img.height; y++)
    {
        png_bytep row = img.row_pointers[y];
        for(int x=0; x<img.width; x++)
        {
            png_bytep px = &(row[x*4]);

            if(px[3] < 10) continue; // 투명한 픽셀 XX

            if(px[0]==0 && px[1]==0 && px[2]==0) continue; // 검정색 테두리 남기기

            switch(style)
            {
                case 0: px[0]=255; px[1]=0;   px[2]=0;   break; 
                case 1: px[0]=255; px[1]=255; px[2]=0;   break; 
                case 2: px[0]=0;   px[1]=0;   px[2]=255; break; 
                case 3: px[0]=255; px[1]=0;   px[2]=255; break; 
                case 4: px[0]=0;   px[1]=255; px[2]=0;   break; 
            }
        }
    }
}
