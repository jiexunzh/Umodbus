/**
 * *********************************************************************************
 * @author ZJX
 * @note 基于Modbus寄存器数据映射表，实现读写Modbus保持寄存器（03H、10H功能码）通用功能
 *       适用于 Modbus RTU / Modbus TCP / Modbus over CANFD
 *       目前支持读写8位数据、16位数据、32位数据，32位数据需占用两个连续的Modbus寄存器，
 *       此时低寄存器存放高16位，高寄存器存放低16位。（并且每个寄存器仍是高字节在前，即模拟大端模式）
 *
 * **********************************************************************************
 */
#include "modbus_general.h"

static void write_high_16bits(uint32_t* p_data32, uint16_t data16);
static void write_low_16bits(uint32_t* p_data32, uint16_t data16);

/**
 * @brief  读取指定地址的Modbus寄存器
 * @param  reg_addr 寄存器地址  ModbusSlave_TypeDef
 * @param  p_read 读取返回值指针
 * @retval 读取结果
 */
RegRead_TypeDef read_modbus_reg(ModbusSlave_TypeDef* p_mbslave,
                                uint16_t reg_addr,
                                uint16_t* p_read)
{
    uint16_t value = 0;
    RegRead_TypeDef RegRead_state = RegRead_ADDR_ERROR;
    uint16_t i;

    /* 遍历Modbus寄存器数据映射表 */
    for (i = 0; i < p_mbslave->map_num; i++)
    {
        /* 数据使用1个Modbus寄存器（8位或16位数据） */
        if (p_mbslave->reg_map_table[i].data_type == USE_ONE_REG_8BIT || p_mbslave->reg_map_table[i].data_type == USE_ONE_REG_16BIT)
        {
            /* reg_addr == 起始地址 */
            if (reg_addr == p_mbslave->reg_map_table[i].start_addr)
            {
                if (p_mbslave->reg_map_table[i].data_type == USE_ONE_REG_8BIT) /* 8位数据 */
                {
                    value = *(uint8_t*)p_mbslave->reg_map_table[i].p_data;
                }
                else /* 16位数据 */
                {
                    value = *(uint16_t*)p_mbslave->reg_map_table[i].p_data;
                }
                RegRead_state = RegRead_SUCCESS;
                break;
            }
        }
        /* 数据使用2个Modbus寄存器（32位数据） */
        else if (p_mbslave->reg_map_table[i].data_type == USE_TWO_REG_32BIT)
        {
            /* reg_addr == 起始地址 || 起始地址 + 1 */
            if (reg_addr == p_mbslave->reg_map_table[i].start_addr || reg_addr == p_mbslave->reg_map_table[i].start_addr + 1)
            {
                if (reg_addr == p_mbslave->reg_map_table[i].start_addr) /* 低地址寄存器 */
                {
                    value = (*(uint32_t*)p_mbslave->reg_map_table[i].p_data) >> 16; /* 低地址寄存器存放32位数据的高16位 */
                }
                else /* 高地址寄存器 */
                {
                    value = (*(uint32_t*)p_mbslave->reg_map_table[i].p_data) & 0xffff; /* 高地址寄存器存放数据的低16位 */
                }
                RegRead_state = RegRead_SUCCESS;
                break;
            }
        }
    }

    if (RegRead_state == RegRead_SUCCESS)
    {
        *p_read = value;
    }

    return RegRead_state;
}

/**
 * @brief  写入指定地址的Modbus寄存器
 * @param  reg_addr 寄存器地址
 * @param  reg_value 写入值
 * @retval 写入结果
 */
RegWrite_TypeDef write_modbus_reg(ModbusSlave_TypeDef* p_mbslave,
                                  uint16_t reg_addr,
                                  uint16_t reg_value)
{
    uint16_t i;

    /* 遍历Modbus寄存器数据映射表 */
    for (i = 0; i < p_mbslave->map_num; i++)
    {
        /* 数据使用1个Modbus寄存器（8位或16位数据） */
        if (p_mbslave->reg_map_table[i].data_type == USE_ONE_REG_8BIT || p_mbslave->reg_map_table[i].data_type == USE_ONE_REG_16BIT)
        {
            if (reg_addr == p_mbslave->reg_map_table[i].start_addr)
            {
                if (p_mbslave->reg_map_table[i].access_type == ONLY_READ)
                {
                    return RegWrite_ACCESS_ERROR;
                }
                if (p_mbslave->reg_map_table[i].data_type == USE_ONE_REG_8BIT) /* 8位数据 */
                {
                    *(uint8_t*)p_mbslave->reg_map_table[i].p_data = (reg_value & 0xFF);
                }
                else /* 16位数据 */
                {
                    *(uint16_t*)p_mbslave->reg_map_table[i].p_data = reg_value;
                }
                return RegWrite_SUCCESS;
            }
        }
        /* 数据使用2个Modbus寄存器（32位数据） */
        else if (p_mbslave->reg_map_table[i].data_type == USE_TWO_REG_32BIT)
        {
            /* 判断寄存器地址 */
            if (reg_addr == p_mbslave->reg_map_table[i].start_addr || reg_addr == p_mbslave->reg_map_table[i].start_addr + 1)
            {
                if (p_mbslave->reg_map_table[i].access_type == ONLY_READ)
                {
                    return RegWrite_ACCESS_ERROR;
                }
                if (reg_addr == p_mbslave->reg_map_table[i].start_addr) /* 低地址寄存器 */
                {
                    write_high_16bits((uint32_t*)p_mbslave->reg_map_table[i].p_data, reg_value); /* 将值写入到32位数据的高16位 */
                }
                else /* 高地址寄存器 */
                {
                    write_low_16bits((uint32_t*)p_mbslave->reg_map_table[i].p_data, reg_value); /* 将值写入到32位数据的低16位 */
                }
                return RegWrite_SUCCESS;
            }
        }
    }

    return RegWrite_ADDR_ERROR;
}

/**
 * @brief  将16位数据写入到32位数据的高16位
 * @param  p_data32 32位数据指针
 * @param  data16 16位数据
 * @retval none
 */
static void write_high_16bits(uint32_t* p_data32, uint16_t data16)
{
    uint32_t data16_left;

    // 清除data32的高16位
    *p_data32 &= ~0xFFFF0000;

    data16_left = ((uint32_t)data16) << 16;

    *p_data32 |= data16_left;
}

/**
 * @brief  将16位数据写入到32位数据的低16位
 * @param  p_data32 32位数据指针
 * @param  data16 16位数据
 * @retval none
 */
static void write_low_16bits(uint32_t* p_data32, uint16_t data16)
{
    // 清除data32的低16位
    *p_data32 &= ~0x0000FFFF;

    *p_data32 |= (uint32_t)data16;
}
