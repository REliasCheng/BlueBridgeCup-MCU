#include <library.h>
#include <onewire.h>
#include <iic.h>
#define OFF 10
uchar Code[]= {0xC0, 0xF9 ,0xA4 ,0xB0, 0x99,0x92 ,0x82 ,0xF8 ,0x80 ,0x90,0xff};
uchar Bit[9]= {OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF};
uchar L[9];
float Volt,Temp;
uchar Buzzer,Relay;
uchar Count_Adc;
uint		 Count_Temp;
uchar Count_Key;
bit Flag_Buzzer;
void Timer2_Service() interrupt 12
{
	static uchar count_dsp = 0;
	Control(0x80,~( L[8]<<7 |  L[7]<<6 |  L[6]<<5 |  L[5]<<4 | 
									L[4]<<3 |  L[3]<<2 |  L[2]<<1 |  L[1]<<0  ));
  Control(0xa0, Buzzer<<6 | Relay<<4);
	if(++count_dsp>8)
	{
		count_dsp=1;
	}
	Control(0xc0,0);
	Control(0xe0,(Bit[count_dsp]>='.')?(Code[Bit[count_dsp]-'.']&0x7f):Code[Bit[count_dsp]]);
	Control(0xc0,1<<(count_dsp-1));
	++Count_Adc;
	++Count_Temp;
	++Count_Key;
}

void System_Init()
{
	uchar i = 0;
	Control(0xa0,0);
	Control(0x80,0xff);
	for(i=0;i<5;i++)
	{
		Volt= Read_Adc(0x43)*0.0196;
	}
	for(i=0;i<100;i++)
	{
		Temp = Read_Temp();
	}
	Timer2_Init();
}
void Display_Task()
{
	uint volt = Volt*10+0.5;
	uint temp = Temp*10+0.5;
	Bit[1]=temp/100%10;
	Bit[2]=temp/10%10+'.';
	Bit[3]=temp/1%10;
	Bit[4]=OFF;
	Bit[5]=OFF;	
	Bit[6]=OFF;	
	Bit[7]=volt/10%10+'.';
	Bit[8]=volt/1%10;
}
void Data_Task()
{
	if(T2H<0xd9)
	{
		if(Count_Adc>=150)
		{
			Count_Adc = 0;
			Volt= Read_Adc(0x43)*0.0196;
		}
		if(Count_Temp>=750)
		{
			Count_Temp = 0;
			Temp = Read_Temp();
		}
	}
}
void Key_Task()
{
	uchar key_value =0;
	if(Count_Key>=1)
	{
		Count_Key = 0;
		key_value = Read_4Key();
	}
	if(key_value==4)
	{
		Flag_Buzzer^=1;
	}
}

void Logic_Task()
{
		uchar i;
	int unit=Volt*10+0.5;
	unit = unit/1%10;

	for(i=1;i<9;i++)
	{
		L[i] = 0;
	}
	if(unit >=1 && unit<=8)
	{
		L[unit]=1;
	}
	else if(unit == 9)
	{
		L[1]=1;
		L[8]=1;
	}
	if(Temp>28.8)
	{
		Relay =1;
	}
	else
	{
		Relay = 0;
	}
	Buzzer = Flag_Buzzer;
}

int main()
{
	System_Init();
	while(1)
	{
		Display_Task();
		Data_Task();
		Key_Task();
		Logic_Task();
	}
}




