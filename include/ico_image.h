#pragma once

#include "image.h"
#include "pack_defines.h"

namespace img_lib
{
	namespace ico_image
	{
		class IcoImage
		{
		public:

			const Image LoadImageICO(const Path& path_);
			bool SaveImageICO(const Path& path, const Image& image_) const;

		private:

			struct IcoHeader
			{
				uint16_t reserved;   // Reserved, always 0
				uint16_t type;       // Resource type, always 1 for ICO
				uint16_t count;      // Number of images in the file
			};

			struct IconDirEntry
			{
				uint8_t width;       // Image width in pixels (0 means 256 pixels)
				uint8_t height;      // Image height in pixels (0 means 256 pixels)
				uint8_t color_count; // Number of colors in the palette (0 if more than 256 colors)
				uint8_t reserved;    // Reserved, always 0
				uint16_t planes;     // Number of planes (always 1 for ICO)
				uint16_t bit_count;  // Number of bits per pixel
				uint32_t size;       // Size of image data in bytes
				uint32_t offset;     // Offset of image data from the start of the file
			};

			PACKED_STRUCT_BEGIN BmpHeader
			{
				uint32_t biSize;          // Size of the structure in bytes
				int32_t biWidth;          // Width of the image in pixels
				int32_t biHeight;         // Height of the image in pixels (doubled for the mask)
				uint16_t biPlanes;        // Planes (should be 1)
				uint16_t biBitCount;      // Bits per pixel (e.g., 32 for RGBA)
				uint32_t biCompression;   // Compression type (0 for BI_RGB, no compression)
				uint32_t biSizeImage;     // Size of the image in bytes
				int32_t biXPelsPerMeter;  // Horizontal resolution (pixels per meter)
				int32_t biYPelsPerMeter;  // Vertical resolution (pixels per meter)
				uint32_t biClrUsed;       // Number of colors used (0 for all)
				uint32_t biClrImportant;  // Number of important colors (0 for all)
			}PACKED_STRUCT_END
		};

	} // end namespace ico_image

} // end namespace img_lib