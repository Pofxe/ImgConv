#include "ico_image.h"

#include <algorithm>

namespace img_lib
{
    namespace ico_image
    {
        Image IcoImage::LoadImageICO(const Path& path_)
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
                throw std::runtime_error("Unsupported color depth: "s);
                return {};
            }

            file.close();
            return image;
        }

        Image IcoImage::Resize(int newWidth_, int newHeight_, const Image& image_) 
        {
            Image resized(newWidth_, newHeight_);

            float x_ratio = static_cast<float>(image_.GetWidth()) / newWidth_;
            float y_ratio = static_cast<float>(image_.GetHeight()) / newHeight_;

            for (int y = 0; y < newHeight_; ++y) {
                for (int x = 0; x < newWidth_ - 1; ++x) {
                    float x_pos = x * x_ratio;
                    float y_pos = y * y_ratio;

                    int x1 = static_cast<int>(x_pos);
                    int y1 = static_cast<int>(y_pos);
                    int x2 = std::min(x1 + 1, image_.GetWidth() - 1);
                    int y2 = std::min(y1 + 1, image_.GetHeight() - 1);

                    float x_diff = x_pos - x1;
                    float y_diff = y_pos - y1;

                    Color c1 = image_.GetPixel(x1, y1);
                    Color c2 = image_.GetPixel(x2, y1);
                    Color c3 = image_.GetPixel(x1, y2);
                    Color c4 = image_.GetPixel(x2, y2);

                    Color top = c1 * (1.0f - x_diff) + c2 * x_diff;
                    Color bottom = c3 * (1.0f - x_diff) + c4 * x_diff;

                    Color final_color = top * (1.0f - y_diff) + bottom * y_diff;

                    resized.SetPixel(x, y, final_color); // out_of_range 
                }
            }

            return resized;
        }

        bool IcoImage::SaveImageICO(const Path& path_, const Image& image_) // the method does not work correctly
        {
            std::ofstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Failed to create ICO file: "s + path_.string());
                return false;
            }

            std::vector<std::pair<int, int>> sizes = { {16, 16}, {24, 24}, {32, 32}, {48, 48}, {64, 64}, {96, 96}, {128, 128}, {256, 256} };
            std::vector<Image> images;

            /*for (const auto& size : sizes)
            {
                images.push_back(Resize(size.first, size.second, image_));
            }*/

            IcoHeader header{ 0, 1, static_cast<uint16_t>(images.size()) };
            file.write(reinterpret_cast<const char*>(&header), sizeof(IcoHeader));
            if (!file)
            {
                return false;
            }

            std::vector<IconDirEntry> entries;
            std::vector<BmpHeader> bmp_headers;

            uint32_t offset = sizeof(IcoHeader) + sizeof(IconDirEntry) * images.size();

            for (const Image& image : images)
            {
                int width = image.GetWidth();
                uint32_t bytes_in_res = width * width * 4 + sizeof(BmpHeader);
                uint32_t size_image = width * width * 4;

                IconDirEntry entry{ static_cast<uint8_t>(width), static_cast<uint8_t>(width), 0, 0, 1, 32, bytes_in_res, offset };

                entries.push_back(entry);

                BmpHeader bmp_header
                {
                    0x4D42,
                    static_cast<uint32_t>(sizeof(BmpHeader) + size_image),
                    0, 0, sizeof(BmpHeader), 40, width,
                    width * 2, 1, 32, 0, size_image, 0, 0, 0, 0
                };

                bmp_headers.push_back(bmp_header);

                offset += bytes_in_res;
            }

            for (const auto& entry : entries)
            {
                file.write(reinterpret_cast<const char*>(&entry), sizeof(IconDirEntry));
                if (!file)
                {
                    return false;
                }
            }

            for (size_t i = 0; i < images.size(); ++i)
            {
                const auto& image = images[i];
                const auto& bmp_header = bmp_headers[i];

                file.write(reinterpret_cast<const char*>(&bmp_header), sizeof(BmpHeader));
                if (!file)
                {
                    return false;
                }

                for (int y = image.GetHeight() - 1; y >= 0; --y)
                {
                    for (int x = 0; x < image.GetWidth(); ++x)
                    {
                        const Color& pixel = image.GetPixel(x, y);
                        file.write(reinterpret_cast<const char*>(&pixel.b), 1);
                        file.write(reinterpret_cast<const char*>(&pixel.g), 1);
                        file.write(reinterpret_cast<const char*>(&pixel.r), 1);
                        file.write(reinterpret_cast<const char*>(&pixel.a), 1);
                        if (!file)
                        {
                            return false;
                        }
                    }
                }
            }

            file.close();
            return true;
        }

	} // end namespace ico_image

} // end namespace img_lib