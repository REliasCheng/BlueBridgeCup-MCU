#include "peripheral_policy.h"

unsigned char PeripheralPolicy_IsValidBcd(unsigned char value, unsigned char max_decimal)
{
    unsigned char high = (unsigned char)((value >> 4) & 0x0fU);
    unsigned char low = (unsigned char)(value & 0x0fU);
    return (unsigned char)(low <= 9U && high <= 9U &&
                           (unsigned char)(high * 10U + low) <= max_decimal);
}

unsigned char PeripheralPolicy_KeyValue(unsigned char scan_due, unsigned char key_value,
                                        unsigned char *led_bit)
{
    if (scan_due == 0U || led_bit == 0 || key_value < 4U || key_value > 7U) {
        return 0U;
    }
    *led_bit = key_value;
    return 1U;
}

unsigned char PeripheralPolicy_Ds18b20Ready(unsigned int elapsed_ms)
{
    return (unsigned char)(elapsed_ms >= 750U);
}

unsigned char PeripheralPolicy_RingNext(unsigned char index, unsigned char capacity)
{
    if (capacity == 0U) {
        return 0U;
    }
    return (unsigned char)((index + 1U) % capacity);
}
