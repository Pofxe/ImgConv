#include "ppm_image.h"

namespace img_lib
{
    namespace ppm_image
    {
        Image PpmImage::LoadImagePPM(const Path& path_)
        {
            std::ifstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Failed to open PPM/P3 file: "s + path_.string());
                return {};
            }

            std::string ppm_type = ""s;
            file >> ppm_type;

            if (ppm_type == PPM_TYPE_P3)
            {
                return LoadP3(path_);
            }
            else if (ppm_type == PPM_TYPE_P6)
            {
                return LoadP6(path_);
            }
            else
            {
                throw std::runtime_error("Unsupported PPM format"s);
                return {};
            }

            file.close();
        }

        bool PpmImage::SaveImagePPM(const Path& file_, const Image& image_)
        {
            std::string extension = file_.extension().string();
            if (extension == ".p3"s)
            {
                return SaveP3(file_, image_);
            }
            else if (extension == ".ppm"s)
            {
                return SaveP6(file_, image_);
            }
            else
            {
                throw std::runtime_error("Unsupported PPM format"s);
                return false;
            }
        }

        Image PpmImage::LoadP3(const Path& path_)
        {
            std::ifstream file(path_);
            if (!file)
            {
                throw std::runtime_error("Failed to open P3 file: "s + path_.string());
                return {};
            }

            std::string sign = ""s;
            int width = 0;
            int height = 0;
            int max_color = 0;
            file >> sign >> width >> height >> max_color;

            if (max_color != PPM_MAX)
            {
                throw std::runtime_error("Unsupported max color value "s);
                return {};
            }

            Image image(width, height, Color::Black());

            for (int y = 0; y < height; ++y)
            {
                Color* line = image.GetLine(y);
                for (int x = 0; x < width; ++x)
                {
                    uint8_t r = 0;
                    uint8_t g = 0;
                    uint8_t b = 0;

                    file >> r >> g >> b;
                    line[x] = { r, g, b, 255 };
                }
            }

            file.close();
            return image;
        }

        bool PpmImage::SaveP3(const Path& path_, const Image& image_)
        {
            std::ofstream file(path_);
            if (!file)
            {
                throw std::runtime_error("Failed to create P3 file: "s + path_.string());
                return false;
            }

            file << PPM_TYPE_P3 << '\n' << image_.GetWidth() << ' ' << image_.GetHeight() << '\n' << PPM_MAX << '\n';

            const int w = image_.GetWidth();
            const int h = image_.GetHeight();

            for (int y = 0; y < h; ++y)
            {
                const Color* line = image_.GetLine(y);

                for (int x = 0; x < w; ++x)
                {
                    const Color& color = line[x];

                    file << color.r << ' ' << color.g << ' ' << color.b << ' ';
                }
                file << '\n';
            }

            file.close();
            return file.good();
        }

        Image PpmImage::LoadP6(const Path& path_)
        {
            std::ifstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Failed to open PPM file: "s + path_.string());
                return {};
            }

            std::string sign = ""s;
            int w = 0;
            int h = 0;
            int color_max = 0;

            file >> sign;
            if (sign != PPM_TYPE_P6)
            {
                return {};
            }
            
            file >> w >> h;
            
            file >> color_max;
            if (color_max != PPM_MAX)
            {
                return {};
            }

            const char next = file.get();
            if (next != '\n')
            {
                return {};
            }

            Image image(w, h, Color::Black());
            std::vector<char> buff(w * 3);

            for (int y = 0; y < h; ++y)
            {
                Color* line = image.GetLine(y);

                file.read(buff.data(), w * 3);
                if (!file)
                {
                    return {};
                }

                for (int x = 0; x < w; ++x)
                {
                    line[x].r = buff[x * 3 + 0];
                    line[x].g = buff[x * 3 + 1];
                    line[x].b = buff[x * 3 + 2];
                }
            }

            file.close();
            return image;
        }

        bool PpmImage::SaveP6(const Path& path_, const Image& image_)
        {
            std::ofstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Failed to create PPM file: "s + path_.string());
                return false;
            }

            file << PPM_TYPE_P6 << '\n' << image_.GetWidth() << ' ' << image_.GetHeight() << '\n' << PPM_MAX << '\n';

            const int w = image_.GetWidth();
            const int h = image_.GetHeight();
            std::vector<char> buff(w * 3);

            for (int y = 0; y < h; ++y)
            {
                const Color* line = image_.GetLine(y);

                for (int x = 0; x < w; ++x)
                {
                    buff[x * 3 + 0] = line[x].r;
                    buff[x * 3 + 1] = line[x].g;
                    buff[x * 3 + 2] = line[x].b;
                }
                file.write(buff.data(), w * 3);
                if (!file)
                {
                    return false;
                }
            }
            file.close();
            return file.good();
        }

    } // end namespace ppm_image

} // end namespace img_lib