#include "peripheral_policy.h"

#include <assert.h>
#include <stdio.h>

static unsigned char starts, writes, reads, stops;
static unsigned char fail_start, fail_write, fail_read, fail_stop;
static unsigned char ack_pattern[2];
static unsigned char write_values[3];

static unsigned char start(void)
{
    ++starts;
    return starts != fail_start;
}

static unsigned char write(unsigned char value)
{
    write_values[writes] = value;
    ++writes;
    return writes != fail_write;
}

static unsigned char read_byte(unsigned char *value, unsigned char send_ack)
{
    ack_pattern[reads] = send_ack;
    *value = reads == 0U ? 0x77U : 0x33U;
    ++reads;
    return reads != fail_read;
}

static unsigned char stop(void)
{
    ++stops;
    return !fail_stop;
}

static void reset(void)
{
    starts = writes = reads = stops = 0U;
    fail_start = fail_write = fail_read = fail_stop = 0U;
}

int main(void)
{
    static const struct PeripheralPolicy_I2cBus bus = {start, write, read_byte, stop};
    unsigned char value = 0xAAU;

    reset();
    assert(PeripheralPolicy_Pcf8591Read(&bus, 2U, &value) == PCF8591_OK);
    assert(value == 0x33U && reads == 2U && starts == 2U && stops == 1U);
    assert(write_values[0] == 0x90U && write_values[1] == 2U && write_values[2] == 0x91U);
    assert(ack_pattern[0] == 1U && ack_pattern[1] == 0U);

    reset();
    assert(PeripheralPolicy_Pcf8591Read(&bus, 4U, &value) == PCF8591_INVALID_ARGUMENT);
    assert(starts == 0U && value == 0x33U);
    reset();
    fail_start = 1U;
    assert(PeripheralPolicy_Pcf8591Read(&bus, 0U, &value) == PCF8591_BUS_BUSY);
    assert(stops == 0U && value == 0x33U);
    reset();
    fail_write = 2U;
    assert(PeripheralPolicy_Pcf8591Read(&bus, 1U, &value) == PCF8591_NACK);
    assert(stops == 1U && reads == 0U && value == 0x33U);
    reset();
    fail_start = 2U;
    assert(PeripheralPolicy_Pcf8591Read(&bus, 1U, &value) == PCF8591_BUS_FAULT);
    assert(stops == 1U && value == 0x33U);
    reset();
    fail_read = 2U;
    assert(PeripheralPolicy_Pcf8591Read(&bus, 3U, &value) == PCF8591_BUS_FAULT);
    assert(stops == 1U && value == 0x33U);
    reset();
    fail_stop = 1U;
    assert(PeripheralPolicy_Pcf8591Read(&bus, 0U, &value) == PCF8591_STOP_FAILED);
    assert(value == 0x33U);
    reset();
    assert(PeripheralPolicy_Pcf8591Read(&bus, 0U, &value) == PCF8591_OK);
    assert(value == 0x33U);

    puts("pcf8591 host transaction tests passed");
    return 0;
}
