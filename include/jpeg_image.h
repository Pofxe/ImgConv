#pragma once

#include "image.h"

extern "C"
{
    #include <jpeglib.h>
}

namespace img_lib
{
    namespace jpeg_image
    {
        class JpegImage
        {
        public:

            Image LoadImageJpeg(const Path& path_);
            bool SaveImageJpeg(const Path& path_, const Image& image_);
        };

    } // end namespace jpeg_image

} // end namespace img_lib