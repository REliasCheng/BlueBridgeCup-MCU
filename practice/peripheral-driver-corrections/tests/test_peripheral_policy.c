#include "peripheral_policy.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    unsigned char led_bit = 0U;
    unsigned char zero[9] = {0x00U, 0x00U, 0x4bU, 0x46U, 0x7fU, 0xffU, 0x0cU, 0x10U, 0xc8U};
    unsigned char negative[9] = {0xf0U, 0xffU, 0x4bU, 0x46U, 0x7fU, 0xffU, 0x0cU, 0x10U, 0xe9U};
    float temperature = 12.5f;

    assert(PeripheralPolicy_IsValidBcd(0x00U, 59U));
    assert(PeripheralPolicy_IsValidBcd(0x59U, 59U));
    assert(!PeripheralPolicy_IsValidBcd(0x6aU, 59U));
    assert(!PeripheralPolicy_IsValidBcd(0x24U, 23U));

    assert(!PeripheralPolicy_KeyValue(0U, 4U, &led_bit));
    assert(!PeripheralPolicy_KeyValue(1U, 3U, &led_bit));
    assert(PeripheralPolicy_KeyValue(1U, 6U, &led_bit));
    assert(led_bit == 6U);

    assert(!PeripheralPolicy_Ds18b20Ready(749U));
    assert(PeripheralPolicy_Ds18b20Ready(750U));
    assert(PeripheralPolicy_RingNext(30U, 32U) == 31U);
    assert(PeripheralPolicy_RingNext(31U, 32U) == 0U);
    assert(PeripheralPolicy_RingNext(5U, 0U) == 0U);

    assert(PeripheralPolicy_Ds18b20Decode(zero, &temperature));
    assert(temperature == 0.0f);
    assert(PeripheralPolicy_Ds18b20Decode(negative, &temperature));
    assert(temperature == -1.0f);
    negative[8] ^= 1U;
    assert(!PeripheralPolicy_Ds18b20Decode(negative, &temperature));
    assert(temperature == -1.0f);
    assert(!PeripheralPolicy_Ds18b20Decode(0, &temperature));
    assert(!PeripheralPolicy_Ds18b20Decode(zero, 0));

    puts("peripheral policy tests passed");
    return 0;
}
