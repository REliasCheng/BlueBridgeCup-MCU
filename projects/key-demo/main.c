#include <library.h>
uchar Count_Key;
uchar Led_Bit = 9;
void Timer2_Service() interrupt 12
{
	Control(0x80,~(1<<(Led_Bit-1)));
	++Count_Key;	
}
void System_Task()
{
	Control(0x80,0xff);
	Control(0xa0,0);
	Timer2_Init();
}
void Key_Task()
{
	uchar key_value;
	if(Count_Key>=1)
	{
		Count_Key=0;
		key_value = Read_4Key();
	}
	if(key_value==4)
		{
			Led_Bit=4;
		}
		else if(key_value==5)
		{
			Led_Bit=5;
		}
		else if(key_value==6)
		{
			Led_Bit=6;
		}
		else if(key_value==7)
		{
			Led_Bit=7;
		}
} 

int main()
{
	System_Task();
	while(1)
	{
		Key_Task();
	}
		
}














