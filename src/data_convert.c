#include "data_convert.h"

/**
 * @brief  将 uint32_t / int32_t 数据转换成4个u8（大端）
 * @param  input 输入的32位数据
 * @param  buf u8数组指针
 * @retval none
 */
void u32_to_u8_BE(uint32_t input, uint8_t* buf)
{
    buf[0] = (input >> 24) & 0xFF;  // 高8位
    buf[1] = (input >> 16) & 0xFF;  // 次高8位
    buf[2] = (input >> 8) & 0xFF;   // 次低8位
    buf[3] = input & 0xFF;          // 低8位
}

/**
 * @brief  将 uint32_t / int32_t 转换成4个u8（小端）
 * @param  input 输入的32位数据
 * @param  buf u8数组指针
 * @retval none
 */
void u32_to_u8_LE(uint32_t input, uint8_t* buf)
{
    /* 考虑通用性，采用手动位移不用 memcpy  */
    buf[0] = input & 0xFF;          // 低8位
    buf[1] = (input >> 8) & 0xFF;   // 次低8位
    buf[2] = (input >> 16) & 0xFF;  // 次高8位
    buf[3] = (input >> 24) & 0xFF;  // 高8位
}

/**
 * @brief  将 uint16_t / int16_t 转换成2个u8（大端）
 * @param  input 输入的16位数据
 * @param  buf u8数组指针
 * @retval none
 */
void u16_to_u8_BE(uint16_t input, uint8_t* buf)
{
    buf[0] = (input >> 8) & 0xFF;  // 高8位
    buf[1] = input & 0xFF;         // 低8位
}

/**
 * @brief  将 uint16_t / int16_t 转换成2个u8（小端）
 * @param  input 输入的16位数据
 * @param  buf u8数组指针
 * @retval none
 */
void u16_to_u8_LE(uint16_t input, uint8_t* buf)
{
    buf[0] = input & 0xFF;         // 低8位
    buf[1] = (input >> 8) & 0xFF;  // 高8位
}

/**
 * @brief  将float转换成4个u8（大端）
 * @param  input 输入的float型数据
 * @param  buf u8数组指针
 * @retval none
 */
void float_to_u8_BE(float input, uint8_t* buf)
{
    floatToBytes data;
    data.f = input;

    buf[0] = data.bytes[3];
    buf[1] = data.bytes[2];
    buf[2] = data.bytes[1];
    buf[3] = data.bytes[0];
}

/**
 * @brief  将float转换成4个u8（小端）
 * @param  input 输入的float型数据
 * @param  buf u8数组指针
 * @retval none
 */
void float_to_u8_LE(float input, uint8_t* buf)
{
    floatToBytes data;
    data.f = input;

    buf[0] = data.bytes[0];
    buf[1] = data.bytes[1];
    buf[2] = data.bytes[2];
    buf[3] = data.bytes[3];
}

/**
 * @brief  将两个大端序的字节数组转换为u16
 * @param  BEbuf 字节数组指针
 * @retval 转换结果
 */
uint16_t BE_bytes_to_u16(uint8_t* BEbuf)
{
    return (((uint16_t)BEbuf[0] << 8) | BEbuf[1]);
}

/**
 * @brief  将int数据转换为float，最高支持7位小数
 * @param  int_data int数据
 * @retval 转换结果
 */
float int2float(int32_t int_data, uint8_t dot)
{
    /* 10^n */
    static const float dot_div_table[] = {
        1.0f,         // dot = 0
        10.0f,        // dot = 1
        100.0f,       // dot = 2
        1000.0f,      // dot = 3
        10000.0f,     // dot = 4
        100000.0f,    // dot = 5
        1000000.0f,   // dot = 6
        10000000.0f,  // dot = 7
    };
    /* 最大能计算的小数位数 */
    uint8_t max_dot = sizeof(dot_div_table) / sizeof(dot_div_table[0]) - 1;

    if (dot > max_dot)
    {
        dot = max_dot;
    }
    return int_data / dot_div_table[dot];
}
