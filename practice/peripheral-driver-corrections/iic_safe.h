#ifndef __IIC_SAFE_H__
#define __IIC_SAFE_H__

#include <library.h>

bit I2C_SafeSendByte(uchar value);
uchar I2C_SafeReceiveByte(bit send_ack);
bit PCF8591_ReadAdcSafe(uchar channel, uchar *value);

#endif
