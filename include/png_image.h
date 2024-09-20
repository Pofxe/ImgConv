#pragma once

#include "image.h"
#include <png.h>

namespace img_lib
{
	namespace png_image
	{
		class PngImage
		{
		public:

			Image LoadImagePng(const Path& path_);
			bool SaveImagePng(const Path& path_, const Image& image_);
		};

	} // end namespace png_image

} // end namespace img_lib