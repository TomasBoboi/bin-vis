#ifndef BMPTOOLS_H
#define BMPTOOLS_H

#define BMP_FILE_TYPE				0x424D
#define BMP_HEADER_SIZE				14
#define BMP_INFO_HEADER_SIZE		40
#define BMP_PALETTE_SIZE			(256*4)
#define BMP_COLOR_PLANES			1
#define BMP_COLOR_DEPTH				8
#define BMP_COMPRESSION_METHOD		0
#define BMP_HORIZONTAL_RESOLUTION	0
#define BMP_VERTICAL_RESOLUTION		0
#define BMP_COLOR_PALETTE			0
#define BMP_IMPORTANT_COLORS		0

#define BMP_COLOR_BLACK		0
#define BMP_COLOR_WHITE		255

typedef struct bmp_BitmapFileHeader_st bmp_BitmapFileHeader_t;
typedef struct bmp_BitmapInfoHeader_st bmp_BitmapInfoHeader_t;

void bmp_WriteImage(int outputFileDescriptor_fd, uint8_t **pixelData_ppu8, int32_t imageWidth_s32, int32_t imageHeight_s32);

#endif /* BMPTOOLS_H */