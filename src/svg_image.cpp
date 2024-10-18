#include "svg_image.h"
#include "pugixml.hpp"

#include <regex>
#include <sstream>

namespace img_lib
{
    namespace svg_image
    {
        namespace
        {
            Color ParseColor(const std::string& color_str_)
            {
                if (color_str_.empty() || color_str_ == "none")
                {
                    return Color::Black();
                }

                if (color_str_[0] == '#') // hexadecimal colors
                {
                    if (color_str_.size() == 7) // #RRGGBB (24 bit)
                    {
                        uint8_t r = std::stoi(color_str_.substr(1, 2), nullptr, 16);
                        uint8_t g = std::stoi(color_str_.substr(3, 2), nullptr, 16);
                        uint8_t b = std::stoi(color_str_.substr(5, 2), nullptr, 16);
                        return Color(r, g, b);
                    }
                    else if (color_str_.size() == 4) // #RGB (16 bit)
                    {
                        uint8_t r = std::stoi(color_str_.substr(1, 1), nullptr, 16) * 17;
                        uint8_t g = std::stoi(color_str_.substr(2, 1), nullptr, 16) * 17;
                        uint8_t b = std::stoi(color_str_.substr(3, 1), nullptr, 16) * 17;
                        return Color(r, g, b);
                    }
                    else if (color_str_.size() == 9) // #RRGGBBAA (32 bit)
                    {
                        uint8_t r = std::stoi(color_str_.substr(1, 2), nullptr, 16);
                        uint8_t g = std::stoi(color_str_.substr(3, 2), nullptr, 16);
                        uint8_t b = std::stoi(color_str_.substr(5, 2), nullptr, 16);
                        uint8_t a = std::stoi(color_str_.substr(7, 2), nullptr, 16);
                        return Color(r, g, b, a);
                    }
                }

                std::smatch match; // functional notations
                static const std::regex rgb_regex(R"(rgb\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\))");
                static const std::regex rgba_regex(R"(rgba\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+(\.\d+)?)\s*\))");

                if (std::regex_match(color_str_, match, rgb_regex))
                {
                    uint8_t r = std::stoi(match[1].str());
                    uint8_t g = std::stoi(match[2].str());
                    uint8_t b = std::stoi(match[3].str());
                    return Color(r, g, b);
                }
                else if (std::regex_match(color_str_, match, rgba_regex))
                {
                    uint8_t r = std::stoi(match[1].str());
                    uint8_t g = std::stoi(match[2].str());
                    uint8_t b = std::stoi(match[3].str());
                    float a = std::stof(match[4].str());
                    return Color(r, g, b, static_cast<uint8_t>(a * 255));
                }

                return Color::Black();
            }

            void DrawCirclePixels(Image& image_, int cx_, int cy_, int x_, int y_, const Color& color_)
            {
                image_.SetPixel(cx_ + x_, cy_ + y_, color_);
                image_.SetPixel(cx_ - x_, cy_ + y_, color_);
                image_.SetPixel(cx_ + x_, cy_ - y_, color_);
                image_.SetPixel(cx_ - x_, cy_ - y_, color_);
                image_.SetPixel(cx_ + y_, cy_ + x_, color_);
                image_.SetPixel(cx_ - y_, cy_ + x_, color_);
                image_.SetPixel(cx_ + y_, cy_ - x_, color_);
                image_.SetPixel(cx_ - y_, cy_ - x_, color_);
            }

            void FillPolygon(Image& image_, const std::vector<std::pair<int, int>>& vertices_, const Color& fill_color_)
            {
                if (vertices_.empty())
                {
                    return;
                }

                int min_x = vertices_[0].first;
                int max_x = vertices_[0].first;
                int min_y = vertices_[0].second;
                int max_y = vertices_[0].second;

                for (const auto& vertex : vertices_)
                {
                    min_x = std::min(min_x, vertex.first);
                    max_x = std::max(max_x, vertex.first);
                    min_y = std::min(min_y, vertex.second);
                    max_y = std::max(max_y, vertex.second);
                }

                for (int y = min_y; y <= max_y; ++y)
                {
                    std::vector<int> intersections;

                    for (size_t i = 0; i < vertices_.size(); ++i)
                    {
                        size_t j = (i + 1) % vertices_.size();
                        int x1 = vertices_[i].first;
                        int y1 = vertices_[i].second;
                        int x2 = vertices_[j].first;
                        int y2 = vertices_[j].second;

                        if ((y1 <= y && y < y2) || (y2 <= y && y < y1))
                        {
                            int x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
                            intersections.push_back(x);
                        }
                    }

                    std::sort(intersections.begin(), intersections.end());

                    for (size_t i = 0; i < intersections.size(); i += 2)
                    {
                        int x_start = intersections[i];
                        int x_end = intersections[i + 1];

                        for (int x = x_start; x <= x_end; ++x)
                        {
                            image_.SetPixel(x, y, fill_color_);
                        }
                    }
                }
            }

            void DrawEllipsePixels(Image& image_, int cx_, int cy_, int x_, int y_, const Color& color_)
            {
                image_.SetPixel(cx_ + x_, cy_ + y_, color_);
                image_.SetPixel(cx_ - x_, cy_ + y_, color_);
                image_.SetPixel(cx_ + x_, cy_ - y_, color_);
                image_.SetPixel(cx_ - x_, cy_ - y_, color_);
            }

            void DrawLine(Image& image_, int x1_, int y1_, int x2_, int y2_, const Color& color_)
            {
                int dx = abs(x2_ - x1_);
                int dy = abs(y2_ - y1_);
                int sx = (x1_ < x2_) ? 1 : -1;
                int sy = (y1_ < y2_) ? 1 : -1;
                int err = dx - dy;

                while (true)
                {
                    if (x1_ >= 0 && x1_ < image_.GetWidth() && y1_ >= 0 && y1_ < image_.GetHeight())
                    {
                        image_.SetPixel(x1_, y1_, color_);
                    }

                    if (x1_ == x2_ && y1_ == y2_)
                    {
                        break;
                    }

                    int e2 = 2 * err;

                    if (e2 > -dy)
                    {
                        err -= dy;
                        x1_ += sx;
                    }

                    if (e2 < dx)
                    {
                        err += dx;
                        y1_ += sy;
                    }
                }
            }

            void DrawPolyline(Image& image_, const std::vector<std::pair<int, int>>& vertices_, const Color& color_)
            {
                for (size_t i = 0; i < vertices_.size() - 1; ++i)
                {
                    DrawLine(image_, vertices_[i].first, vertices_[i].second, vertices_[i + 1].first, vertices_[i + 1].second, color_);
                }
            }
        } // end namespace 

        void XmlRectLoad(pugi::xml_node child_, Image& image_)
        {
            int x = child_.attribute("x").as_int();
            int y = child_.attribute("y").as_int();
            int rect_width = child_.attribute("width").as_int();
            int rect_height = child_.attribute("height").as_int();
            std::string fill_str = child_.attribute("fill").as_string();

            Color fill_color = ParseColor(fill_str);

            for (int i = x; i < x + rect_width; ++i)
            {
                for (int j = y; j < y + rect_height; ++j)
                {
                    image_.SetPixel(i, j, fill_color);
                }
            }
        }

        void XmlCircleLoad(pugi::xml_node child_, Image& image_)
        {
            int cx = child_.attribute("cx").as_int();
            int cy = child_.attribute("cy").as_int();
            int radius = child_.attribute("r").as_int();
            std::string fill_str = child_.attribute("fill").as_string();

            Color fill_color = ParseColor(fill_str);

            int x = 0;
            int y = radius;
            int d = 3 - 2 * radius;

            DrawCirclePixels(image_, cx, cy, x, y, fill_color);

            while (y >= x)
            {
                x++;

                if (d > 0)
                {
                    y--;
                    d = d + 4 * (x - y) + 10;
                }
                else
                {
                    d = d + 4 * x + 6;
                }

                DrawCirclePixels(image_, cx, cy, x, y, fill_color);
            }

            for (int i = -radius; i <= radius; ++i)
            {
                for (int j = -radius; j <= radius; ++j)
                {
                    if (i * i + j * j <= radius * radius)
                    {
                        image_.SetPixel(cx + i, cy + j, fill_color);
                    }
                }
            }
        }

        void XmlPolygonLoad(pugi::xml_node child_, Image& image_)
        {
            std::string points_str = child_.attribute("points").as_string();
            std::string fill_str = child_.attribute("fill").as_string();

            Color fill_color = ParseColor(fill_str);

            std::vector<std::pair<int, int>> vertices;
            std::stringstream ss(points_str);
            std::string point;

            while (std::getline(ss, point, ' '))
            {
                std::stringstream point_ss(point);
                std::string x_str;
                std::string y_str;

                std::getline(point_ss, x_str, ',');
                std::getline(point_ss, y_str, ',');

                int x = std::stoi(x_str);
                int y = std::stoi(y_str);

                vertices.push_back({ x, y });
            }

            FillPolygon(image_, vertices, fill_color);
        }

        void XmlEllipseLoad(pugi::xml_node child_, Image& image_)
        {
            int cx = child_.attribute("cx").as_int();
            int cy = child_.attribute("cy").as_int();
            int rx = child_.attribute("rx").as_int();
            int ry = child_.attribute("ry").as_int();
            std::string fill_str = child_.attribute("fill").as_string();

            Color fill_color = ParseColor(fill_str);

            int x = 0;
            int y = ry;
            int d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);
            int dx = 2 * ry * ry * x;
            int dy = 2 * rx * rx * y;

            DrawEllipsePixels(image_, cx, cy, x, y, fill_color);

            while (dx < dy)
            {
                x++;
                dx += 2 * ry * ry;

                if (d1 < 0)
                {
                    d1 += dx + ry * ry;
                }
                else
                {
                    y--;
                    dy -= 2 * rx * rx;
                    d1 += dx - dy + ry * ry;
                }

                DrawEllipsePixels(image_, cx, cy, x, y, fill_color);
            }

            int d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) + ((rx * rx) * ((y - 1) * (y - 1))) - (rx * rx * ry * ry);

            while (y >= 0)
            {
                y--;
                dy -= 2 * rx * rx;

                if (d2 > 0)
                {
                    d2 += rx * rx - dy;
                }
                else
                {
                    x++;
                    dx += 2 * ry * ry;
                    d2 += dx - dy + rx * rx;
                }

                DrawEllipsePixels(image_, cx, cy, x, y, fill_color);
            }

            for (int i = -rx; i <= rx; ++i)
            {
                for (int j = -ry; j <= ry; ++j)
                {
                    if ((i * i * ry * ry + j * j * rx * rx) <= rx * rx * ry * ry)
                    {
                        image_.SetPixel(cx + i, cy + j, fill_color);
                    }
                }
            }
        }

        void XmlPolylineLoad(pugi::xml_node child_, Image& image_)
        {
            std::string points_str = child_.attribute("points").as_string();
            std::string stroke_str = child_.attribute("stroke").as_string();

            Color stroke_color = ParseColor(stroke_str);

            std::vector<std::pair<int, int>> vertices;
            std::stringstream ss(points_str);
            std::string point;

            while (std::getline(ss, point, ' '))
            {
                std::stringstream point_ss(point);
                std::string x_str;
                std::string y_str;

                std::getline(point_ss, x_str, ',');
                std::getline(point_ss, y_str, ',');

                int x = std::stoi(x_str);
                int y = std::stoi(y_str);

                vertices.push_back({ x, y });
            }

            DrawPolyline(image_, vertices, stroke_color);
        }

        void XmlLineLoad(pugi::xml_node child_, Image& image_)
        {
            int x1 = child_.attribute("x1").as_int();
            int y1 = child_.attribute("y1").as_int();
            int x2 = child_.attribute("x2").as_int();
            int y2 = child_.attribute("y2").as_int();
            std::string stroke_str = child_.attribute("stroke").as_string();

            Color stroke_color = ParseColor(stroke_str);

            DrawLine(image_, x1, y1, x2, y2, stroke_color);
        }

        void XmlPathLoad(pugi::xml_node child_, Image& image_)
        {
            
        }

        void XmlGLoad(pugi::xml_node child_, Image& image_)
        {

        }

        const Image SvgImage::LoadImageSVG(const Path& path_)
        {
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_file(path_.c_str());

            if (!result) 
            {
                throw std::runtime_error("File upload error: "s + std::string(result.description()));
            }

            pugi::xml_node svg_node = doc.child("svg");
            if (!svg_node) 
            {
                throw std::runtime_error("The file does not contain a root element <svg>"s);
            }

            int width = svg_node.attribute("width").as_int();
            int height = svg_node.attribute("height").as_int();

            Image image(width, height, Color::White());

            for (pugi::xml_node child : svg_node.children())
            {
                if (std::string(child.name()) == "rect"s)
                {
                    XmlRectLoad(child, image);
                }

                else if (std::string(child.name()) == "circle"s)
                {
                    XmlCircleLoad(child, image);
                }

                else if (std::string(child.name()) == "polygon"s)
                {
                    XmlPolygonLoad(child, image);
                }

                else if (std::string(child.name()) == "ellipse"s)
                {
                    XmlEllipseLoad(child, image);
                }

                else if (std::string(child.name()) == "polyline"s)
                {
                    XmlPolylineLoad(child, image);
                }

                else if (std::string(child.name()) == "line"s)
                {
                    XmlLineLoad(child, image);
                }

                else if (std::string(child.name()) == "path"s)
                {
                    XmlPathLoad(child, image);
                }

                else if (std::string(child.name()) == "g"s)
                {
                    XmlGLoad(child, image);
                }
            }

            return image;
        }

        bool SvgImage::SaveImageSVG(const Image& image_, const Path& path_) const
        {
            return false;
        }

    } // end namespace svg_image

} // end namespace img_lib