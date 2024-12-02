#include "png_image.h"

#include <setjmp.h>

namespace img_lib
{
    namespace png_image
    {
        const Image PngImage::LoadImagePNG(const Path& path_)
        {
            return {};
        }
        
        bool PngImage::SaveImagePNG(const Path& path_, const Image& image_) const
        {
            return false;
        }

    } // end namespace png_image

} // end namespace img_lib