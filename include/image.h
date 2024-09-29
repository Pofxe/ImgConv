#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <vector>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <fstream>

namespace img_lib
{
    using namespace std::string_literals;
    using Path = std::filesystem::path;

    struct Color
    {
        Color() = default;
        Color(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_), a(255) {}
        Color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_) : r(r_), g(g_), b(b_), a(a_) {}

        static Color Black()
        {
            return { 0, 0, 0, 255 };
        }

        static Color White()
        {
            return { 255, 255, 255, 255 };
        }

        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        Color operator*(float scalar_) const
        {
            return
            {
                static_cast<uint8_t>(r * scalar_),
                static_cast<uint8_t>(g * scalar_),
                static_cast<uint8_t>(b * scalar_),
                static_cast<uint8_t>(a * scalar_)
            };
        }

        Color operator+(const Color& other_) const
        {
            return
            {
                static_cast<uint8_t>(r + other_.r),
                static_cast<uint8_t>(g + other_.g),
                static_cast<uint8_t>(b + other_.b),
                static_cast<uint8_t>(a + other_.a)
            };
        }

        Color& operator+=(const Color& other_) 
        {
            r += other_.r;
            g += other_.g;
            b += other_.b;
            a += other_.a;
            return *this;
        }
    };

    class Image
    {
    public:

        Image() = default;
        Image(int w_, int h_);
        Image(int w_, int h_, Color fill_);

        Image(const Image& other_);
        Image& operator=(const Image& other_);

        Image(Image&& other_) noexcept;
        Image& operator=(Image&& other_) noexcept;

        explicit operator bool() const
        {
            return GetWidth() > 0 && GetHeight() > 0;
        }

        bool operator!() const
        {
            return !operator bool();
        }

        const Color& GetPixel(int x_, int y_) const;
        Color& GetPixel(int x_, int y_);

        std::vector<Color>& GetPixels();
        const std::vector<Color>& GetPixels() const;

        Color* GetLine(int y_);
        const Color* GetLine(int y_) const;

        int GetWidth() const;
        int GetHeight() const;

        int GetStep() const;

        const uint8_t* GetData() const;

        void SetPixel(int x, int y, const Color& pixel);

        Image ResizeImage(int new_width_, int new_height_) const;

    private:

        int width = 0;
        int height = 0;
        int step = 0;

        std::vector<Color> pixels;

        void CheckBounds(int x_, int y_) const;
    };

}//end namespace img_lib