#include <library.h>

void Timer2_Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;			//定时器时钟1T模式
	T2L = 0x20;				//设置定时初始值
	T2H = 0xD1;				//设置定时初始值
	AUXR |= 0x10;			//定时器2开始计时
	IE2 = 0X04;
	EA=1;
}
uchar Read_4Key()
{
	static uchar count_press = 0;
	static uchar flag_press = 0;
	uchar value =1;
	if((P3|0x10)!=0xff)
	{
		if((flag_press==0)&&(++count_press>=20))
		{
			count_press = 0;
			flag_press = 1;
			if(P33==0) value=4;
			else if(P32==0) value=5;
			else if(P31==0) value=6;
			else if(P30==0) value=7;
		}
	}
	else
	{
		count_press = 0;
		flag_press = 0;
	}
	return value;
}



