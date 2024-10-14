#include "svg_image.h"

namespace img_lib
{
	namespace svg_image
	{
		namespace
		{
			class SVG
			{
			public:

				SVG(int width_, int height_)
				{
					std::ostringstream oss;
					oss << "<svg width = " << width_ << " height = " << height_ << " viewBox = \"0 0 100 100\" xmlns = \"http://www.w3.org/2000/svg\">";
					element = oss.str();
				}

				void AddElement(const std::string& element_)
				{
					element += element_;
				}

				std::string GetContent() const
				{
					return element + "</svg>";
				}

			private:

				std::string element;
			};

			class Rect
			{
			public:

				Rect(int x_, int y_, int width_, int height_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{
					std::ostringstream oss;
					oss << "<rect x=\"" << x_ << "\" y=\"" << y_ << "\" width=\"" << width_ << "\" height=\"" << height_ << "\" fill=\"" << fill_ << "\" stroke=\"" << stroke_ << "\" stroke-width=\"" << strokeWidth_ << "\"/>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class Circle
			{
			public:

				Circle(int cx_, int cy_, int r_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{
					std::ostringstream oss;
					oss << "<circle cx=\"" << cx_ << "\" cy=\"" << cy_ << "\" r=\"" << r_ << "\" fill=\"" << fill_ << "\" stroke=\"" << stroke_ << "\" stroke-width=\"" << strokeWidth_ << "\"/>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class Ellipse
			{
			public:

				Ellipse(int cx_, int cy_, int rx_, int ry_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{
					std::ostringstream oss;
					oss << "<ellipse cx=\"" << cx_ << "\" cy=\"" << cy_ << "\" rx=\"" << rx_ << "\" ry=\"" << ry_ << "\" fill=\"" << fill_ << "\" stroke=\"" << stroke_ << "\" stroke-width=\"" << strokeWidth_ << "\"/>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class Line
			{
			public:

				Line(int x1_, int y1_, int x2_, int y2_, const std::string& stroke_, int strokeWidth_)
				{
					std::ostringstream oss;
					oss << "<line x1=\"" << x1_ << "\" y1=\"" << y1_ << "\" x2=\"" << x2_ << "\" y2=\"" << y2_ << "\" stroke=\"" << stroke_ << "\" stroke-width=\"" << strokeWidth_ << "\"/>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class Polyline
			{
			public:

				Polyline(const std::string& points_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{
					std::ostringstream oss;
					oss << "<polyline points=\"" << points_ << "\" fill=\"" << fill_ << "\" stroke=\"" << stroke_ << "\" stroke-width=\"" << strokeWidth_ << "\"/>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class Polygon
			{
			public:

				Polygon(const std::string& points_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{
					std::ostringstream oss;
					oss << "<polygon points=\"" << points_ << "\" fill=\"" << fill_ << "\" stroke=\"" << stroke_ << "\" stroke-width=\"" << strokeWidth_ << "\"/>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class PathSvg
			{
			public:

				PathSvg(const std::string& d_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{
					std::ostringstream oss;
					oss << "<path d=\"" << d_ << "\" fill=\"" << fill_ << "\" stroke=\"" << stroke_ << "\" stroke-width=\"" << strokeWidth_ << "\"/>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class Text
			{
			public:

				Text(int x_, int y_, const std::string& text_, const std::string& fontFamily_, int fontSize_, const std::string& fill_)
				{
					std::ostringstream oss;
					oss << "<text x=\"" << x_ << "\" y=\"" << y_ << "\" font-family=\"" << fontFamily_ << "\" font-size=\"" << fontSize_ << "\" fill=\"" << fill_ << "\">" << text_ << "</text>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class G
			{
			public:

				G(const std::string& transform_)
				{
					std::ostringstream oss;
					oss << "<g transform=\"" << transform_ << "\">";
					element = oss.str();
				}

				void AddElement(const std::string& childElement_)
				{
					element += childElement_;
				}

				std::string GetElement() const
				{
					return element + "</g>";
				}

			private:

				std::string element;
			};

			class Defs
			{
			public:

				Defs()
				{
					element = "<defs>";
				}

				void AddElement(const std::string& childElement)
				{
					element += childElement;
				}

				std::string GetElement() const
				{
					return element + "</defs>";
				}


			private:

				std::string element;
			};

			class Use
			{
			public:

				Use(const std::string& href_, int x_, int y_)
				{
					std::ostringstream oss;
					oss << "<use xlink:href=\"" << href_ << "\" x=\"" << x_ << "\" y=\"" << y_ << "\"/>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class Style
			{
			public:

				Style(const std::string& css_)
				{
					std::ostringstream oss;
					oss << "<style>" << css_ << "</style>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class Title
			{
			public:

				Title(const std::string& title_)
				{
					std::ostringstream oss;
					oss << "<title>" << title_ << "</title>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class Desc
			{
			public:

				Desc(const std::string& description_)
				{
					std::ostringstream oss;
					oss << "<desc>" << description_ << "</desc>";
					element = oss.str();
				}

				std::string GetElement() const
				{
					return element;
				}

			private:

				std::string element;
			};

			class GeneratorSVG
			{
			public:

				void AddRect(int x_, int y_, int width_, int height_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{

				}

				void AddCircle(int cx_, int cy_, int r_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{

				}

				void AddEllipse(int cx_, int cy_, int rx_, int ry_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{

				}

				void AddLine(int x1_, int y1_, int x2_, int y2_, const std::string& stroke_, int strokeWidth_)
				{

				}

				void AddPolyline(const std::string& points_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{

				}

				void AddPolygon(const std::string& points_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{

				}

				void AddPath(const std::string& d_, const std::string& fill_, const std::string& stroke_, int strokeWidth_)
				{

				}

				void AddText(int x_, int y_, const std::string& text_, const std::string& fontFamily_, int fontSize_, const std::string& fill_)
				{

				}

				void AddG(const std::string& transform_)
				{

				}

				void AddDefs()
				{

				}

				void AddUse(const std::string& href_, int x_, int y_)
				{

				}

				void AddStyle(const std::string& css_)
				{

				}

				void AddTitle(const std::string& title_)
				{

				}

				void AddDesc(const std::string& description_)
				{

				}

				std::string Generate() const
				{

				}
			};

		} // end namespace

		const Image SvgImage::LoadImageSVG(const Path& path_)
		{
			return Image();
		}

		bool SvgImage::SaveImageSVG(const Image& image_, const Path& path_) const
		{
			return false;
		}

} // end namespace svg_image

} // end namespace img_lib