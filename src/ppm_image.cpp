#include "ppm_image.h"

namespace img_lib
{
    namespace ppm_image
    {
        Image PpmImage::LoadImagePPM(const Path& file_)
        {
            std::ifstream file(file_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Load file is not open"s);
                return {};
            }

            std::string ppm_type = ""s;
            file >> ppm_type;

            if (ppm_type == PPM_TYPE_P3)
            {
                return LoadP3(file_);
            }
            else if (ppm_type == PPM_TYPE_P6)
            {
                return LoadP6(file_);
            }
            else
            {
                throw std::runtime_error("Unsupported PPM format"s);
                return {};
            }
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

        Image PpmImage::LoadP3(const Path& file_)
        {
            std::ifstream file(file_);
            if (!file)
            {
                throw std::runtime_error("P3 load file is not open"s);
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
            return image;
        }

        bool PpmImage::SaveP3(const Path& file_, const Image& image_)
        {
            std::ofstream out(file_);
            if (!out)
            {
                throw std::runtime_error("Save file is not open"s);
                return false;
            }

            out << PPM_TYPE_P3 << '\n' << image_.GetWidth() << ' ' << image_.GetHeight() << '\n' << PPM_MAX << '\n';

            const int w = image_.GetWidth();
            const int h = image_.GetHeight();

            for (int y = 0; y < h; ++y)
            {
                const Color* line = image_.GetLine(y);

                for (int x = 0; x < w; ++x)
                {
                    const Color& color = line[x];

                    out << color.r << ' ' << color.g << ' ' << color.b << ' ';
                }
                out << '\n';
            }
            return out.good();
        }

        Image PpmImage::LoadP6(const Path& file)
        {
            std::ifstream file_(file, std::ios::binary);
            if (!file_)
            {
                throw std::runtime_error("P6 load file is not open"s);
                return {};
            }

            std::string sign = ""s;
            int w = 0;
            int h = 0;
            int color_max = 0;

            file_ >> sign;
            if (sign != PPM_TYPE_P6)
            {
                return {};
            }
            
            file_ >> w >> h;
            
            file_ >> color_max;
            if (color_max != PPM_MAX)
            {
                return {};
            }

            const char next = file_.get();
            if (next != '\n')
            {
                return {};
            }

            Image result(w, h, Color::Black());
            std::vector<char> buff(w * 3);

            for (int y = 0; y < h; ++y)
            {
                Color* line = result.GetLine(y);

                file_.read(buff.data(), w * 3);
                if (!file_)
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
            return result;
        }

        bool PpmImage::SaveP6(const Path& file_, const Image& image_)
        {
            std::ofstream out(file_, std::ios::binary);
            if (!out)
            {
                throw std::runtime_error("Save file is not open"s);
                return false;
            }

            out << PPM_TYPE_P6 << '\n' << image_.GetWidth() << ' ' << image_.GetHeight() << '\n' << PPM_MAX << '\n';

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
                out.write(buff.data(), w * 3);
                if (!out)
                {
                    return false;
                }
            }
            return out.good();
        }

    } // end namespace ppm_image

} // end namespace img_lib