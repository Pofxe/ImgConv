#include "jpeg_image.h"

#include <stdexcept>
#include <cstdio>
#include <setjmp.h>
#include <stdio.h>

namespace img_lib
{
    namespace jpeg_image
    {
        struct my_error_mgr 
        {
            struct jpeg_error_mgr pub;
            jmp_buf setjmp_buffer;
        };

        typedef struct my_error_mgr* my_error_ptr;

        METHODDEF(void)
        my_error_exit (j_common_ptr cinfo_) 
        {
            my_error_ptr myerr = (my_error_ptr) cinfo_->err;
            (*cinfo_->err->output_message) (cinfo_);
            longjmp(myerr->setjmp_buffer, 1);
        }
        
        void SaveSсanlineToImage(const JSAMPLE* row_, int y, Image& out_image_) 
        {
            Color* line = out_image_.GetLine(y);
            for (int x = 0; x < out_image_.GetWidth(); ++x) 
            {
                const JSAMPLE* pixel = row_ + x * 3;
                line[x] = Color{pixel[0], pixel[1], pixel[2], {255}};
            }
        }

        const Image JpegImage::LoadImageJPEG(const Path& path_)
        {
            jpeg_decompress_struct cinfo;
            my_error_mgr jerr;
    
            FILE* file;
            JSAMPARRAY buffer;
            int row_stride;

            #ifdef _MSC_VER
            if ((file = _wfopen(path_.wstring().c_str(), L"rb")) == NULL)
            {
            #else
            if ((file = fopen(path_.string().c_str(), "rb")) == NULL) 
            {
            #endif
                return {};
            }

            cinfo.err = jpeg_std_error(&jerr.pub);
            jerr.pub.error_exit = my_error_exit;

            if (setjmp(jerr.setjmp_buffer)) 
            {
                jpeg_destroy_decompress(&cinfo);
                fclose(file);
                return {};
            }

            jpeg_create_decompress(&cinfo);
            jpeg_stdio_src(&cinfo, file);
            (void) jpeg_read_header(&cinfo, TRUE);

            cinfo.out_color_space = JCS_RGB;
            cinfo.output_components = 3;

            (void) jpeg_start_decompress(&cinfo);
    
            row_stride = cinfo.output_width * cinfo.output_components;
            buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
            Image image(cinfo.output_width, cinfo.output_height, Color::Black());

            while (cinfo.output_scanline < cinfo.output_height) 
            {
                int y = cinfo.output_scanline;
                (void) jpeg_read_scanlines(&cinfo, buffer, 1);

                SaveSсanlineToImage(buffer[0], y, image);
            }
            
            (void) jpeg_finish_decompress(&cinfo);
            jpeg_destroy_decompress(&cinfo);
            fclose(file);

            return image;
        }

        bool JpegImage::SaveImageJPEG(const Path& path_, const Image& image_) const
        {
            jpeg_compress_struct cinfo;
            jpeg_error_mgr jerr;
            FILE * file;      
            JSAMPROW row_pointer[1];  
            int row_stride;       

            cinfo.err = jpeg_std_error(&jerr);
            jpeg_create_compress(&cinfo);

            #ifdef _MSC_VER
            if ((file = _wfopen(path_.wstring().c_str(), L"wb")) == NULL) 
            {
            #else
            if ((file = fopen(path_.string().c_str(), "wb")) == NULL) 
            {
            #endif
                return false;
            }
            jpeg_stdio_dest(&cinfo, file);


            cinfo.image_width = image_.GetWidth(); 
            cinfo.image_height = image_.GetHeight();
            cinfo.input_components = 3;    
            cinfo.in_color_space = JCS_RGB; 

            jpeg_set_defaults(&cinfo);
            jpeg_start_compress(&cinfo, TRUE);

            row_stride = image_.GetWidth() * 3;

            while (cinfo.next_scanline < cinfo.image_height) 
            {       
                std::vector<JSAMPLE> jsample(row_stride);
                for (int i = 0; i < image_.GetWidth(); ++i) 
                {
                    jsample[3 * i] = JSAMPLE(image_.GetLine(cinfo.next_scanline)[i].r);
                    jsample[1 + 3 * i] = JSAMPLE(image_.GetLine(cinfo.next_scanline)[i].g);
                    jsample[2 + 3 * i] = JSAMPLE(image_.GetLine(cinfo.next_scanline)[i].b);
                }
                row_pointer[0] = &jsample[0];
                (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }

            jpeg_finish_compress(&cinfo);
            fclose(file);
            jpeg_destroy_compress(&cinfo);

            return true;
        }


    } // end namespace jpeg_image

} // end namespace img_lib