#ifndef __UART_SAFE_H__
#define __UART_SAFE_H__

#include <library.h>

void UartSafe_Init(void);
void UartSafe_SendByte(uchar value);
bit UartSafe_ReadByte(uchar *value);
bit UartSafe_TakeOverflow(void);

#endif
