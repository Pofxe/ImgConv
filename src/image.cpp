#include "image.h"

namespace img_lib
{
    Image::Image(int w_, int h_, Color fill_) : width(w_), height(h_), step(w_), pixels(w_* h_, fill_) {}

    Image::Image(const Image& other_) : width(other_.width), height(other_.height), step(other_.step), pixels(other_.pixels) {}

    Image::Image(Image&& other_) noexcept : width(other_.width), height(other_.height), step(other_.step), pixels(std::move(other_.pixels))
    {
        other_.width = 0;
        other_.height = 0;
        other_.step = 0;
    }

    Image& Image::operator=(const Image& other_)
    {
        if (this != &other_)
        {
            width = other_.width;
            height = other_.height;
            step = other_.step;
            pixels = other_.pixels;
        }
        return *this;
    }

    Image& Image::operator=(Image&& other_) noexcept
    {
        if (this != &other_)
        {
            width = other_.width;
            height = other_.height;
            step = other_.step;
            pixels = std::move(other_.pixels);

            other_.width = 0;
            other_.height = 0;
            other_.step = 0;
        }
        return *this;
    }

    const Color& Image::GetPixel(int x_, int y_) const
    {
        CheckBounds(x_, y_);
        return pixels[y_ * step + x_];
    }

    Color& Image::GetPixel(int x_, int y_)
    {
        CheckBounds(x_, y_);
        return pixels[y_ * step + x_];
    }

    void Image::SetPixel(int x_, int y_, const Color& pixel_)
    {
        pixels[y_ * width + x_] = pixel_;
    }

    std::vector<Color>& Image::GetPixels()
    {
        return pixels;
    }

    const std::vector<Color>& Image::GetPixels() const
    {
        return pixels;
    }

    Color* Image::GetLine(int y_)
    {
        CheckBounds(0, y_);
        return &pixels[y_ * step];
    }

    const Color* Image::GetLine(int y_) const
    {
        CheckBounds(0, y_);
        return &pixels[y_ * step];
    }

    int Image::GetWidth() const
    {
        return width;
    }

    int Image::GetHeight() const
    {
        return height;
    }

    int Image::GetStep() const
    {
        return step;
    }

    const uint8_t* Image::GetData() const
    {
        if (pixels.empty())
        {
            return nullptr;
        }
        return reinterpret_cast<const uint8_t*>(pixels.data());
    }

    Image Image::ResizeImage(int newWidth_, int newHeight_) const 
    {
        Image newImage(newWidth_, newHeight_);

        for (int y = 0; y < newHeight_; ++y) 
        {
            for (int x = 0; x < newWidth_; ++x) 
            {
                float gx = static_cast<float>(x) / newWidth_ * (width - 1);
                float gy = static_cast<float>(y) / newHeight_ * (height - 1);

                Color interpolatedColor = LanczosInterpolation(gx, gy);
                newImage.SetPixel(x, y, interpolatedColor); // !!! out_of_range !!!
            }
        }
        return newImage;
    }

    Color Image::LanczosInterpolation(float x_, float y_, int a_) const 
    {
        int xi = static_cast<int>(x_);
        int yi = static_cast<int>(y_);

        Color result(0, 0, 0, 0);
        float totalWeight = 0;

        for (int dy = -a_ + 1; dy <= a_; ++dy) 
        {
            for (int dx = -a_ + 1; dx <= a_; ++dx) 
            {
                int px = xi + dx;
                int py = yi + dy;

                if (px >= 0 && px < width && py >= 0 && py < height) 
                {
                    Color c = GetPixel(px, py);
                    float wx = LanczosWeight(x_ - (xi + dx), a_);
                    float wy = LanczosWeight(y_ - (yi + dy), a_);
                    float weight = wx * wy;

                    result += c * weight;
                    totalWeight += weight;
                }
            }
        }
        if (totalWeight > 0) 
        {
            result = result * (1.0f / totalWeight);
        }

        return result;
    }

    float Image::LanczosWeight(float t_, int a_) const 
    {
        static const int PI = 3.1415926535;
        if (t_ == 0)
        {
            return 1;
        }
        if (t_ > -a_ && t_ < a_)
        {
            return a_ * sin(PI * t_) * sin(PI * t_ / a_) / (PI * PI * t_ * t_);
        }
        return 0;
    }

    void Image::CheckBounds(int x_, int y_) const
    {
        if (x_ < 0 || x_ >= width || y_ < 0 || y_ >= height)
        {
            throw std::out_of_range("Pixel coordinates out of range"s);
        }
    }

}//end namespace img_lib