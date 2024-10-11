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
                throw std::runtime_error("Failed to open GIF file: "s + path_.string());
            }

            int error_code = DGifSlurp(gif_file);
            if (error_code != GIF_OK)
            {
                DGifCloseFile(gif_file, nullptr);
                throw std::runtime_error("Failed to read GIF file: "s + path_.string());
            }

            if (gif_file->ImageCount != 1)
            {
                DGifCloseFile(gif_file, nullptr);
                throw std::runtime_error("GIF file is not static: "s + path_.string());
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

        bool GifImage::SaveImageGIF(const Path& path_, const Image& image_) const // image is saved in grayscale
        {
            GifFileType* gif_file = EGifOpenFileName(path_.string().c_str(), false, nullptr);
            if (!gif_file)
            {
                throw std::runtime_error("Failed to create GIF file: "s + path_.string());
            }

            EGifSetGifVersion(gif_file, true);

            ColorMapObject* color_map = GifMakeMapObject(256, nullptr);
            if (!color_map)
            {
                EGifCloseFile(gif_file, nullptr);
                throw std::runtime_error("Failed to create color map for GIF file: "s + path_.string());
            }

            for (int i = 0; i < 256; ++i)
            {
                color_map->Colors[i].Red = i;
                color_map->Colors[i].Green = i;
                color_map->Colors[i].Blue = i;
            }

            gif_file->SColorMap = color_map;
            int width = image_.GetWidth();
            int height = image_.GetHeight();

            if (EGifPutScreenDesc(gif_file, width, height, 8, 0, color_map) == GIF_ERROR)
            {
                GifFreeMapObject(color_map);
                EGifCloseFile(gif_file, nullptr);
                throw std::runtime_error("Failed to set screen description for GIF file: "s + path_.string());
            }

            if (EGifPutImageDesc(gif_file, 0, 0, width, height, false, nullptr) == GIF_ERROR)
            {
                GifFreeMapObject(color_map);
                EGifCloseFile(gif_file, nullptr);
                throw std::runtime_error("Failed to set image description for GIF file: "s + path_.string());
            }

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    const Color& color = image_.GetPixel(x, y);
                    GifByteType pixel = (GifByteType)((color.r + color.g + color.b) / 3);
                    if (EGifPutPixel(gif_file, pixel) == GIF_ERROR)
                    {
                        GifFreeMapObject(color_map);
                        EGifCloseFile(gif_file, nullptr);
                        throw std::runtime_error("Failed to write pixel to GIF file: "s + path_.string());
                    }
                }
            }

            GifFreeMapObject(color_map);
            if (EGifCloseFile(gif_file, nullptr) == GIF_ERROR)
            {
                throw std::runtime_error("Failed to close GIF file: "s + path_.string());
            }

            return true;
        }
	}
}