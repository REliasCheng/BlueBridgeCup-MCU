#include <library.h>

void Timer2_Init(void)	 //1毫秒@12.000MHz
{
	AUXR |= 0x04;		 //定时器2时钟1T模式
	T2L = 0x20;
	T2H = 0xD1;
	AUXR |= 0x10;		 //定时器2开始计时

    IE2 |= 0x04;         //打开定时器2中断
    EA = 1;              //打开总中断
}