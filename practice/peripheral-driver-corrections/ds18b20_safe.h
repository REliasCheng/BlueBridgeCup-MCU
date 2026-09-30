#ifndef __DS18B20_SAFE_H__
#define __DS18B20_SAFE_H__

#include <library.h>

void DS18B20_StartConversion(void);
bit DS18B20_ConversionReady(uint elapsed_ms);
float DS18B20_ReadTemperature(void);
float DS18B20_ReadTemperatureBlocking(void);

#endif
