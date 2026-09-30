#include "peripheral_policy.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    unsigned char led_bit = 0U;

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

    puts("peripheral policy tests passed");
    return 0;
}
