#include <library.h>
void Timer2_Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;			//定时器时钟1T模式
	T2L = 0x20;				//设置定时初始值
	T2H = 0xD1;				//设置定时初始值
	AUXR |= 0x10;			//定时器2开始计时
	IE2|=0x04;
	EA=1;
}
uchar Read_16Key()
{
	static uchar cnt=0;
	static uchar flag=0;
	uchar value =0;
	uchar x=0;
	uchar y=0;
	P3=0x0f;
	P4=0x00;
	if(P30==0) x=0;
	else if(P31==0) x=1;
	else if(P32==0) x=2;
	else if(P33==0) x=3;
	P3=0xf0;
	P4=0xff;
	if(P44==0) y=1;
	else if(P42==0) y=2;
	else if(P35==0) y=3;
	else if(P34==0) y=4;
	if(y!=0)
	{
		if((flag==0)&&(++cnt>=20))
		{
			flag=1;
			cnt=0;
			value=x+y*4;
		}
	}
	else
	{
		flag=0;
		cnt=0;
	}
	return value;
}
