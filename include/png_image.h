#pragma once 

#include "image.h"

extern "C"
{
	#include <png.h>
}

namespace img_lib
{
	namespace png_image
	{
		class PngImage
		{
		public:

			Image LoadImagePNG(const Path& path_);
			bool SaveImagePNG(const Path& path_, const Image& image_);
		};

	} // end namespace png_image

} // end namespace img_lib
