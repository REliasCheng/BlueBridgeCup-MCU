#include "uart_safe.h"

#include <assert.h>
#include <stdio.h>

volatile uchar SCON, AUXR, TMOD, TL1, TH1, ET1, TR1, ES, EA, RI, TI, SBUF;
static unsigned int hooks;
static unsigned int complete_after;
static bit receive_during_poll;

void UartSafe_ISR(void);

void UartSafe_TestPollHook(void)
{
    ++hooks;
    if(receive_during_poll && hooks == 1U) {
        RI = 1U;
        SBUF = 0x5AU;
        UartSafe_ISR();
    }
    if(complete_after != 0U && hooks == complete_after) {
        TI = 1U;
        UartSafe_ISR();
    }
}

int main(void)
{
    uchar value = 0U;
    UartSafe_Init();
    ES = 0U;
    assert(UartSafe_SendByte(0x41U, 3U) == UART_SAFE_TX_INTERRUPTS_DISABLED);
    ES = 1U;

    hooks = 0U;
    complete_after = 2U;
    receive_during_poll = 1U;
    assert(UartSafe_SendByte(0x42U, 4U) == UART_SAFE_TX_OK);
    assert(hooks == 2U);
    assert(UartSafe_ReadByte(&value));
    assert(value == 0x5AU);
    receive_during_poll = 0U;

    hooks = 0U;
    complete_after = 0U;
    assert(UartSafe_SendByte(0x43U, 3U) == UART_SAFE_TX_TIMEOUT);
    assert(hooks == 3U);
    assert(UartSafe_SendByte(0x44U, 3U) == UART_SAFE_TX_BUSY);
    TI = 1U;
    UartSafe_ISR();
    hooks = 0U;
    complete_after = 1U;
    assert(UartSafe_SendByte(0x45U, 3U) == UART_SAFE_TX_OK);

    puts("uart host state tests passed");
    return 0;
}
