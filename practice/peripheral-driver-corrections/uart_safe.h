#ifndef __UART_SAFE_H__
#define __UART_SAFE_H__

#include <library.h>

void UartSafe_Init(void);
#define UART_SAFE_TX_OK 0U
#define UART_SAFE_TX_BUSY 1U
#define UART_SAFE_TX_TIMEOUT 2U
#define UART_SAFE_TX_INTERRUPTS_DISABLED 3U

/* poll_budget is a bounded iteration count, not a calibrated time in ms.
 * TIMEOUT means completion is unknown: do not blindly retransmit. A later TI
 * interrupt releases the busy state; otherwise reinitialize after diagnosis. */
uchar UartSafe_SendByte(uchar value, uint poll_budget);
bit UartSafe_ReadByte(uchar *value);
bit UartSafe_TakeOverflow(void);

#endif
