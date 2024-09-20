#pragma once

#include "image.h"


namespace img_lib 
{
    namespace tiff_image
    {
        class TiffImage
        {
        public:

            Image LoadImageTiff(const Path& path_);
            bool SaveImageTiff(const Path& path_, const Image& image_);

        private:

            struct TiffHeader
            {
                uint16_t byteOrder;     // Byte order (0x4949 for little-endian, 0x4D4D for big-endian)
                uint16_t magic;         // TIFF file identifier (value is 42)
                uint32_t ifdOffset;     // Offset to the first Image File Directory (IFD)
            };

            struct IFDEntry
            {
                uint16_t tag;             // Tag identifier for the entry type
                uint16_t type;            // Data type of the values (e.g., 1 for byte, 2 for string, 3 for short, 4 for long, 5 for rational)
                uint32_t count;           // Number of values in the entry
                uint32_t valueOffset;     // Offset to the value(s) if data does not fit within the entry
            };
        };

    } // end namespace tiff_image

} // end namespace img_lib