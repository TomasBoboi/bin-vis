#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint16_t utils_LittleToBigEndian16(uint16_t value_u16)
{
	uint8_t highNibble_u8 = (value_u16) >> 8;
	uint8_t lowNibble_u8 = (value_u16) & 0xFF;
	
	return (lowNibble_u8 << 8) | highNibble_u8;
}

uint32_t utils_LittleToBigEndian32(uint32_t value_u16)
{
	uint16_t highBytes_u16 = (value_u16) >> 16;
	uint16_t lowBytes_u16 = (value_u16) & 0x0000FFFF;
	
	highBytes_u16 = utils_LittleToBigEndian16(highBytes_u16);
	lowBytes_u16 = utils_LittleToBigEndian16(lowBytes_u16);
	
	return (lowBytes_u16 << 16) | highBytes_u16;
}

void utils_ErrorMessage(char *errorMessage_pc)
{
	printf("Error: %s\n", errorMessage_pc);
	exit(EXIT_FAILURE);
}