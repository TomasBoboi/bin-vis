#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "bmptools.h"
#include "utils.h"

#define REQUIRED_ARGS_NO_U32 3u

char*   inputFileName_pc;
int     inputFileDescriptor_fd = -1;
ssize_t inputNumberOfBytesRead_sz = 0;

struct stat binaryFileStats_st;
uint32_t    binaryFileSize_u32 = 0;
uint8_t**   binaryFileContents_ppu8;

char*   outputFileName_pc = "./a.bmp";
int     outputFileDescriptor_fd = -1;

int32_t outputImageWidth_s32 = 128;
int32_t outputImageHeight_s32;

int main(int argc, char **argv)
{
    if(REQUIRED_ARGS_NO_U32 > argc || 0 == argc % 2)
    {
        utils_ErrorMessage("Usage: ./bin-vis -i <input-file-name> [-o <output-file-name>] [-w <image-width>]\n");
    }

    for(uint32_t argIndex_u32 = 1; argIndex_u32 < argc; argIndex_u32++)
    {
        switch(argv[argIndex_u32][1])
        {
            case 'i':
                inputFileName_pc = argv[++argIndex_u32];
                break;

            case 'o':
                outputFileName_pc = argv[++argIndex_u32];
                break;

            case 'w':
                outputImageWidth_s32 = atoi(argv[++argIndex_u32]);
                break;

            default:
                utils_ErrorMessage("Usage: ./bin-vis -i <input-file-name> [-o <output-file-name>] [-w <image-width>]\n");
                break;
        }
    }

    inputFileDescriptor_fd = open(inputFileName_pc, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
    if(-1 == inputFileDescriptor_fd)
    {
        utils_ErrorMessage("invalid input file");
    }

    if(-1 == fstat(inputFileDescriptor_fd, &binaryFileStats_st))
    {
        utils_ErrorMessage("could not retrieve input file characteristics");
    }

    binaryFileSize_u32 = binaryFileStats_st.st_size;

    outputImageHeight_s32 = binaryFileSize_u32 / outputImageWidth_s32;

    if(0 != binaryFileSize_u32 % outputImageWidth_s32)
    {
        outputImageHeight_s32++;
    }

    binaryFileContents_ppu8 = (uint8_t **)malloc(outputImageHeight_s32 * sizeof(uint8_t *));
    for(int32_t rowIndex_s32 = 0; rowIndex_s32 < outputImageHeight_s32; rowIndex_s32++)
    {
        binaryFileContents_ppu8[rowIndex_s32] = (uint8_t *)malloc(outputImageWidth_s32 * sizeof(uint8_t));

        for(int32_t columnIndex_s32 = 0; columnIndex_s32 < outputImageWidth_s32; columnIndex_s32++)
        {
            binaryFileContents_ppu8[rowIndex_s32][columnIndex_s32] = 0;
        }

        inputNumberOfBytesRead_sz += read(inputFileDescriptor_fd, binaryFileContents_ppu8[rowIndex_s32], outputImageWidth_s32);
    }

    if(inputNumberOfBytesRead_sz != binaryFileSize_u32)
    {
        utils_ErrorMessage("could not read the entire input file");
    }

    outputFileDescriptor_fd = open(outputFileName_pc, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if(-1 == outputFileDescriptor_fd)
    {
        utils_ErrorMessage("invalid output file");
    }

    bmp_WriteImage(outputFileDescriptor_fd, binaryFileContents_ppu8, outputImageWidth_s32, outputImageHeight_s32);

    close(inputFileDescriptor_fd);
    close(outputFileDescriptor_fd);

    for(int32_t rowIndex_s32 = 0; rowIndex_s32 < outputImageHeight_s32; rowIndex_s32++)
    {
        free(binaryFileContents_ppu8[rowIndex_s32]);
    }
    free(binaryFileContents_ppu8);

    return 0;
}