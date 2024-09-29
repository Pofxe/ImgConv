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

            const Image LoadImageJPEG(const Path& path_);
            bool SaveImageJPEG(const Path& path_, const Image& image_) const;
        };

    } // end namespace jpeg_image

} // end namespace img_lib