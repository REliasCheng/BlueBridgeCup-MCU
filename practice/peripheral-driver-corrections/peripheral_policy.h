#ifndef __PERIPHERAL_POLICY_H__
#define __PERIPHERAL_POLICY_H__

unsigned char PeripheralPolicy_IsValidBcd(unsigned char value, unsigned char max_decimal);
unsigned char PeripheralPolicy_KeyValue(unsigned char scan_due, unsigned char key_value,
                                        unsigned char *led_bit);
unsigned char PeripheralPolicy_Ds18b20Ready(unsigned int elapsed_ms);
unsigned char PeripheralPolicy_RingNext(unsigned char index, unsigned char capacity);

#endif
