#include "ico_image.h"

#include <algorithm>

namespace img_lib
{
    namespace ico_image
    {
        const Image IcoImage::LoadImageICO(const Path& path_) // the image is not loading correctly
        {
            std::ifstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Load file is not open: "s + path_.string());
                return {};
            }

            IcoHeader header{};
            file.read(reinterpret_cast<char*>(&header), sizeof(IcoHeader));
            if (!file)
            {
                return {};
            }

            if (header.reserved != 0 || header.type != 1)
            {
                throw std::runtime_error("Incorrect ICO file"s);
                return {};
            }

            if (header.count == 0)
            {
                throw std::runtime_error("The ICO file does not contain images"s);
                return {};
            }

            IconDirEntry entry{};
            file.read(reinterpret_cast<char*>(&entry), sizeof(IconDirEntry));
            if (!file)
            {
                return {};
            }

            int width = entry.width == 0 ? 256 : entry.width;
            int height = entry.height == 0 ? 256 : entry.height;
            uint32_t bit_count = entry.bit_count;

            file.seekg(entry.offset, std::ios::beg);

            BmpHeader bmpHeader{};
            file.read(reinterpret_cast<char*>(&bmpHeader), sizeof(BmpHeader));
            if (!file)
            {
                return {};
            }

            if (bmpHeader.biBitCount != bit_count)
            {
                throw std::runtime_error("Mismatch between bit count in directory entry and BMP header"s);
                return {};
            }

            Image image(width, height, Color::Black());

            if (bit_count == 32) // it works not correctly
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
            else if (bit_count == 24)
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
            return image.ResizeImage(360, 360);
        }

        bool IcoImage::SaveImageICO(const Path& path_, const Image& image_) const // it works correctly for a single image & large file weight (will decrease when there is a function ResizeImage)
        {
            std::ofstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Failed to create ICO file: "s + path_.string());
                return false;
            }

            IcoHeader header = { 0, 1, 1 };
            file.write(reinterpret_cast<const char*>(&header), sizeof(IcoHeader));
            if (!file)
            {
                return false;
            }

            int width = image_.GetWidth();
            int height = image_.GetHeight();

            IconDirEntry entry{};
            entry.width = static_cast<uint8_t>(width > 256 ? 0 : width);
            entry.height = static_cast<uint8_t>(height > 256 ? 0 : height);
            entry.color_count = 0;
            entry.reserved = 0;
            entry.planes = 1;
            entry.bit_count = 32;
            entry.size = static_cast<uint32_t>(sizeof(BmpHeader) + (width * height * 4));
            entry.offset = static_cast<uint32_t>(sizeof(IcoHeader) + sizeof(IconDirEntry));

            file.write(reinterpret_cast<const char*>(&entry), sizeof(IconDirEntry));
            if (!file)
            {
                return false;
            }

            BmpHeader bmpHeader{};
            bmpHeader.biSize = sizeof(BmpHeader);
            bmpHeader.biWidth = width;
            bmpHeader.biHeight = height * 2;
            bmpHeader.biPlanes = 1;
            bmpHeader.biBitCount = 32;
            bmpHeader.biCompression = 0;
            bmpHeader.biSizeImage = static_cast<uint32_t>(width * height * 4);
            bmpHeader.biXPelsPerMeter = 0;
            bmpHeader.biYPelsPerMeter = 0;
            bmpHeader.biClrUsed = 0;
            bmpHeader.biClrImportant = 0;

            file.write(reinterpret_cast<const char*>(&bmpHeader), sizeof(BmpHeader));
            if (!file)
            {
                return false;
            }

            for (int y = height - 1; y >= 0; --y)
            {
                for (int x = 0; x < width; ++x)
                {
                    Color pixel = image_.GetPixel(x, y);

                    uint8_t r = pixel.r;
                    uint8_t g = pixel.g;
                    uint8_t b = pixel.b;
                    uint8_t a = pixel.a;

                    file.write(reinterpret_cast<const char*>(&b), sizeof(uint8_t));
                    file.write(reinterpret_cast<const char*>(&g), sizeof(uint8_t));
                    file.write(reinterpret_cast<const char*>(&r), sizeof(uint8_t));
                    file.write(reinterpret_cast<const char*>(&a), sizeof(uint8_t));

                    if (!file)
                    {
                        return false;
                    }
                }
            }

            file.close();
            return true;
        }

    } // end namespace ico_image

} // end namespace img_lib