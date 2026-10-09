#ifndef __DS18B20_SAFE_H__
#define __DS18B20_SAFE_H__

#include <library.h>

bit DS18B20_StartConversion(void);
bit DS18B20_ConversionReady(uint elapsed_ms);
/* Call only after a successful start and at least 750 ms of externally
 * measured elapsed time. On reset/CRC failure, output is unchanged. */
bit DS18B20_ReadTemperature(uint elapsed_ms, float *temperature_c);

#endif
