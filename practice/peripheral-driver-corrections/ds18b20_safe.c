#include "ds18b20_safe.h"

sbit SAFE_DQ = P1^4;

static void DS18B20_Delay(uint count)
{
    uchar i;
    while(count--) {
        for(i = 0; i < 12; ++i);
    }
}

static bit DS18B20_Reset(void)
{
    bit presence;
    SAFE_DQ = 1;
    DS18B20_Delay(12);
    SAFE_DQ = 0;
    DS18B20_Delay(80);
    SAFE_DQ = 1;
    DS18B20_Delay(10);
    presence = SAFE_DQ;
    DS18B20_Delay(5);
    return presence == 0;
}

static void DS18B20_WriteByte(uchar value)
{
    uchar i;
    for(i = 0; i < 8; ++i) {
        SAFE_DQ = 0;
        SAFE_DQ = value & 0x01;
        DS18B20_Delay(5);
        SAFE_DQ = 1;
        value >>= 1;
    }
    DS18B20_Delay(5);
}

static uchar DS18B20_ReadByte(void)
{
    uchar i;
    uchar value = 0;
    for(i = 0; i < 8; ++i) {
        SAFE_DQ = 0;
        value >>= 1;
        SAFE_DQ = 1;
        if(SAFE_DQ) {
            value |= 0x80;
        }
        DS18B20_Delay(5);
    }
    return value;
}

void DS18B20_StartConversion(void)
{
    if(DS18B20_Reset()) {
        DS18B20_WriteByte(0xCC);
        DS18B20_WriteByte(0x44);
    }
}

float DS18B20_ReadTemperature(void)
{
    uchar low;
    uchar high;
    int raw;
    if(!DS18B20_Reset()) {
        return 0.0f;
    }
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE);
    low = DS18B20_ReadByte();
    high = DS18B20_ReadByte();
    raw = (int)(((uint)high << 8) | low);
    return raw * 0.0625f;
}

float DS18B20_ReadTemperatureBlocking(void)
{
    DS18B20_StartConversion();
    DS18B20_Delay(62500U);
    return DS18B20_ReadTemperature();
}
