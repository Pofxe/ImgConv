#include "image.h"

namespace img_lib
{
    Image::Image(int w_, int h_) : width(w_), height(h_), step(w_), pixels(w_ * h_) {}

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

    Image Image::ResizeImage(int new_width_, int new_height_) const
    {
        Image resizedImage(new_width_, new_height_);

        int oldWidth = GetWidth();
        int oldHeight = GetHeight();

        for (int y = 0; y < new_height_; ++y)
        {
            for (int x = 0; x < new_width_; ++x)
            {
                float srcX = x * static_cast<float>(oldWidth) / static_cast<float>(new_width_);
                float srcY = y * static_cast<float>(oldHeight) / static_cast<float>(new_height_);

                int x1 = static_cast<int>(srcX);
                int y1 = static_cast<int>(srcY);
                int x2 = std::min(x1 + 1, oldWidth - 1);
                int y2 = std::min(y1 + 1, oldHeight - 1);

                float dx = srcX - x1;
                float dy = srcY - y1;

                Color p1 = GetPixel(x1, y1);
                Color p2 = GetPixel(x2, y1);
                Color p3 = GetPixel(x1, y2);
                Color p4 = GetPixel(x2, y2);

                Color newColor;
                newColor.r = static_cast<uint8_t>((1 - dx) * (1 - dy) * p1.r + dx * (1 - dy) * p2.r + (1 - dx) * dy * p3.r + dx * dy * p4.r);
                newColor.g = static_cast<uint8_t>((1 - dx) * (1 - dy) * p1.g + dx * (1 - dy) * p2.g + (1 - dx) * dy * p3.g + dx * dy * p4.g);
                newColor.b = static_cast<uint8_t>((1 - dx) * (1 - dy) * p1.b + dx * (1 - dy) * p2.b + (1 - dx) * dy * p3.b + dx * dy * p4.b);
                newColor.a = static_cast<uint8_t>((1 - dx) * (1 - dy) * p1.a + dx * (1 - dy) * p2.a + (1 - dx) * dy * p3.a + dx * dy * p4.a);

                resizedImage.SetPixel(x, y, newColor);
            }
        }
        return resizedImage;
    }

    void Image::CheckBounds(int x_, int y_) const
    {
        if (x_ < 0 || x_ >= width || y_ < 0 || y_ >= height)
        {
            throw std::out_of_range("Pixel coordinates out of range"s);
        }
    }

}//end namespace img_lib