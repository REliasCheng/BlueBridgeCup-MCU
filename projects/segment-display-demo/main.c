#include <library.h>
uchar Code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
uchar Bit[9]={OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF};
long love =12345;


void Timer2_Service() interrupt 12
{
	static  uchar count_dsp = 1;
	if(++count_dsp>8)
	{
		count_dsp = 1;
	}
	Control(0xc0,0);
	Control(0xe0,Code[Bit[count_dsp]]);
	Control(0xc0,1<<(count_dsp-1));
}


void System_Task()
{
	Control(0xa0,0);
	Control(0x80,0xff);
	Timer2_Init();
}

void Display_Task()
{
	Bit[1]= OFF ;
	Bit[2]= OFF ;
	Bit[3]= OFF ;
	Bit[4]= love/10000%10 ;
	Bit[5]= love/1000%10;
	Bit[6]= love/100%10;
	Bit[7]= love/10%10;
	Bit[8]= love/1%10;
}

int main()
{
	System_Task();
	while(1)
	{
		Display_Task();
	}
}





