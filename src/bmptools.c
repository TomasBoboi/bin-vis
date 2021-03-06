#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "bmptools.h"
#include "utils.h"

struct __attribute__((__packed__)) bmp_BitmapFileHeader_st {
    uint16_t fileType_u16;
    uint32_t fileSize_u32;
    uint16_t reserved1_u16;
    uint16_t reserved2_u16;
    uint32_t pixelOffset_u32;
};

struct __attribute__((__packed__)) bmp_BitmapInfoHeader_st {
    uint32_t infoHeaderSize_u32;
    int32_t  bitmapWidth_s32;
    int32_t  bitmapHeight_u32;
    uint16_t colorPlanes_u16;
    uint16_t colorDepth_u16;
    uint32_t compressionMethod_u32;
    uint32_t rawImageSize_u32;
    int32_t  horizontalResolution_s32;
    int32_t  verticalResolution_s32;
    uint32_t colorPalette_u32;
    uint32_t importantColors_u32;
};

static void bmp_WriteFileHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32);
static void bmp_WriteInfoHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32);
static void bmp_WriteColorPalette(int outputFileDescriptor_fd);

static void bmp_WriteFileHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32)
{
    bmp_BitmapFileHeader_t bitmapFileHeader_t;

    bitmapFileHeader_t.fileType_u16 =   utils_LittleToBigEndian16(BMP_FILE_TYPE);
    bitmapFileHeader_t.fileSize_u32 =   (imageWidth_s32 * imageHeight_s32) +
                                          BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE + BMP_PALETTE_SIZE;
    bitmapFileHeader_t.reserved1_u16    = 0x0000;
    bitmapFileHeader_t.reserved2_u16    = 0x0000;
    bitmapFileHeader_t.pixelOffset_u32  = BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE + BMP_PALETTE_SIZE;

    if(write(outputFileDescriptor_fd, &bitmapFileHeader_t, sizeof(bitmapFileHeader_t)) < 0)
    {
        utils_ErrorMessage("writing the bitmap file header");
    }
}

static void bmp_WriteInfoHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32)
{
    bmp_BitmapInfoHeader_t bitmapInfoHeader_t;

    bitmapInfoHeader_t.infoHeaderSize_u32       = BMP_INFO_HEADER_SIZE;
    bitmapInfoHeader_t.bitmapWidth_s32          = imageWidth_s32 * 8;
    bitmapInfoHeader_t.bitmapHeight_u32         = imageHeight_s32;
    bitmapInfoHeader_t.colorPlanes_u16          = BMP_COLOR_PLANES;
    bitmapInfoHeader_t.colorDepth_u16           = BMP_COLOR_DEPTH;
    bitmapInfoHeader_t.compressionMethod_u32    = BMP_COMPRESSION_METHOD;
    bitmapInfoHeader_t.rawImageSize_u32         = imageWidth_s32 * imageHeight_s32 * BMP_COLOR_DEPTH;
    bitmapInfoHeader_t.horizontalResolution_s32 = BMP_HORIZONTAL_RESOLUTION;
    bitmapInfoHeader_t.verticalResolution_s32   = BMP_VERTICAL_RESOLUTION;
    bitmapInfoHeader_t.colorPalette_u32         = BMP_COLOR_PALETTE;
    bitmapInfoHeader_t.importantColors_u32      = BMP_IMPORTANT_COLORS;

    if(write(outputFileDescriptor_fd, &bitmapInfoHeader_t, sizeof(bitmapInfoHeader_t)) < 0)
    {
        utils_ErrorMessage("writing the bitmap information header");
    }
}

static void bmp_WriteColorPalette(int outputFileDescriptor_fd)
{
    uint32_t currentColor_u32;

    currentColor_u32 = 0x00000000;
    if(write(outputFileDescriptor_fd, &currentColor_u32, sizeof(currentColor_u32)) < 0)
    {
        utils_ErrorMessage("writing the color palette to the output file");
    }

    currentColor_u32 = 0x00FFFFFF;
    if(write(outputFileDescriptor_fd, &currentColor_u32, sizeof(currentColor_u32)) < 0)
    {
        utils_ErrorMessage("writing the color palette to the output file");
    }
}

void bmp_WriteImage(int outputFileDescriptor_fd, uint8_t **pixelData_ppu8, int32_t imageWidth_s32, int32_t imageHeight_s32)
{
    printf("Writing the image to the output file...\n");

    int32_t row_s32, column_s32;

    bmp_WriteFileHeader(outputFileDescriptor_fd, imageWidth_s32, imageHeight_s32);
    bmp_WriteInfoHeader(outputFileDescriptor_fd, imageWidth_s32, imageHeight_s32);
    bmp_WriteColorPalette(outputFileDescriptor_fd);

    for(row_s32 = imageHeight_s32 - 1; row_s32 >= 0; row_s32--)
    {
        if(write(outputFileDescriptor_fd, pixelData_ppu8[row_s32], imageWidth_s32) < 0)
        {
            utils_ErrorMessage("writing the pixel data to the output file");
        }
    }
}