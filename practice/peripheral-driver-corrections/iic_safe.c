#include "iic_safe.h"

sbit SAFE_SDA = P2^1;
sbit SAFE_SCL = P2^0;

#define I2C_DELAY_TIME 5

static void I2C_SafeDelay(uchar count)
{
    do {
        _nop_();
    } while(count--);
}

static void I2C_SafeStart(void)
{
    SAFE_SDA = 1;
    SAFE_SCL = 1;
    I2C_SafeDelay(I2C_DELAY_TIME);
    SAFE_SDA = 0;
    I2C_SafeDelay(I2C_DELAY_TIME);
    SAFE_SCL = 0;
}

static void I2C_SafeStop(void)
{
    SAFE_SDA = 0;
    SAFE_SCL = 1;
    I2C_SafeDelay(I2C_DELAY_TIME);
    SAFE_SDA = 1;
    I2C_SafeDelay(I2C_DELAY_TIME);
}

static bit I2C_SafeReadAck(void)
{
    bit nack;
    SAFE_SDA = 1;
    SAFE_SCL = 1;
    I2C_SafeDelay(I2C_DELAY_TIME);
    nack = SAFE_SDA;
    SAFE_SCL = 0;
    I2C_SafeDelay(I2C_DELAY_TIME);
    return nack == 0;
}

static void I2C_SafeSendAck(bit ack)
{
    SAFE_SDA = ack ? 0 : 1;
    SAFE_SCL = 1;
    I2C_SafeDelay(I2C_DELAY_TIME);
    SAFE_SCL = 0;
    SAFE_SDA = 1;
}

bit I2C_SafeSendByte(uchar value)
{
    uchar i;
    for(i = 0; i < 8; ++i) {
        SAFE_SCL = 0;
        SAFE_SDA = (value & 0x80) ? 1 : 0;
        I2C_SafeDelay(I2C_DELAY_TIME);
        SAFE_SCL = 1;
        I2C_SafeDelay(I2C_DELAY_TIME);
        value <<= 1;
    }
    SAFE_SCL = 0;
    return I2C_SafeReadAck();
}

uchar I2C_SafeReceiveByte(bit send_ack)
{
    uchar value = 0;
    uchar i;
    SAFE_SDA = 1;
    for(i = 0; i < 8; ++i) {
        SAFE_SCL = 1;
        I2C_SafeDelay(I2C_DELAY_TIME);
        value <<= 1;
        if(SAFE_SDA) {
            value |= 0x01;
        }
        SAFE_SCL = 0;
        I2C_SafeDelay(I2C_DELAY_TIME);
    }
    I2C_SafeSendAck(send_ack);
    return value;
}

bit PCF8591_ReadAdcSafe(uchar channel, uchar *value)
{
    if(value == 0) {
        return 0;
    }
    I2C_SafeStart();
    if(!I2C_SafeSendByte(0x90) || !I2C_SafeSendByte(channel)) {
        I2C_SafeStop();
        return 0;
    }
    I2C_SafeStart();
    if(!I2C_SafeSendByte(0x91)) {
        I2C_SafeStop();
        return 0;
    }
    *value = I2C_SafeReceiveByte(0);
    I2C_SafeStop();
    return 1;
}
