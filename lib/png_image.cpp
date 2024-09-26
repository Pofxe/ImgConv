#include "png_image.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <setjmp.h>

namespace img_lib
{
    namespace png_image
    {
<<<<<<< Updated upstream:lib/png_image.cpp
        Image PngImage::LoadImagePng(const Path& path_)
=======
        void SaveScanlineToImage(const png_bytep row_, int y, Image& out_image_){
            Color* line = out_image_.GetLine(y);
            int channels = 4;
            for (int x = 0; x < out_image_.GetWidth(); ++x){
                const png_bytep pixel = row_ + x * channels;
                line[x] = Color{pixel[0], pixel[1], pixel[2], pixel[3]};
            }
        }

        Image PngImage::LoadImagePNG(const Path& path_)
>>>>>>> Stashed changes:src/png_image.cpp
        {
            FILE* file;
            #ifdef _MSC_VER
            if ((file = _wfopen(path_.wstring().c_str(), L"rb")) == NULL)
            {
            #else
            if ((file = fopen(path_.string().c_str(), "rb")) == NULL) 
            {
            #endif
                return {};
            }
            png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            if (!png){
                return {};
            }
            
            png_infop info = png_create_info_struct(png);
            if (!info){
                return {};
            }

            png_init_io(png, file);
            png_read_info(png, info);

            /*int width = png_get_image_width(png, info);
            int height = png_get_image_height(png, info);
            png_byte color_type = png_get_color_type(png,info);
            png_byte bit_depth = png_get_bit_depth(png,info);

            if (bit_depth == 16){
                png_set_strip_16(png);
            }

            if (color_type == PNG_COLOR_TYPE_PALETTE){
                png_set_palette_to_rgb(png);
            }
            
            if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8){
                png_set_expand_gray_1_2_4_to_8(png);
            }
            if (png_get_valid(png, info, PNG_INFO_tRNS)){
                png_set_tRNS_to_alpha(png);
            }

            if (color_type == PNG_COLOR_TYPE_RGB || 
            color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_PALETTE){
                png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
            }

            if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA){
                png_set_gray_to_rgb(png);
            }

            png_read_update_info(png, info);

            png_bytepp row_pointers = png_get_rows(png, info);
            png_read_image(png,row_pointers);

            Image image(width, height, Color::Black());
            for (int y = 0; y < height; ++y){
                SaveScanlineToImage(row_pointers[y], y, image);
            }

            for (int y = 0; y < height; ++y){
                free(row_pointers[y]);
            }
            free(row_pointers);
            png_destroy_read_struct(&png, &info, NULL);
            fclose(file);*/

            return {};
        }

        bool PngImage::SaveImagePng(const Path& path_, const Image& image_)
        {
            return false;
        }

    } // end namespace png_image

} // end namespace img_lib