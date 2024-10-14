#include "image.h"

#include <sstream>

namespace img_lib
{
	namespace svg_image
	{
		class SvgImage
		{
		public:

			const Image LoadImageSVG(const Path& path_);
			bool SaveImageSVG(const Image& image_, const Path& path_) const;
		};

	} // end namespace svg_image

} // end namespace img_lib