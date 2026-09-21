#include <library.h>
uchar Led_Bit=1;
bit Flag_Direct =1;
uchar Dsp_Bit=1;
uint Count_Led=0;
uint Count_Dsp=0;
uint Count_Relay=0;
uchar Flag_Relay =1;
void Timer2_Service() interrupt 12
{
	P0 = ~(1<<(Led_Bit - 1));
	P2 = 0x80;
	P2 = 0;
	if(Flag_Relay==1)
	{
		P0=1<<4;
	}
	else
	{
		P0=0;
	}
	P2 = 0xa0;
	P2 = 0x00;
	
	P0 = 0;
	P2 = 0xc0;
	P2 = 0;
	
	P0 = 0x82;
	P2 = 0xe0;
	P2 = 0;
	
	P0 = 1<<(Dsp_Bit - 1);
	P2 = 0xc0;
	P2 = 0;
	++Count_Led;
	++Count_Dsp;
	++Count_Relay;
	
}
void System_Init()
{
	P0=0xff;
	P2=0x80;
	P2=0;
	P0=0;
	P2=0xa0;
	P2=0;
	Timer2_init();
	
}
void Logic_Task()
{
	if(Count_Led>200)
	{
		Count_Led = 0;
		if(Flag_Direct==1)
		{
			++Led_Bit;
		}
		else if(Flag_Direct==0)
		{
			--Led_Bit;
		}
		if(Led_Bit>8)
		{
			Led_Bit = 7;
			Flag_Direct=0;
		}
		else if(Led_Bit<1)
		{
			Led_Bit=2;
			Flag_Direct = 1;
		}
	}
	if(Count_Dsp >=300)
	{
		Count_Dsp =0;
		if(++Dsp_Bit>8)
		{
			Dsp_Bit=1;
		}
	}
	if(Count_Relay >=5000)
	{
		Count_Relay =0;
		Flag_Relay ^= 1;
	}
}
int main()
{
	System_Init();
	while(1)
	{
		Logic_Task();
	}
}











