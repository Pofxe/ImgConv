## Current status ##

Supports:
- .tiff
- .bmp
- .ppm
- .p3
- .jpg/.jpeg
- .ico
  
Planning:
- .png
- .gif

## Install ##


#### Compiling

```bash
git clone https://github.com/Pofxe/ImgConv.git
cd ImgConv
mkdir build
cd build
cmake ..
cmake --build .
```

## Requirements ##

- [libpng](https://github.com/pnggroup/libpng.git)
- [libjpeg](https://github.com/winlibs/libjpeg.git)

## Usage ##

```bash
./ImgConv <input_file> <output_file>
```
Example
```bash
./ImgConv image1.ppm image2.bmp
```
