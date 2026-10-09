#ifndef __PERIPHERAL_POLICY_H__
#define __PERIPHERAL_POLICY_H__

unsigned char PeripheralPolicy_IsValidBcd(unsigned char value, unsigned char max_decimal);
unsigned char PeripheralPolicy_KeyValue(unsigned char scan_due, unsigned char key_value,
                                        unsigned char *led_bit);
unsigned char PeripheralPolicy_Ds18b20Ready(unsigned int elapsed_ms);
unsigned char PeripheralPolicy_RingNext(unsigned char index, unsigned char capacity);
/* Decode a complete nine-byte DS18B20 scratchpad only after CRC passes.
 * On failure the caller's last valid output is preserved. */
unsigned char PeripheralPolicy_Ds18b20Decode(const unsigned char *scratchpad, float *temperature_c);

#define PCF8591_OK 0U
#define PCF8591_INVALID_ARGUMENT 1U
#define PCF8591_BUS_BUSY 2U
#define PCF8591_NACK 3U
#define PCF8591_BUS_FAULT 4U
#define PCF8591_STOP_FAILED 5U

struct PeripheralPolicy_I2cBus {
    unsigned char (*start)(void);
    unsigned char (*write)(unsigned char value);
    unsigned char (*read)(unsigned char *value, unsigned char send_ack);
    unsigned char (*stop)(void);
};

/* Single-ended channels 0..3, no auto-increment. The first read byte is the
 * previous conversion and must be ACKed/discarded; the second is NACKed. */
unsigned char PeripheralPolicy_Pcf8591Read(const struct PeripheralPolicy_I2cBus *bus,
                                            unsigned char channel, unsigned char *value);

#endif
