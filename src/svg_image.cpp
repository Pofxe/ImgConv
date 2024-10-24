#include "svg_image.h"
#include "pugixml.hpp"
#include "color_map.h"

#include <regex>
#include <sstream>
#include <stack>

namespace img_lib
{
    namespace svg_image
    {
        namespace
        {
            struct Point
            {
                float x = 0.0;
                float y = 0.0;
            };

            struct GSettings
            {
                std::string fill;
                std::string stroke;

                bool use_g_settings = false;
            };


            Color ParseColor(const std::string& color_str_)
            {
                if (color_str_.empty() || color_str_ == "none")
                {
                    return Color::Transparent();
                }

                auto it = ColorMapLookup.find(color_str_);
                if (it != ColorMapLookup.end())
                {
                    return it->second;
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

            void DrawEllipsePixels(Image& image_, int cx_, int cy_, int x_, int y_, const Color& color_)
            {
                image_.SetPixel(cx_ + x_, cy_ + y_, color_);
                image_.SetPixel(cx_ - x_, cy_ + y_, color_);
                image_.SetPixel(cx_ + x_, cy_ - y_, color_);
                image_.SetPixel(cx_ - x_, cy_ - y_, color_);
            }

            void DrawLine(Image& image_, int x1_, int y1_, int x2_, int y2_, const Color& color_)
            {
                if (x1_ > x2_)
                {
                    std::swap(x1_, x2_);
                    std::swap(y1_, y2_);
                }

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

            void FillShape(Image& image_, const std::vector<std::pair<int, int>>& points_, const Color& fill_color_)
            {
                if (points_.empty())
                {
                    return;
                }

                int min_x = points_[0].first;
                int max_x = points_[0].first;
                int min_y = points_[0].second;
                int max_y = points_[0].second;

                for (const auto& point : points_)
                {
                    min_x = std::min(min_x, point.first);
                    max_x = std::max(max_x, point.first);
                    min_y = std::min(min_y, point.second);
                    max_y = std::max(max_y, point.second);
                }

                for (int y = min_y; y <= max_y; ++y)
                {
                    std::vector<int> intersections;

                    for (size_t i = 0; i < points_.size(); ++i)
                    {
                        const auto& p1 = points_[i];
                        const auto& p2 = points_[(i + 1) % points_.size()];

                        if ((p1.second < y && p2.second >= y) || (p2.second < y && p1.second >= y))
                        {
                            int x = p1.first + (y - p1.second) * (p2.first - p1.first) / (p2.second - p1.second);
                            intersections.push_back(x);
                        }
                    }

                    std::sort(intersections.begin(), intersections.end());

                    for (size_t i = 0; i < intersections.size(); i += 2)
                    {
                        int x1 = intersections[i];
                        int x2 = intersections[i + 1];

                        for (int x = x1 + 1; x < x2; ++x)
                        {
                            if (x >= 0 && x < image_.GetWidth() && y >= 0 && y < image_.GetHeight())
                            {
                                image_.SetPixel(x, y, fill_color_);
                            }
                        }
                    }
                }
            }

            void DrawBezierQuadratic(Image& image_, const std::vector<Point>& control_points_, const Color& color_)
            {
                if (control_points_.size() != 3)
                {
                    return;
                }

                Point p1 = control_points_[0];
                Point p2 = control_points_[1];
                Point p3 = control_points_[2];

                for (float t = 0; t <= 1; t += 0.005)
                {
                    float x = (1 - t) * (1 - t) * p1.x + 2 * (1 - t) * t * p2.x + t * t * p3.x;
                    float y = (1 - t) * (1 - t) * p1.y + 2 * (1 - t) * t * p2.y + t * t * p3.y;

                    image_.SetPixel(x, y, color_);
                }
            }

            void DrawBezierCurve(Image& image_, const std::vector<Point>& control_points_, const Color& color_)
            {
                if (control_points_.size() != 4)
                {
                    return;
                }

                Point p1 = control_points_[0];
                Point p2 = control_points_[1];
                Point p3 = control_points_[2];
                Point p4 = control_points_[3];

                for (float t = 0; t <= 1; t += 0.005)
                {
                    float x = (1 - t) * (1 - t) * (1 - t) * p1.x + 3 * (1 - t) * (1 - t) * t * p2.x + 3 * (1 - t) * t * t * p3.x + t * t * t * p4.x;
                    float y = (1 - t) * (1 - t) * (1 - t) * p1.y + 3 * (1 - t) * (1 - t) * t * p2.y + 3 * (1 - t) * t * t * p3.y + t * t * t * p4.y;

                    image_.SetPixel(x, y, color_);
                }
            }

        } // end namespace 

        void XmlRectLoad(pugi::xml_node child_, Image& image_, const GSettings& settings_ = GSettings())
        {
            int x = child_.attribute("x").as_int();
            int y = child_.attribute("y").as_int();
            int rect_width = child_.attribute("width").as_int();
            int rect_height = child_.attribute("height").as_int();

            Color fill_color;
            Color stroke_color;

            if (settings_.use_g_settings)
            {
                fill_color = ParseColor(settings_.fill.c_str());
                stroke_color = ParseColor(settings_.stroke.c_str());
            }
            else
            {
                std::string fill_str = child_.attribute("fill").as_string();
                std::string stroke_str = child_.attribute("stroke").as_string();
                fill_color = ParseColor(fill_str);

                if (stroke_str.empty())
                {
                    stroke_color = fill_color;
                }
                else
                {
                    stroke_color = ParseColor(stroke_str);
                }
            }

            for (int i = x; i < x + rect_width; ++i)
            {
                for (int j = y; j < y + rect_height; ++j)
                {
                    image_.SetPixel(i, j, fill_color);
                }
            }

            for (int i = x; i < x + rect_width; ++i)
            {
                image_.SetPixel(i, y, stroke_color);
                image_.SetPixel(i, y + rect_height - 1, stroke_color);
            }
            for (int j = y; j < y + rect_height; ++j)
            {
                image_.SetPixel(x, j, stroke_color);
                image_.SetPixel(x + rect_width - 1, j, stroke_color);
            }
        }

        void XmlCircleLoad(pugi::xml_node child_, Image& image_, const GSettings& settings_ = GSettings())
        {
            int cx = child_.attribute("cx").as_int();
            int cy = child_.attribute("cy").as_int();
            int radius = child_.attribute("r").as_int();

            Color fill_color;
            Color stroke_color;

            if (settings_.use_g_settings)
            {
                fill_color = ParseColor(settings_.fill.c_str());
                stroke_color = ParseColor(settings_.stroke.c_str());
            }
            else
            {
                std::string fill_str = child_.attribute("fill").as_string();
                std::string stroke_str = child_.attribute("stroke").as_string();
                fill_color = ParseColor(fill_str);

                if (stroke_str.empty())
                {
                    stroke_color = fill_color;
                }
                else
                {
                    stroke_color = ParseColor(stroke_str);
                }
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

            int x = 0;
            int y = radius;
            int d = 3 - 2 * radius;

            DrawCirclePixels(image_, cx, cy, x, y, stroke_color);

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

                DrawCirclePixels(image_, cx, cy, x, y, stroke_color);
            }
        }

        void XmlPolygonLoad(pugi::xml_node child_, Image& image_, const GSettings& settings_ = GSettings())
        {
            std::string points_str = child_.attribute("points").as_string();

            Color fill_color;
            Color stroke_color;

            if (settings_.use_g_settings)
            {
                fill_color = ParseColor(settings_.fill.c_str());
                stroke_color = ParseColor(settings_.stroke.c_str());
            }
            else
            {
                std::string fill_str = child_.attribute("fill").as_string();
                std::string stroke_str = child_.attribute("stroke").as_string();
                fill_color = ParseColor(fill_str);

                if (stroke_str.empty())
                {
                    stroke_color = fill_color;
                }
                else
                {
                    stroke_color = ParseColor(stroke_str);
                }
            }

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

            FillShape(image_, vertices, fill_color);

            for (size_t i = 1; i < vertices.size(); ++i)
            {
                DrawLine(image_, vertices[i - 1].first, vertices[i - 1].second, vertices[i].first, vertices[i].second, stroke_color);
            }
            DrawLine(image_, vertices.back().first, vertices.back().second, vertices.front().first, vertices.front().second, stroke_color);
        }

        void XmlEllipseLoad(pugi::xml_node child_, Image& image_, const GSettings& settings_ = GSettings())
        {
            int cx = child_.attribute("cx").as_int();
            int cy = child_.attribute("cy").as_int();
            int rx = child_.attribute("rx").as_int();
            int ry = child_.attribute("ry").as_int();

            Color fill_color;
            Color stroke_color;

            if (settings_.use_g_settings)
            {
                fill_color = ParseColor(settings_.fill.c_str());
                stroke_color = ParseColor(settings_.stroke.c_str());
            }
            else
            {
                std::string fill_str = child_.attribute("fill").as_string();
                std::string stroke_str = child_.attribute("stroke").as_string();
                fill_color = ParseColor(fill_str);

                if (stroke_str.empty())
                {
                    stroke_color = fill_color;
                }
                else
                {
                    stroke_color = ParseColor(stroke_str);
                }
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

            int x = 0;
            int y = ry;
            int d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);
            int dx = 2 * ry * ry * x;
            int dy = 2 * rx * rx * y;

            DrawEllipsePixels(image_, cx, cy, x, y, stroke_color);

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

                DrawEllipsePixels(image_, cx, cy, x, y, stroke_color);
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

                DrawEllipsePixels(image_, cx, cy, x, y, stroke_color);
            }
        }

        void XmlPolylineLoad(pugi::xml_node child_, Image& image_, const GSettings& settings_ = GSettings())
        {
            std::string points_str = child_.attribute("points").as_string();

            Color stroke_color;

            if (settings_.use_g_settings)
            {
                stroke_color = ParseColor(settings_.stroke.c_str());
            }
            else
            {
                std::string stroke_str = child_.attribute("stroke").as_string();
                stroke_color = ParseColor(stroke_str);
            }

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

        void XmlLineLoad(pugi::xml_node child_, Image& image_, const GSettings& settings_ = GSettings())
        {
            int x1 = child_.attribute("x1").as_int();
            int y1 = child_.attribute("y1").as_int();
            int x2 = child_.attribute("x2").as_int();
            int y2 = child_.attribute("y2").as_int();

            Color stroke_color;

            if (settings_.use_g_settings)
            {
                stroke_color = ParseColor(settings_.stroke.c_str());
            }
            else
            {
                std::string stroke_str = child_.attribute("stroke").as_string();
                stroke_color = ParseColor(stroke_str);
            }

            DrawLine(image_, x1, y1, x2, y2, stroke_color);
        }

        void XmlPathLoad(pugi::xml_node child_, Image& image_, const GSettings& settings_ = GSettings())
        {
            std::string d_str = child_.attribute("d").as_string();

            std::string fill_str = "";
            Color stroke_color;
            Color fill_color;

            if (settings_.use_g_settings)
            {
                stroke_color = ParseColor(settings_.stroke.c_str());
                fill_color = ParseColor(settings_.fill.c_str());
            }
            else
            {
                std::string stroke_str = child_.attribute("stroke").as_string();
                fill_str = child_.attribute("fill").as_string();
                fill_color = ParseColor(fill_str);

                if (stroke_str.empty())
                {
                    stroke_color = fill_color;
                }
                else
                {
                    stroke_color = ParseColor(stroke_str);
                }
            }

            std::stringstream ss(d_str);
            std::string command;
            std::vector<Point> points;

            char current_command = '\0';

            float x = 0;
            float y = 0;

            while (ss >> command)
            {
                if (isalpha(command[0]))
                {
                    current_command = command[0];
                    continue;
                }

                std::stringstream command_ss(command);
                std::string x_str, y_str;

                if (current_command == 'M' || current_command == 'L')
                {
                    std::getline(command_ss, x_str, ',');
                    std::getline(command_ss, y_str, ',');
                    x = std::stof(x_str);
                    y = std::stof(y_str);
                    points.push_back({ x, y });

                    if (current_command == 'M')
                    {
                        current_command = 'L';
                    }
                }
                else if (current_command == 'Z')
                {
                    if (!points.empty())
                    {
                        DrawLine(image_, points.back().x, points.back().y, points.front().x, points.front().y, stroke_color);
                    }
                }
                else if (current_command == 'Q')
                {
                    if (points.empty())
                    {
                        continue;
                    }

                    std::vector<Point> control_points;
                    for (int i = 0; i < 2; ++i)
                    {
                        std::getline(command_ss, x_str, ',');
                        std::getline(command_ss, y_str, ',');
                        float cx = std::stof(x_str);
                        float cy = std::stof(y_str);
                        control_points.push_back({ cx, cy });
                    }
                    DrawBezierQuadratic(image_, { points.back(), control_points[0], control_points[1] }, stroke_color);
                    points.push_back(control_points[1]);
                }
                else if (current_command == 'C')
                {
                    if (points.empty())
                    {
                        continue;
                    }

                    std::vector<Point> control_points;
                    for (int i = 0; i < 3; ++i)
                    {
                        std::getline(command_ss, x_str, ',');
                        std::getline(command_ss, y_str, ',');
                        float cx = std::stof(x_str);
                        float cy = std::stof(y_str);
                        control_points.push_back({ cx, cy });
                    }
                    DrawBezierCurve(image_, { points.back(), control_points[0], control_points[1], control_points[2] }, stroke_color);
                    points.push_back(control_points[2]);
                }
            }

            for (size_t i = 1; i < points.size(); ++i)
            {
                DrawLine(image_, points[i - 1].x, points[i - 1].y, points[i].x, points[i].y, stroke_color);
            }

            if (!fill_str.empty() && fill_str != "none"s)
            {
                std::vector<std::pair<int, int>> int_points;
                for (const auto& point : points)
                {
                    int_points.push_back({ static_cast<int>(point.x), static_cast<int>(point.y) });
                }
                FillShape(image_, int_points, fill_color);
            }
        }

        void XmlGLoad(pugi::xml_node child_, Image& image_)
        {
            GSettings settings;

            settings.fill = child_.attribute("fill").as_string();
            settings.stroke = child_.attribute("stroke").as_string();
            settings.use_g_settings = true;

            for (pugi::xml_node node : child_.children())
            {
                std::string node_name = node.name();

                if (node_name == "path")
                {
                    XmlPathLoad(node, image_, settings);
                }
                else if (node_name == "rect")
                {
                    XmlRectLoad(node, image_, settings);
                }
                else if (node_name == "circle")
                {
                    XmlCircleLoad(node, image_, settings);
                }
                else if (node_name == "ellipse")
                {
                    XmlEllipseLoad(node, image_, settings);
                }
                else if (node_name == "line")
                {
                    XmlLineLoad(node, image_, settings);
                }
                else if (node_name == "polyline")
                {
                    XmlPolylineLoad(node, image_, settings);
                }
                else if (node_name == "polygon")
                {
                    XmlPolygonLoad(node, image_, settings);
                }
            }
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