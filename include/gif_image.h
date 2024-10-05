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

		private:

			const Image LoadStatic(const Path& path_);
			bool SaveStatic(const Path& path_, const Image& image_) const;

			const Image LoadDinamic(const Path& path_);
			bool SaveDinamic(const Path& path_, const Image& image_) const;
		};
	}
}