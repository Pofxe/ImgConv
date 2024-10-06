#pragma once

#include "image.h"

#include <gif_lib.h>

namespace img_lib
{
	namespace gif_image
	{
		class GifImage
		{
		public:

			const Image LoadImageGIF(const Path& path_);
			bool SaveImageGIF(const Path& path_, const Image& image_) const;
		};
	}
}