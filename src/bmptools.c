#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "bmptools.h"
#include "utils.h"

struct __attribute__((__packed__)) bmp_BitmapFileHeader_st {
    uint16_t    fileType_u16;
    uint32_t    fileSize_u32;
    uint16_t    reserved1_u16;
    uint16_t    reserved2_u16;
    uint32_t    pixelOffset_u32;
};

struct __attribute__((__packed__)) bmp_BitmapInfoHeader_st {
    uint32_t    infoHeaderSize_u32;
    int32_t     bitmapWidth_s32;
    int32_t     bitmapHeight_u32;
    uint16_t    colorPlanes_u16;
    uint16_t    colorDepth_u16;
    uint32_t    compressionMethod_u32;
    uint32_t    rawImageSize_u32;
    int32_t     horizontalResolution_s32;
    int32_t     verticalResolution_s32;
    uint32_t    colorPalette_u32;
    uint32_t    importantColors_u32;
};

static void bmp_WriteFileHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32);
static void bmp_WriteInfoHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32);
static void bmp_WriteColorPalette(int outputFileDescriptor_fd);

static void bmp_WriteFileHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32)
{
    printf("Writing bitmap file header... ");

    bmp_BitmapFileHeader_t bitmapFileHeader_t;
    
    bitmapFileHeader_t.fileType_u16 =   utils_LittleToBigEndian16(BMP_FILE_TYPE);
    bitmapFileHeader_t.fileSize_u32 =   imageWidth_s32 * imageHeight_s32 +
                                            BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE + (BMP_PALETTE_SIZE * 4);
    bitmapFileHeader_t.reserved1_u16    =   0x0000;
    bitmapFileHeader_t.reserved2_u16    =   0x0000;
    bitmapFileHeader_t.pixelOffset_u32  =   BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE + BMP_PALETTE_SIZE;
    
    if(write(outputFileDescriptor_fd, &bitmapFileHeader_t, sizeof(bitmapFileHeader_t)) < 0)
        utils_ErrorMessage("writing the bitmap file header");
    
    printf("done\n");
}

static void bmp_WriteInfoHeader(int outputFileDescriptor_fd, int32_t imageWidth_s32, int32_t imageHeight_s32)
{
    printf("Writing bitmap information header... ");

    bmp_BitmapInfoHeader_t bitmapInfoHeader_t;
    
    bitmapInfoHeader_t.infoHeaderSize_u32       = BMP_INFO_HEADER_SIZE;
    bitmapInfoHeader_t.bitmapWidth_s32          = imageWidth_s32;
    bitmapInfoHeader_t.bitmapHeight_u32         = imageHeight_s32;
    bitmapInfoHeader_t.colorPlanes_u16          = BMP_COLOR_PLANES;
    bitmapInfoHeader_t.colorDepth_u16           = BMP_COLOR_DEPTH;
    bitmapInfoHeader_t.compressionMethod_u32    = BMP_COMPRESSION_METHOD;
    bitmapInfoHeader_t.rawImageSize_u32         = imageWidth_s32 * imageHeight_s32 * BMP_COLOR_DEPTH / 8;
    bitmapInfoHeader_t.horizontalResolution_s32 = BMP_HORIZONTAL_RESOLUTION;
    bitmapInfoHeader_t.verticalResolution_s32   = BMP_VERTICAL_RESOLUTION;
    bitmapInfoHeader_t.colorPalette_u32         = BMP_COLOR_PALETTE;
    bitmapInfoHeader_t.importantColors_u32      = BMP_IMPORTANT_COLORS;
    
    if(write(outputFileDescriptor_fd, &bitmapInfoHeader_t, sizeof(bitmapInfoHeader_t)) < 0)
        utils_ErrorMessage("writing the bitmap information header");
    
    printf("done\n");
}

static void bmp_WriteColorPalette(int outputFileDescriptor_fd)
{
    printf("Writing the color palette... ");

    uint32_t currentColor_u32, colorToBeWritten_u32;
    
    for(currentColor_u32 = 0; currentColor_u32 < 256; currentColor_u32++)
    {
        colorToBeWritten_u32 = currentColor_u32;  colorToBeWritten_u32 <<= 8;
        colorToBeWritten_u32 |= currentColor_u32; colorToBeWritten_u32 <<= 8;
        colorToBeWritten_u32 |= currentColor_u32; colorToBeWritten_u32 <<= 8;
        colorToBeWritten_u32 &= 0xFFFFFF00;
        colorToBeWritten_u32 = utils_LittleToBigEndian32(colorToBeWritten_u32);
        
        if(write(outputFileDescriptor_fd, &colorToBeWritten_u32, sizeof(colorToBeWritten_u32)) < 0)
            utils_ErrorMessage("writing the color palette to the output file");
    }

    printf("done\n");
}

void bmp_WriteImage(int outputFileDescriptor_fd, uint8_t **pixelData_ppu8, int32_t imageWidth_s32, int32_t imageHeight_s32)
{
    printf("Writing the image to the output file...\n");

    int32_t row_s32, column_s32;
    int32_t unpaddedWidth_s32 = imageWidth_s32;

    while(imageWidth_s32 % 4 != 0) imageWidth_s32++;
    for(column_s32 = unpaddedWidth_s32; column_s32 < imageWidth_s32; column_s32++)
        for(row_s32 = 0; row_s32 < imageHeight_s32; row_s32++)
            pixelData_ppu8[row_s32][column_s32] = BMP_COLOR_WHITE;
    
    printf("\t");
    bmp_WriteFileHeader(outputFileDescriptor_fd, unpaddedWidth_s32, imageHeight_s32);

    printf("\t");
    bmp_WriteInfoHeader(outputFileDescriptor_fd, unpaddedWidth_s32, imageHeight_s32);

    printf("\t");
    bmp_WriteColorPalette(outputFileDescriptor_fd);
    
    for(row_s32 = imageHeight_s32 - 1; row_s32 >= 0; row_s32--)
    {
        if(row_s32 == imageHeight_s32 / 4)
            printf("\t75%%\n");
        else if(row_s32 == imageHeight_s32 / 2)
            printf("\t50%%\n");
        else if(row_s32 == 3 * imageHeight_s32 / 4)
            printf("\t25%%\n");
        
        for(column_s32 = 0; column_s32 < imageWidth_s32; column_s32++)
            if(write(outputFileDescriptor_fd, &pixelData_ppu8[row_s32][column_s32], sizeof(pixelData_ppu8[row_s32][column_s32])) < 0)
                utils_ErrorMessage("writing the pixel data to the output file");
    }
    printf("\t100%%\n");
}