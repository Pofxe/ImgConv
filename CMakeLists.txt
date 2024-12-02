cmake_minimum_required(VERSION 3.10)

project(ImgConv)
set(CMAKE_CXX_STANDARD 17)

if(UNIX)
	set(LIBPNG_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/libpng/lib/libpng.a)
	set(LIBJPEG_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/libjpeg/lib/libjpeg.a)
	set(GIFLIB_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/giflib/lib/libgif.a)
	set(LIBPNG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libpng/include)
	set(LIBJPEG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libjpeg/include)
	set(GIFLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/giflib/include)
elseif(WIN32)
	set(LIBPNG_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/libpng/lib/libpng16.lib)
	set(LIBJPEG_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/libjpeg/lib/jpeg.lib)
	set(GIFLIB_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/giflib/lib/giflib.lib)
	set(LIBPNG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libpng/include)
	set(LIBJPEG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libjpeg/include)
	set(GIFLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/giflib/include)
	
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

set(SOURCES
    src/main.cpp
    src/image.cpp
    src/ppm_image.cpp
    src/ico_image.cpp
    src/tiff_image.cpp
    src/bmp_image.cpp
    src/png_image.cpp
    src/jpeg_image.cpp
    src/gif_image.cpp
)

set(HEADERS
    include/image.h
    include/ppm_image.h
    include/ico_image.h
    include/tiff_image.h
    include/bmp_image.h
    include/png_image.h
    include/jpeg_image.h
    include/gif_image.h
    include/pack_defines.h
)

add_executable(ImgConv ${SOURCES} ${HEADERS})

target_include_directories(ImgConv
    PRIVATE "${LIBPNG_INCLUDE_DIR}"
            "${LIBJPEG_INCLUDE_DIR}"
	    "${GIFLIB_INCLUDE_DIR}"
            "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(ImgConv PRIVATE ${LIBPNG_LIBRARY} ${LIBJPEG_LIBRARY} ${GIFLIB_LIBRARY})

