#include "bmp_image.h"

namespace img_lib
{
	namespace bmp_image
	{
        static int GetBMPStride(int w_)
        {
            const int bytesPerPixel = 3;
            const int alignment = 4;

            return alignment * ((w_ * bytesPerPixel + (alignment - 1)) / alignment);
        }

        const Image BmpImage::LoadImageBMP(const Path& path_)
        {
            std::ifstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Failed to open BMP file: "s + path_.string());
                return {};
            }

            BitmapFileHeader file_header;
            file.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
            if (!file)
            {
                return {};
            }
            if (file_header.file_type != 0x4D42 || !file)
            {
                return {};
            }

            BitmapInfoHeader info_header;
            file.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));
            if (!file)
            {
                return {};
            }
            if (info_header.compression != 0 || !file)
            {
                return {};
            }
            if (info_header.bit_count != 24 && info_header.bit_count != 32)
            {
                return {};
            }

            int width = info_header.width;
            int height = info_header.height;
            int stride = GetBMPStride(width);

            Image image(width, height, Color::Black());

            if (info_header.bit_count == 32)
            {
                for (int y = height - 1; y >= 0; --y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        uint8_t b = 0;
                        uint8_t g = 0;
                        uint8_t r = 0;
                        uint8_t a = 0;

                        file.read(reinterpret_cast<char*>(&b), sizeof(uint8_t));
                        file.read(reinterpret_cast<char*>(&g), sizeof(uint8_t));
                        file.read(reinterpret_cast<char*>(&r), sizeof(uint8_t));
                        file.read(reinterpret_cast<char*>(&a), sizeof(uint8_t));

                        if (!file)
                        {
                            return {};
                        }
                        Color pixel(r, g, b, a);
                        image.SetPixel(x, y, pixel);
                    }
                }
            }
            else if (info_header.bit_count == 24)
            {
                for (int y = height - 1; y >= 0; --y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        uint8_t b = 0;
                        uint8_t g = 0;
                        uint8_t r = 0;

                        file.read(reinterpret_cast<char*>(&b), sizeof(uint8_t));
                        file.read(reinterpret_cast<char*>(&g), sizeof(uint8_t));
                        file.read(reinterpret_cast<char*>(&r), sizeof(uint8_t));

                        if (!file)
                        {
                            return {};
                        }

                        Color pixel(r, g, b, 255);
                        image.SetPixel(x, y, pixel);
                    }
                }
            }
            else
            {
                throw std::runtime_error("Unsupported color depth"s);
                return {};
            }

            file.close();
            return image;
        }

        bool BmpImage::SaveImageBMP(const Path& path_, const Image& image_) const
        {
            std::ofstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Failed to create BMP file: "s + path_.string());
                return false;
            }

            int width = image_.GetWidth();
            int height = image_.GetHeight();
            int row_stride = width * 3;
            int stride = GetBMPStride(width);

            BitmapFileHeader file_header;
            file_header.file_type = 0x4D42;
            file_header.file_size = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + stride * height;
            file_header.reserved1 = 0;
            file_header.reserved2 = 0;
            file_header.offset_data = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);

            BitmapInfoHeader info_header;
            info_header.size = sizeof(BitmapInfoHeader);
            info_header.width = width;
            info_header.height = height;
            info_header.planes = 1;
            info_header.bit_count = 24;
            info_header.compression = 0;
            info_header.image_size = stride * height;
            info_header.x_pixels_per_meter = 11811; // 300 DPI
            info_header.y_pixels_per_meter = 11811; // 300 DPI
            info_header.colors_used = 0;
            info_header.colors_important = 0x1000000;

            file.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
            if (!file)
            {
                return false;
            }

            file.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));
            if (!file)
            {
                return false;
            }

            std::vector<uint8_t> row(stride);

            for (int y = height - 1; y >= 0; --y)
            {
                const Color* row_data = image_.GetLine(y);
                for (int x = 0; x < width; ++x)
                {
                    row[x * 3 + 0] = static_cast<uint8_t>(row_data[x].b);
                    row[x * 3 + 1] = static_cast<uint8_t>(row_data[x].g);
                    row[x * 3 + 2] = static_cast<uint8_t>(row_data[x].r);
                }
                for (int x = row_stride; x < stride; ++x)
                {
                    row[x] = 0;
                }

                file.write(reinterpret_cast<const char*>(row.data()), stride);
                if (!file)
                {
                    return false;
                }
            }

            file.close();
            return true;
        }    

    } // end namespace bmp_image

} // end namespace img_lib