#pragma once

#include "image.h"

namespace img_lib
{
	namespace ppm_image
	{
		static const int PPM_MAX = 255;
		static const std::string PPM_TYPE_P3 = "P3"s;
		static const std::string PPM_TYPE_P6 = "P6"s;

		class PpmImage
		{
		public:

			const Image LoadImagePPM(const Path& file_);
			bool SaveImagePPM(const Path& file_, const Image& image_) const;

		private:

			const Image LoadP3(const Path& file_);
			const Image LoadP6(const Path& file_);

			bool SaveP3(const Path& file_, const Image& image_) const;
			bool SaveP6(const Path& file_, const Image& image_) const;
		};

	} // end namespace ppm_image

} // end namespace img_lib