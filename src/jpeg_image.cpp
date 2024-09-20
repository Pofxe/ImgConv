#include "jpeg_image.h"

#include <stdexcept>
#include <cstdio>
#include <setjmp.h>

namespace img_lib
{
    namespace jpeg_image
    {
        Image JpegImage::LoadImageJpeg(const Path& path_)
        {
            return Image();
        }

        bool JpegImage::SaveImageJpeg(const Path& path_, const Image& image_)
        {
            return false;
        }


    } // end namespace jpeg_image

} // end namespace img_lib