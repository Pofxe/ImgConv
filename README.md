## Current status ##

Supports:
- .tiff
- .bmp
- .ppm
- .p3
- .jpg / .jpeg
- .ico
- .gif (static/grayscale)
- .png
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

```bash
git clone https://github.com/Pofxe/ImgConv.git
cd ImgConv
docker build -t my_project_image .
docker run -it --rm my_project_image
```

## Requirements ##

- [libpng](https://github.com/pnggroup/libpng.git)
- [libjpeg](https://github.com/winlibs/libjpeg.git)
- [giflib](https://giflib.sourceforge.net/)

## Usage ##

```bash
./ImgConv <input_file> <output_file>
```
Example
```bash
./ImgConv image1.ppm image2.bmp
```
