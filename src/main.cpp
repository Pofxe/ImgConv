#include <iostream>
#include <stdexcept>

#include "ppm_image.h"
#include "bmp_image.h"
#include "ico_image.h"
#include "tiff_image.h"
#include "png_image.h"
#include "jpeg_image.h"

#include "image.h"

using namespace std;

using img_lib::Image;
using img_lib::Path;

enum class Format { PPM, BMP, TIFF, PNG, JPEG, UNKNOWN };

Format GetFormatByExtension(const Path& input_file_)
{
    const string ext = input_file_.extension().string();

    if (ext == ".ppm"s || ext == ".p3"s)
    {
        return Format::PPM;
    }

    if (ext == ".bmp"s)
    {
        return Format::BMP;
    }

    if (ext == ".tiff"s)
    {
        return Format::TIFF;
    }

    if (ext == ".png"s)
    {
        return Format::PNG;
    }

    if (ext == ".jpeg" || ext == ".jpg")
    {
        return Format::JPEG;
    }

    return Format::UNKNOWN;
}

Image LoadImage(const Path& input_file_, Format format_)
{
    img_lib::ppm_image::PpmImage ppm_image;
    img_lib::bmp_image::BmpImage bmp_image;
    img_lib::tiff_image::TiffImage tiff_image;
    img_lib::png_image::PngImage png_image;
    img_lib::jpeg_image::JpegImage jpeg_image;

    switch (format_) 
    {
    case Format::PPM:
        return ppm_image.LoadImagePPM(input_file_);

    case Format::BMP:
        return bmp_image.LoadImageBMP(input_file_);

    case Format::TIFF:
        return tiff_image.LoadImageTiff(input_file_);

    case Format::PNG:
        return png_image.LoadImagePng(input_file_);

    case Format::JPEG:
        return jpeg_image.LoadImageJpeg(input_file_);

    default:
        throw std::runtime_error("Unsupported input file format"s);
    }
}

void SaveImage(const Path& output_file_, const Image& image_, Format format_)
{
    img_lib::ppm_image::PpmImage ppm_image;
    img_lib::bmp_image::BmpImage bmp_image;
    img_lib::tiff_image::TiffImage tiff_image;
    img_lib::png_image::PngImage png_image;
    img_lib::jpeg_image::JpegImage jpeg_image;

    switch (format_) 
    {
    case Format::PPM:
        ppm_image.SaveImagePPM(output_file_, image_);
        break;

    case Format::BMP:
        bmp_image.SaveImageBMP(output_file_, image_);
        break;

    case Format::TIFF:
        tiff_image.SaveImageTiff(output_file_, image_);
        break;

    case Format::PNG:
        png_image.SaveImagePng(output_file_, image_);
        break;

    case Format::JPEG:
        jpeg_image.SaveImageJpeg(output_file_, image_);
        break;

    default:
        throw std::runtime_error("Unsupported output file format"s);
    }
}

int main(int argc_, const char** argv_)
{
    if (argc_ != 3) 
    {
        cerr << "Usage: "s << argv_[0] << " <input_file> <output_file>"s << endl;
        return 1;
    }

    Path input_file = argv_[1];
    Path output_file = argv_[2];

    Format input_format = GetFormatByExtension(input_file);

    if (input_format == Format::UNKNOWN)
    {
        cerr << "Unknown input file format: "s << input_file.extension().string() << endl;
        return 1;
    }

   Image image;

    try 
    {
        image = LoadImage(input_file, input_format);
    }
    catch (const exception& e) 
    {
        cerr << "Error loading image: "s << e.what() << endl;
        return 1;
    }

    if (!image) 
    {
        cerr << "Failed to load image: "s << input_file.string() << endl;
        return 1;
    }

    Format output_format = GetFormatByExtension(output_file);

    if (output_format == Format::UNKNOWN) 
    {
        cerr << "Unknown output file format: "s << output_file.extension().string() << endl;
        return 1;
    }

    try 
    {
        SaveImage(output_file, image, output_format);
    }
    catch (const exception& e) 
    {
        cerr << "Error saving image: "s << e.what() << endl;
        return 1;
    }

    cout << "Image successfully converted from "s << input_file.string() << " to "s << output_file.string() << endl;

    return 0;
}