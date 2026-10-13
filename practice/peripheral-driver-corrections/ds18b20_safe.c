#include "ds18b20_safe.h"
#include "peripheral_policy.h"

#ifdef HOST_TEST
extern volatile uchar SAFE_DQ;
#else
sbit SAFE_DQ = P1^4;
#endif
static bit conversion_started;

static bit DS18B20_Reset(void)
{
    bit presence;
    SAFE_DQ = 1;
    DS18B20_DelayUs(5);
    SAFE_DQ = 0;
    DS18B20_DelayUs(480);
    SAFE_DQ = 1;
    DS18B20_DelayUs(70);
    presence = SAFE_DQ;
    DS18B20_DelayUs(410);
    return presence == 0;
}

static void DS18B20_WriteByte(uchar value)
{
    uchar i;
    for(i = 0; i < 8; ++i) {
        SAFE_DQ = 0;
        if(value & 0x01) {
            DS18B20_DelayUs(6);
            SAFE_DQ = 1;
            DS18B20_DelayUs(64);
        } else {
            DS18B20_DelayUs(60);
            SAFE_DQ = 1;
            DS18B20_DelayUs(10);
        }
        value >>= 1;
    }
}

static uchar DS18B20_ReadByte(void)
{
    uchar i;
    uchar value = 0;
    for(i = 0; i < 8; ++i) {
        SAFE_DQ = 0;
        DS18B20_DelayUs(3);
        value >>= 1;
        SAFE_DQ = 1;
        DS18B20_DelayUs(8);
        if(SAFE_DQ) {
            value |= 0x80;
        }
        DS18B20_DelayUs(59);
    }
    return value;
}

bit DS18B20_StartConversion(void)
{
    if(!DS18B20_Reset()) {
        conversion_started = 0;
        return 0;
    }
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0x44);
    conversion_started = 1;
    return 1;
}

bit DS18B20_ConversionReady(uint elapsed_ms)
{
    return conversion_started && PeripheralPolicy_Ds18b20Ready(elapsed_ms) != 0;
}

bit DS18B20_ReadTemperature(uint elapsed_ms, float *temperature_c)
{
    uchar scratchpad[9];
    uchar index;
    if(temperature_c == 0 || !DS18B20_ConversionReady(elapsed_ms)) {
        return 0;
    }
    conversion_started = 0;
    if(!DS18B20_Reset()) {
        return 0;
    }
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE);
    for(index = 0U; index < 9U; ++index) {
        scratchpad[index] = DS18B20_ReadByte();
    }
    return PeripheralPolicy_Ds18b20Decode(scratchpad, temperature_c) != 0U;
}
