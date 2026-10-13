/* Host trace of the original driver; does not emulate MCU cycles or a bus. */
#include <assert.h>
#include <stdio.h>

#include "../ds18b20_safe.c"

volatile uchar SAFE_DQ = 1;
static uint delays[64];
static unsigned count;

void DS18B20_DelayUs(uint microseconds)
{
    assert(count < sizeof(delays) / sizeof(delays[0]));
    delays[count++] = microseconds;
    if (microseconds == 70) SAFE_DQ = 0; /* Stub a presence pulse. */
    if (microseconds == 8) SAFE_DQ = 1;  /* Stub a read-one bit. */
}

int main(void)
{
    unsigned i;
    count = 0;
    assert(DS18B20_Reset() == 1);
    assert(count == 4 && delays[0] == 5 && delays[1] == 480 &&
           delays[2] == 70 && delays[3] == 410);

    count = 0;
    DS18B20_WriteByte(0x01);
    assert(count == 16 && delays[0] == 6 && delays[1] == 64);
    for (i = 1; i < 8; ++i) {
        assert(delays[2 * i] == 60 && delays[2 * i + 1] == 10);
    }

    count = 0;
    assert(DS18B20_ReadByte() == 0xFF);
    assert(count == 24);
    for (i = 0; i < 8; ++i) {
        assert(delays[3 * i] == 3 && delays[3 * i + 1] == 8 &&
               delays[3 * i + 2] == 59);
    }
    puts("DS18B20 driver delay-call trace passed; physical timing unverified");
    return 0;
}
