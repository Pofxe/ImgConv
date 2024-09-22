#pragma once

#include "image.h"

namespace img_lib
{
	namespace ico_image
	{
		class IcoImage
		{
		public:

			Image LoadImageICO(const Path& path_);
			bool SaveImageICO(const Path& path, const Image& image_);

		private:

            struct IcoHeader 
            {
                uint16_t reserved;   // Reserved field (must be 0)
                uint16_t type;       // File type (must be 1 for ICO)
                uint16_t count;      // Number of images
            };

            struct IcoDirEntry 
            {
                uint8_t width;       // Image width (0 means 256 pixels)
                uint8_t height;      // Image height (0 means 256 pixels)
                uint8_t colorCount;  // Number of colors in the palette (0 means no palette)
                uint8_t reserved;    // Reserved field (must be 0)
                uint16_t planes;     // Number of planes (must be 0 or 1)
                uint16_t bitCount;   // Number of bits per pixel
                uint32_t size;       // Size of image data in bytes
                uint32_t offset;     // Offset of image data from the start of the file
            };

		};

	} // end namespace ico_image

} // end namespace img_lib