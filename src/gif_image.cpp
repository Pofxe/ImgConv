#include "gif_image.h"

namespace img_lib
{
	namespace gif_image
	{
		const Image GifImage::LoadImageGIF(const Path& path_)
		{
            GifFileType* gif_file = DGifOpenFileName(path_.string().c_str(), nullptr);
            if (!gif_file)
            {
                throw std::runtime_error("Failed to open GIF file: " + path_.string());
            }

            int error_code = DGifSlurp(gif_file);
            if (error_code != GIF_OK)
            {
                DGifCloseFile(gif_file, nullptr);
                throw std::runtime_error("Failed to read GIF file: " + path_.string());
            }

            if (gif_file->ImageCount != 1)
            {
                DGifCloseFile(gif_file, nullptr);
                throw std::runtime_error("GIF file is not static: " + path_.string());
            }

            SavedImage* frame = &gif_file->SavedImages[0];
            GifImageDesc& frame_desc = frame->ImageDesc;

            Image image(frame_desc.Width, frame_desc.Height);

            for (int y = 0; y < frame_desc.Height; ++y)
            {
                for (int x = 0; x < frame_desc.Width; ++x)
                {
                    int pixel_index = y * frame_desc.Width + x;
                    GifByteType* pixel = &frame->RasterBits[pixel_index];
                    Color color =
                    {
                        gif_file->SColorMap->Colors[*pixel].Red,
                        gif_file->SColorMap->Colors[*pixel].Green,
                        gif_file->SColorMap->Colors[*pixel].Blue,
                        255
                    };
                    image.SetPixel(x, y, color);
                }
            }

            DGifCloseFile(gif_file, nullptr);

            return image;
		}

		bool GifImage::SaveImageGIF(const Path& path_, const Image& image_) const
		{
            return false;
		}
	}
}