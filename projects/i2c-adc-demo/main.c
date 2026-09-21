#include <library.h>
#include <iic.h>
uchar Code[]={0xC0 ,0xF9, 0xA4 ,0xB0, 0x99 ,0x92,0x82 ,0xF8, 0x80, 0x90,0xff};
uchar Bit[9] = {OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF};
float Volt;
uint Count_Adc;




void Tiemr2_Service() interrupt 12
{
	static uchar count_dsp = 0;
	if(++count_dsp>8)
	{
		count_dsp = 1;
	}
	Control(0xc0,0);
	Control(0xe0, (Bit[count_dsp]>= '.') ? ((Code[Bit[count_dsp]- '.']) & 0x7f):(Code[Bit[count_dsp]]));
	Control(0xc0,1<<(count_dsp-1));
	++Count_Adc;
	
	
}
void System_Init()
{
	uchar i = 0;
	Control(0xc0,0);
	Control(0x80,0xff);
	for(i=0;i<5;i++)
	{
		Volt=Read_Adc(0x43)*0.0196;
	}
	Timer2_Init();
}
void Display_Task()
{
	uint volt = Volt*100+0.5;
	Bit[6]=volt/100%10 + '.';
	Bit[7]=volt/10%10;
	Bit[8]=volt/1%10;
}
void Data_Task()
{
	if(T2H<0xd9)
	{
		if(Count_Adc>=150)
		{
			Count_Adc = 0;
			Volt = Read_Adc(0x43)*0.0196;
		}
	}
	
}
int main()
{
	System_Init();
	while(1)
	{
		Display_Task();
		Data_Task();
	}
}



