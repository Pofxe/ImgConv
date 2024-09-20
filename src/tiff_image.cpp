#include "tiff_image.h"

namespace img_lib
{
    namespace tiff_image
    {
        Image TiffImage::LoadImageTiff(const Path& path_)
        {
            std::ifstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Failed to open TIFF file: "s + path_.string());
            }

            TiffHeader header;

            file.read(reinterpret_cast<char*>(&header), sizeof(TiffHeader));
            if (!file || header.byteOrder != 0x4949 || header.magic != 42)
            {
                throw std::runtime_error("Invalid TIFF file: "s + path_.string());
            }

            std::vector<IFDEntry> ifdEntries;
            file.seekg(header.ifdOffset, std::ios::beg);

            uint16_t entryCount;
            file.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount));
            if (!file)
            {
                return {};
            }

            ifdEntries.resize(entryCount);
            file.read(reinterpret_cast<char*>(ifdEntries.data()), entryCount * sizeof(IFDEntry));
            if (!file)
            {
                return {};
            }

            uint32_t width = 0;
            uint32_t height = 0;
            uint32_t bitsPerSample = 0;
            uint32_t samplesPerPixel = 0;
            uint32_t rowsPerStrip = 0;
            uint32_t stripOffsets = 0;
            uint32_t stripByteCounts = 0;

            for (const auto& entry : ifdEntries)
            {
                switch (entry.tag)
                {
                case 0x0100:
                    width = entry.valueOffset;
                    break;

                case 0x0101:
                    height = entry.valueOffset;
                    break;

                case 0x0102:
                    bitsPerSample = entry.valueOffset;
                    break;

                case 0x0115:
                    samplesPerPixel = entry.valueOffset;
                    break;

                case 0x0116:
                    rowsPerStrip = entry.valueOffset;
                    break;

                case 0x0111:
                    stripOffsets = entry.valueOffset;
                    break;

                case 0x0117:
                    stripByteCounts = entry.valueOffset;
                    break;
                }
            }

            if (bitsPerSample != 8 || samplesPerPixel != 3)
            {
                throw std::runtime_error("Unsupported TIFF format"s);
            }

            Image image(width, height, Color::Black());

            file.seekg(stripOffsets, std::ios::beg);
            std::vector<uint8_t> buffer(width * height * samplesPerPixel);

            file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
            if (!file)
            {
                return {};
            }

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    int index = (y * width + x) * samplesPerPixel;
                    image.GetPixel(x, y) = { buffer[index], buffer[index + 1], buffer[index + 2] };
                }
            }
            return image;
        }

        bool TiffImage::SaveImageTiff(const Path& path_, const Image& image_)
        {
            std::ofstream file(path_, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Failed to create TIFF file: "s + path_.string());
            }

            uint32_t ifdOffset = sizeof(TiffHeader);
            TiffHeader header = { 0x4949, 42, ifdOffset };

            file.write(reinterpret_cast<char*>(&header), sizeof(header));
            if (!file)
            {
                return false;
            }

            uint16_t entryCount = 8;

            file.write(reinterpret_cast<char*>(&entryCount), sizeof(entryCount));
            if (!file)
            {
                return false;
            }

            uint32_t width = image_.GetWidth();
            uint32_t height = image_.GetHeight();
            uint32_t bitsPerSample = 8;
            uint32_t samplesPerPixel = 3;
            uint32_t rowsPerStrip = height;
            uint32_t stripOffsets = sizeof(TiffHeader) + sizeof(entryCount) + entryCount * sizeof(IFDEntry) + sizeof(uint32_t);
            uint32_t stripByteCounts = width * height * sizeof(Color);

            IFDEntry entries[] =
            {
                {0x0100, 3, 1, width},
                {0x0101, 3, 1, height},
                {0x0102, 3, 1, bitsPerSample},
                {0x0115, 3, 1, samplesPerPixel},
                {0x0116, 3, 1, rowsPerStrip},
                {0x0111, 4, 1, stripOffsets},
                {0x0117, 4, 1, stripByteCounts},
                {0x0118, 3, 1, 1}
            };

            file.write(reinterpret_cast<char*>(entries), sizeof(entries));
            if (!file)
            {
                return false;
            }

            uint32_t nextIFDOffset = 0;

            file.write(reinterpret_cast<char*>(&nextIFDOffset), sizeof(nextIFDOffset));
            if (!file)
            {
                return false;
            }

            for (int y = 0; y < image_.GetHeight(); ++y)
            {
                for (int x = 0; x < image_.GetWidth(); ++x)
                {
                    const Color& color = image_.GetPixel(x, y);

                    file.write(reinterpret_cast<const char*>(&color.r), sizeof(color.r));
                    file.write(reinterpret_cast<const char*>(&color.g), sizeof(color.g));
                    file.write(reinterpret_cast<const char*>(&color.b), sizeof(color.b));
                }
            }
            return true;
        }

    } // end namespace tiff_image

} // end namespace img_lib

