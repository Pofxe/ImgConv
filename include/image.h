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

    struct Size
    {
        int width = 0;
        int height = 0;
    };

    struct Color
    {
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
    };

    class Image
    {
    public:

        Image() = default;
        Image(int w_, int h_) : width(w_), height(h_) {}
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

        void SetPixel(int x, int y, const Color& pixel);

        Color* GetLine(int y_);
        const Color* GetLine(int y_) const;

        int GetWidth() const;
        int GetHeight() const;

        int GetStep() const;

    private:

        int width = 0;
        int height = 0;
        int step = 0;

        std::vector<Color> pixels;

        void CheckBounds(int x_, int y_) const;
    };

}//end namespace img_lib