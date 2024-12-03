#include "png_image.h"

#include <setjmp.h>
#include <stdexcept>

namespace img_lib
{
    namespace png_image
    {
        const Image PngImage::LoadImagePNG(const Path& path_)
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

            if (!file)
            {
                throw std::runtime_error("Failed to open file for reading: " + path_.string());
            }

            png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
            if (!png)
            {
                fclose(file);
                throw std::runtime_error("Failed to create PNG read struct");
            }

            png_infop info = png_create_info_struct(png);
            if (!info)
            {
                png_destroy_read_struct(&png, nullptr, nullptr);
                fclose(file);
                throw std::runtime_error("Failed to create PNG info struct");
            }

            if (setjmp(png_jmpbuf(png)))
            {
                png_destroy_read_struct(&png, &info, nullptr);
                fclose(file);
                throw std::runtime_error("Error during PNG read");
            }

            png_init_io(png, file);

            png_read_info(png, info);

            int width = png_get_image_width(png, info);
            int height = png_get_image_height(png, info);
            png_byte color_type = png_get_color_type(png, info);
            png_byte bit_depth = png_get_bit_depth(png, info);

            if (bit_depth == 16)
            {
                png_set_strip_16(png);
            }

            if (color_type == PNG_COLOR_TYPE_PALETTE)
            {
                png_set_palette_to_rgb(png);
            }

            if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
            {
                png_set_expand_gray_1_2_4_to_8(png);
            }

            if (png_get_valid(png, info, PNG_INFO_tRNS))
            {
                png_set_tRNS_to_alpha(png);
            }

            if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
            {
                png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
            }

            if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            {
                png_set_gray_to_rgb(png);
            }
            png_read_update_info(png, info);

            std::vector<png_bytep> row_pointers(height);
            std::vector<Color> pixels(width * height);

            for (int y = 0; y < height; y++)
            {
                row_pointers[y] = reinterpret_cast<png_bytep>(&pixels[y * width]);
            }

            png_read_image(png, row_pointers.data());

            fclose(file);

            png_destroy_read_struct(&png, &info, nullptr);

            Image image(width, height);
            image.GetPixels() = std::move(pixels);
            return image;
        }
        
        bool PngImage::SaveImagePNG(const Path& path_, const Image& image_) const
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

            if (!file)
            {
                throw std::runtime_error("Failed to open file for writing: " + path_.string());
            }

            png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
            if (!png)
            {
                fclose(file);
                throw std::runtime_error("Failed to create PNG write struct");
            }

            png_infop info = png_create_info_struct(png);
            if (!info)
            {
                png_destroy_write_struct(&png, nullptr);
                fclose(file);
                throw std::runtime_error("Failed to create PNG info struct");
            }

            if (setjmp(png_jmpbuf(png)))
            {
                png_destroy_write_struct(&png, &info);
                fclose(file);
                throw std::runtime_error("Error during PNG write");
            }

            png_init_io(png, file);

            int width = image_.GetWidth();
            int height = image_.GetHeight();

            png_set_IHDR(
                png,
                info,
                width, height,
                8,
                PNG_COLOR_TYPE_RGBA,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT
            );
            png_write_info(png, info);

            std::vector<png_bytep> row_pointers(height);
            std::vector<png_byte> temp_buffer(width * height * 4);

            for (int y = 0; y < height; y++)
            {
                const Color* row = image_.GetLine(y);
                png_bytep row_pointer = &temp_buffer[y * width * 4];
                
                for (int x = 0; x < width; x++)
                {
                    const Color& pixel = row[x];
                    row_pointer[x * 4 + 0] = pixel.r;
                    row_pointer[x * 4 + 1] = pixel.g;
                    row_pointer[x * 4 + 2] = pixel.b;
                    row_pointer[x * 4 + 3] = pixel.a;
                }
                row_pointers[y] = row_pointer;
            }

            png_write_image(png, row_pointers.data());
            png_write_end(png, nullptr);

            fclose(file);

            png_destroy_write_struct(&png, &info);

            return true;
        }

    } // end namespace png_image

} // end namespace img_lib