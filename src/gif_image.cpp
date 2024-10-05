#include "gif_image.h"

namespace img_lib
{
	namespace gif_image
	{
		const Image GifImage::LoadImageGIF(const Path& path_)
		{
			return {};
		}

		bool GifImage::SaveImageGIF(const Path& path_, const Image& image_) const
		{
			return false;
		}

		const Image GifImage::LoadStatic(const Path& path_)
		{
			return {};
		}

		bool GifImage::SaveStatic(const Path& path_, const Image& image_) const
		{
			return false;
		}

		const Image GifImage::LoadDinamic(const Path& path_)
		{	
			return {};
		}	
		
		bool GifImage::SaveDinamic(const Path& path_, const Image& image_) const
		{
			return false;
		}

	} // end namespace gif_image
}// end namespace img_lib