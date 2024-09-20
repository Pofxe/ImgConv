#pragma once
#include "image.h"
#include "pack_defines.h"

namespace img_lib
{
	namespace bmp_image
	{
		class BmpImage
		{
		public:

			Image LoadImageBMP(const Path& file_);
			bool SaveImageBMP(const Path& file_, const Image& image_);

		private:

            PACKED_STRUCT_BEGIN BitmapFileHeader
            {
                uint16_t file_type;      // Signature "BM"
                uint32_t file_size;      // File size in bytes
                uint16_t reserved1;      // Reserved, must be 0
                uint16_t reserved2;      // Reserved, must be 0
                uint32_t offset_data;    // Offset to the start of image data
            }
            PACKED_STRUCT_END

            PACKED_STRUCT_BEGIN BitmapInfoHeader
            {
                uint32_t size;              // Size of the header in bytes (40)
                int32_t width;              // Image width in pixels
                int32_t height;             // Image height in pixels
                uint16_t planes;            // Number of planes (1)
                uint16_t bit_count;         // Number of bits per pixel (24)
                uint32_t compression;       // Compression type (0 - no compression)
                uint32_t image_size;        // Size of image data in bytes
                int32_t x_pixels_per_meter; // Horizontal resolution (pixels per meter)
                int32_t y_pixels_per_meter; // Vertical resolution (pixels per meter)
                uint32_t colors_used;       // Number of colors used (0 - all)
                uint32_t colors_important;  // Number of important colors (0 - all)
            }
            PACKED_STRUCT_END
		};

	} // end namespace bmp_image

} // end namespace img_lib