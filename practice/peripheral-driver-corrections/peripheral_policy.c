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

unsigned char PeripheralPolicy_Ds18b20Decode(const unsigned char *scratchpad, float *temperature_c)
{
    unsigned char crc = 0U;
    unsigned char index;
    unsigned char bit_index;
    unsigned char byte_value;
    unsigned int raw;
    long signed_raw;
    if (scratchpad == 0 || temperature_c == 0) {
        return 0U;
    }
    for (index = 0U; index < 8U; ++index) {
        byte_value = scratchpad[index];
        for (bit_index = 0U; bit_index < 8U; ++bit_index) {
            unsigned char mix = (unsigned char)((crc ^ byte_value) & 1U);
            crc >>= 1;
            if (mix) {
                crc ^= 0x8cU;
            }
            byte_value >>= 1;
        }
    }
    if (crc != scratchpad[8]) {
        return 0U;
    }
    raw = (unsigned int)(((unsigned int)scratchpad[1] << 8) | scratchpad[0]);
    signed_raw = (raw & 0x8000U) ? (long)raw - 65536L : (long)raw;
    *temperature_c = (float)signed_raw / 16.0f;
    return 1U;
}

unsigned char PeripheralPolicy_Pcf8591Read(const struct PeripheralPolicy_I2cBus *bus,
                                            unsigned char channel, unsigned char *value)
{
    unsigned char previous = 0U;
    unsigned char current = 0U;
    unsigned char result = PCF8591_OK;
    if (bus == 0 || value == 0 || channel > 3U || bus->start == 0 ||
        bus->write == 0 || bus->read == 0 || bus->stop == 0) {
        return PCF8591_INVALID_ARGUMENT;
    }
    if (!bus->start()) {
        return PCF8591_BUS_BUSY;
    }
    if (!bus->write(0x90U) || !bus->write(channel)) {
        result = PCF8591_NACK;
    } else if (!bus->start()) {
        result = PCF8591_BUS_FAULT;
    } else if (!bus->write(0x91U)) {
        result = PCF8591_NACK;
    } else if (!bus->read(&previous, 1U) || !bus->read(&current, 0U)) {
        result = PCF8591_BUS_FAULT;
    }
    if (!bus->stop()) {
        return PCF8591_STOP_FAILED;
    }
    if (result == PCF8591_OK) {
        *value = current;
    }
    return result;
}
