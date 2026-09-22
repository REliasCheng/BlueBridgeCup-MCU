#include <uart.h>

void Uart_Init(void)     //9600bps@12.000MHz
{
    SCON = 0x50;         //8位数据,可变波特率
    AUXR |= 0x40;        //定时器1时钟1T模式
    AUXR &= 0xFE;        //串口1选择定时器1为波特率发生器
    TMOD &= 0x0F;        //设置定时器1模式
    TL1 = 0xC7;          //设置定时初始值
    TH1 = 0xFE;          //设置定时初始值
    ET1 = 0;             //禁止定时器1中断
    TR1 = 1;             //定时器1开始计时

    EA = 1;              //打开总中断
}

char putchar(char dat)
{
    SBUF = dat;
    while(!TI);
    TI = 0;

    return dat;
}

void Delay_5ms(void)     //@12.000MHz
{
    unsigned char i, j;

    i = 59;
    j = 90;
    do
    {
        while(--j);
    }
    while(--i);
}