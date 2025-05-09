#ifndef DATA_CONVERT_H
#define DATA_CONVERT_H

#include <stdint.h>

typedef union 
{
    float f;
    uint8_t bytes[4];
} floatToBytes;

void u32_to_u8_BE(uint32_t input, uint8_t* buf);
void u32_to_u8_LE(uint32_t input, uint8_t* buf);
void u16_to_u8_BE(uint16_t input, uint8_t* buf);
void u16_to_u8_LE(uint16_t input, uint8_t* buf);
void float_to_u8_BE(float input, uint8_t* buf);
void float_to_u8_LE(float input, uint8_t* buf);
uint16_t BE_bytes_to_u16(uint8_t* BEbuf);
float int2float(int32_t int_data, uint8_t dot);

#endif /* DATA_CONVERT_H */
