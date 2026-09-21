#include <library.h>
#include <onewire.h>


uchar Code[]={0xC0 ,0xF9, 0xA4 ,0xB0 ,0x99 ,0x92, 0x82, 0xF8, 0x80 ,0x90 ,0xff};
uchar Bit[9]={OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF};
float Temp;
uint Count_Temp;
void Timer2_Service() interrupt 12
{
	static uchar count_dsp = 0;
	if(++count_dsp>8)
	{
		count_dsp = 1;
	}
	Control(0xc0,0);
	Control(0xe0, (Bit[count_dsp] >= '.') ? (Code[Bit[count_dsp]-'.'] & 0x7f) : Code[Bit[count_dsp]]);
	Control(0xc0,1<<(count_dsp-1));
	++Count_Temp;
}

void System_Init()
{
	uchar i = 0;
	Control(0xa0,0);
	Control(0x80,0xff);
	for(i=0;i<100;++i)
	{
		Temp = Read_Temp();

	}
	Timer2_Init();
}
void Display_Task()
{
	int temp = Temp*100 +0.5;
	Bit[5]=temp/1000%10;
	Bit[6]=temp/100%10+'.';
	Bit[7]=temp/10%10;
	Bit[8]=temp/1%10;

}
void Data_Task()
{
	if(Count_Temp>=750)
	{
		Count_Temp = 0;
		Temp = Read_Temp();
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
