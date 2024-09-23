#include "ico_image.h"

#include <algorithm>

namespace img_lib
{
    namespace ico_image
    {
        Image IcoImage::LoadImageICO(const Path& path_) // the file is not always loaded (in particular when there is one image)
        {
            std::ifstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Load file is not open: "s + path_.string());
                return {};
            }

            IcoHeader header;
            file.read(reinterpret_cast<char*>(&header), sizeof(IcoHeader));

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

            IconDirEntry entry;
            file.read(reinterpret_cast<char*>(&entry), sizeof(IconDirEntry));

            file.seekg(entry.offset, std::ios::beg);
            std::vector<uint8_t> image_data(entry.size);
            file.read(reinterpret_cast<char*>(image_data.data()), entry.size);

            int width = entry.width == 0 ? 256 : entry.width;
            int height = entry.height == 0 ? 256 : entry.height;
            uint32_t bit_count = entry.bit_count;

            Image image(width, height, Color::Black());
            std::vector<uint8_t> row(width * height);

            if (bit_count == 32)
            {
                for (int y = height - 1; y >= 0; --y)
                {
                    file.read(reinterpret_cast<char*>(row.data()), width * height);
                    if (!file)
                    {
                        return {};
                    }

                    Color* row_data = image.GetLine(y);

                    for (int x = 0; x < width; ++x)
                    {
                        row_data[x].b = row[x * 4 + 0];
                        row_data[x].g = row[x * 4 + 1];
                        row_data[x].r = row[x * 4 + 2];
                        row_data[x].a = row[x * 4 + 3];
                    }
                }
            }
            else if (bit_count == 24)
            {
                for (int y = height - 1; y >= 0; --y)
                {
                    file.read(reinterpret_cast<char*>(row.data()), width * height);
                    if (!file)
                    {
                        return {};
                    }

                    Color* row_data = image.GetLine(y);

                    for (int x = 0; x < width; ++x)
                    {
                        row_data[x].b = row[x * 4 + 0];
                        row_data[x].g = row[x * 4 + 1];
                        row_data[x].r = row[x * 4 + 2];
                        row_data[x].a = 255;
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

        bool IcoImage::SaveImageICO(const Path& path_, const Image& image_) // the color of the final image does not match the original one
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

            Image image{ image_.ResizeImage(16, 16) };
            int width = image.GetWidth();
            int height = image.GetHeight();

            IconDirEntry entry = {};
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

            BmpHeader bmpHeader = {};
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
                    file.write(reinterpret_cast<const char*>(&pixel), sizeof(Color));
                    if (!file)
                    {
                        return false;
                    }
                }
            }

            for (int y = height - 1; y >= 0; --y)
            {
                for (int x = 0; x < width; ++x)
                {
                    Color pixel = image_.GetPixel(x, y);
                    uint8_t alpha = pixel.a;
                    uint8_t mask = (alpha == 0) ? 0 : 1;
                    file.write(reinterpret_cast<const char*>(&mask), sizeof(uint8_t));
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