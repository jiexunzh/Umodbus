#ifndef __CRC16_H_
#define __CRC16_H_

#include <stdint.h>

uint16_t crc16_modbus(const uint8_t* ptr, uint16_t len);

#endif
