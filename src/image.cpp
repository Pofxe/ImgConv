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

    void Image::CheckBounds(int x_, int y_) const
    {
        if (x_ < 0 || x_ >= width || y_ < 0 || y_ >= height)
        {
            throw std::out_of_range("Pixel coordinates out of range"s);
        }
    }

}//end namespace img_lib