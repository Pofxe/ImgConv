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

			Image LoadImageICO(const Path& path_);
			bool SaveImageICO(const Path& path, const Image& image_);

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
				uint16_t bfType;          // BMP signature: 0x4D42 (BM)
			    uint32_t bfSize;          // BMP file size
			    uint16_t bfReserved1;     // Reserved: 0
			    uint16_t bfReserved2;     // Reserved: 0
			    uint32_t bfOffBits;       // Offset of image data from the start of the file
			    uint32_t biSize;          // Size of the BMP header
			    uint32_t biWidth;         // Image width
			    uint32_t biHeight;        // Image height (negative for top-left origin)
			    uint16_t biPlanes;        // Always 1
			    uint16_t biBitCount;      // Color depth (32 bits)
			    uint32_t biCompression;   // Compression: 0 (no compression)
			    uint32_t biSizeImage;     // Size of image data
			    uint32_t biXPelsPerMeter; // Horizontal resolution (0)
			    uint32_t biYPelsPerMeter; // Vertical resolution (0)
			    uint32_t biClrUsed;       // Number of colors used (0)
			    uint32_t biClrImportant;  // Number of important colors (0)
			}PACKED_STRUCT_END

			Image Resize(int newWidth_, int newHeight_, const Image& image_);
        };

		

	} // end namespace ico_image

} // end namespace img_lib