/* Executes the host-compiled driver with an undriven SDA line. No bus timing is emulated. */
#include "iic_safe.h"
#include "peripheral_policy.h"

#include <assert.h>
#include <stdio.h>

volatile uchar SAFE_SDA = 1U;
volatile uchar SAFE_SCL = 1U;

int main(void)
{
    uchar value = 0x5aU;
    assert(PCF8591_ReadAdcSafe(4U, &value) == PCF8591_INVALID_ARGUMENT);
    assert(value == 0x5aU);
    assert(PCF8591_ReadAdcSafe(0U, 0) == PCF8591_INVALID_ARGUMENT);
    assert(PCF8591_ReadAdcSafe(0U, &value) == PCF8591_NACK);
    assert(value == 0x5aU);
    assert(SAFE_SDA == 1U);
    puts("IIC driver host NACK/argument path passed; CT107D timing unverified");
    return 0;
}
