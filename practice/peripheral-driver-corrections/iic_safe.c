#include "iic_safe.h"
#include "peripheral_policy.h"

#ifdef HOST_TEST
extern volatile uchar SAFE_SDA;
extern volatile uchar SAFE_SCL;
#else
sbit SAFE_SDA = P2^1;
sbit SAFE_SCL = P2^0;
#endif

#define I2C_DELAY_TIME 5
#define I2C_CLOCK_WAIT_LIMIT 16U

static bit i2c_fault;

static void I2C_SafeDelay(uchar count)
{
    do {
        _nop_();
    } while(count--);
}

static bit I2C_SafeClockHigh(void)
{
    uchar attempt;
    SAFE_SCL = 1;
    for(attempt = 0U; attempt < I2C_CLOCK_WAIT_LIMIT; ++attempt) {
        if(SAFE_SCL) {
            return 1;
        }
        _nop_();
    }
    i2c_fault = 1;
    return 0;
}

static bit I2C_SafeStart(void)
{
    SAFE_SDA = 1;
    if(!I2C_SafeClockHigh()) {
        return 0;
    }
    I2C_SafeDelay(I2C_DELAY_TIME);
    if(!SAFE_SDA) {
        i2c_fault = 1;
        return 0;
    }
    SAFE_SDA = 0;
    I2C_SafeDelay(I2C_DELAY_TIME);
    SAFE_SCL = 0;
    return 1;
}

static bit I2C_SafeStop(void)
{
    SAFE_SDA = 0;
    if(!I2C_SafeClockHigh()) {
        SAFE_SDA = 1;
        return 0;
    }
    I2C_SafeDelay(I2C_DELAY_TIME);
    SAFE_SDA = 1;
    I2C_SafeDelay(I2C_DELAY_TIME);
    if(!SAFE_SDA) {
        i2c_fault = 1;
        return 0;
    }
    return 1;
}

static bit I2C_SafeReadAck(void)
{
    bit nack;
    SAFE_SDA = 1;
    if(!I2C_SafeClockHigh()) {
        return 0;
    }
    I2C_SafeDelay(I2C_DELAY_TIME);
    nack = SAFE_SDA;
    SAFE_SCL = 0;
    I2C_SafeDelay(I2C_DELAY_TIME);
    return nack == 0;
}

static bit I2C_SafeSendAck(bit ack)
{
    SAFE_SDA = ack ? 0 : 1;
    if(!I2C_SafeClockHigh()) {
        SAFE_SDA = 1;
        return 0;
    }
    I2C_SafeDelay(I2C_DELAY_TIME);
    SAFE_SCL = 0;
    SAFE_SDA = 1;
    return 1;
}

static bit I2C_SafeSendByte(uchar value)
{
    uchar i;
    for(i = 0; i < 8; ++i) {
        SAFE_SCL = 0;
        SAFE_SDA = (value & 0x80) ? 1 : 0;
        I2C_SafeDelay(I2C_DELAY_TIME);
        if(!I2C_SafeClockHigh()) {
            SAFE_SDA = 1;
            return 0;
        }
        I2C_SafeDelay(I2C_DELAY_TIME);
        value <<= 1;
    }
    SAFE_SCL = 0;
    return I2C_SafeReadAck();
}

static bit I2C_SafeReadChecked(uchar *value, bit send_ack)
{
    uchar received = 0;
    uchar i;
    SAFE_SDA = 1;
    for(i = 0; i < 8; ++i) {
        if(!I2C_SafeClockHigh()) {
            return 0;
        }
        I2C_SafeDelay(I2C_DELAY_TIME);
        received <<= 1;
        if(SAFE_SDA) {
            received |= 0x01;
        }
        SAFE_SCL = 0;
        I2C_SafeDelay(I2C_DELAY_TIME);
    }
    if(!I2C_SafeSendAck(send_ack)) {
        return 0;
    }
    *value = received;
    return 1;
}

static unsigned char I2C_BusStart(void) { return I2C_SafeStart() ? 1U : 0U; }
static unsigned char I2C_BusWrite(unsigned char value) { return I2C_SafeSendByte(value) ? 1U : 0U; }
static unsigned char I2C_BusRead(unsigned char *value, unsigned char ack)
{
    return I2C_SafeReadChecked(value, ack != 0U) ? 1U : 0U;
}
static unsigned char I2C_BusStop(void) { return I2C_SafeStop() ? 1U : 0U; }

uchar PCF8591_ReadAdcSafe(uchar channel, uchar *value)
{
    static const struct PeripheralPolicy_I2cBus bus = {
        I2C_BusStart, I2C_BusWrite, I2C_BusRead, I2C_BusStop
    };
    uchar result;
    i2c_fault = 0;
    result = PeripheralPolicy_Pcf8591Read(&bus, channel, value);
    if(i2c_fault && result == PCF8591_NACK) {
        return PCF8591_BUS_FAULT;
    }
    return result;
}
