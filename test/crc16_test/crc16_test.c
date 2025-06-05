#include <stdio.h>
#include <stdint.h>
#include "crc16.h"

int main()
{
    uint8_t test[6] = {0x01,0x03,0x06,0x00,0x00,0x02};
    uint16_t crc;   // 理论值应为0x83C4

    crc = crc16_calculate(test, sizeof(test));
    printf("0x%04X\r\n", crc);
    return 0;
}

