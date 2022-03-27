#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define REQUIRED_ARGS_NO_U32 2u
#define READ_BUFFER_SIZE_U32 16u

int fileDescriptor_fd = -1;
ssize_t numberOfBytesRead_sz = 0;
uint8_t readBytes_au8[READ_BUFFER_SIZE_U32] = {0};

uint32_t binaryFileSize_u32 = 0;
uint8_t * binaryFileContents_pu8;
struct stat binaryFileStats_st;

int main(int argc, char **argv)
{
    if(REQUIRED_ARGS_NO_U32 != argc)
    {
        printf("Usage: ./bin-vis <file-name>\n");
        exit(EXIT_FAILURE);
    }

    fileDescriptor_fd = open(argv[1], O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
    if(-1 == fileDescriptor_fd)
    {
        printf("Invalid input file!");
        exit(EXIT_FAILURE);
    }

    if(-1 == fstat(fileDescriptor_fd, &binaryFileStats_st))
    {
        printf("Could not retrieve input file characteristics!");
        exit(EXIT_FAILURE);
    }

    binaryFileSize_u32 = binaryFileStats_st.st_size;
    binaryFileContents_pu8 = (uint8_t *)malloc(binaryFileSize_u32 * sizeof(uint8_t));

    numberOfBytesRead_sz = read(fileDescriptor_fd, &binaryFileContents_pu8, binaryFileSize_u32);
    if(numberOfBytesRead_sz != binaryFileSize_u32)
    {
        printf("Could not read the entire input file!");
        exit(EXIT_FAILURE);
    }

    free(binaryFileContents_pu8);

    return 0;
}