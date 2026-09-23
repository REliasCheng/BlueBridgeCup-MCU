#include "uart_safe.h"

#define UART_RX_CAPACITY 32

static xdata volatile uchar rx_buffer[UART_RX_CAPACITY];
static volatile uchar rx_head;
static volatile uchar rx_tail;
static volatile bit rx_overflow;
static volatile bit tx_done;

void UartSafe_Init(void)
{
    SCON = 0x50;
    AUXR |= 0x40;
    AUXR &= 0xFE;
    TMOD &= 0x0F;
    TL1 = 0xC7;
    TH1 = 0xFE;
    ET1 = 0;
    TR1 = 1;
    rx_head = 0;
    rx_tail = 0;
    rx_overflow = 0;
    tx_done = 0;
    ES = 1;
    EA = 1;
}

void UartSafe_SendByte(uchar value)
{
    tx_done = 0;
    SBUF = value;
    while(!tx_done);
}

bit UartSafe_ReadByte(uchar *value)
{
    bit previous_es;
    if(value == 0 || rx_head == rx_tail) {
        return 0;
    }
    previous_es = ES;
    ES = 0;
    *value = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % UART_RX_CAPACITY;
    ES = previous_es;
    return 1;
}

bit UartSafe_TakeOverflow(void)
{
    bit value;
    bit previous_es = ES;
    ES = 0;
    value = rx_overflow;
    rx_overflow = 0;
    ES = previous_es;
    return value;
}

void UartSafe_ISR(void) interrupt 4
{
    if(RI) {
        uchar next;
        uchar value = SBUF;
        RI = 0;
        next = (rx_head + 1) % UART_RX_CAPACITY;
        if(next == rx_tail) {
            rx_overflow = 1;
        } else {
            rx_buffer[rx_head] = value;
            rx_head = next;
        }
    }
    if(TI) {
        TI = 0;
        tx_done = 1;
    }
}
