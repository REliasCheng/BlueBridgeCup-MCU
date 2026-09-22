#include <library.h>
#include <iic.h>
#include <ds1302.h>
#include <uart.h>
#define SHUT_OFF        1
#define RUN_PERIOD_SET  2
#define BRIGHT_LEVEL    3
#define TIME            4

#define OFF 10
#define _   11
uchar Bright_Level = 1;

code uchar Code[] = {
    0xC0,0xF9,0xA4,0xB0,0x99,
    0x92,0x82,0xF8,0x80,0x90,
    0xff,0xbf
};

/* 大数组放 xdata */
xdata uchar Bit[9] = {
    OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF
};

xdata uchar L[9] = {
    0,0,0,0,0,0,0,0,0
};

xdata uint Count[9] = {
    0,0,0,0,0,0,0,0,0
};

xdata uchar Work_Mode[5][8];

/* 小变量继续放默认 DATA */
uchar Buzzer, Relay;
uchar Hour, Minute, Second;

uchar Interface = SHUT_OFF;
uchar RunMode_Dsp = 1;
uchar Set_Bit = 1;
uchar State_Flash = 1;

xdata uint RunPeriod_Dsp[5] = {
    0,400,400,400,400
};

uchar Run_Mode = 1;
uchar Led_Value = 0xff;

xdata uint Run_Period[5] = {
    0,400,400,400,400
};

bit Flag_Led = 0;
bit Flag_LedRun = 0;
bit Flag_Send;
uchar Save_Addr;
uchar End_Addr;
bit Flag_Save;
uchar Read_4Key()
{
    static uchar count_press = 0;
    static uchar flag_press = 0;
    uchar value = 1;
	

    // “| 0x10” 是为了消除 P34 和 signal 相接时产生的干扰
    if((P3 | 0x10) != 0xff)
    {
        if((flag_press == 0) && (++count_press >= 20))
        {
            flag_press = 1;
            count_press = 0;

            if(P33 == 0)      value = 4;   //S4
            else if(P32 == 0) value = 5;   //S5
            else if(P31 == 0) value = 6;   //S6
            else if(P30 == 0) value = 7;   //S7
        }
    }
    else
    {
        flag_press = 0;
        count_press = 0;
    }

    // 非设置状态下，S4要支持“按住显示亮度等级”
    if(Interface != RUN_PERIOD_SET)
    {
        if(P33 == 0)
        {
            value = 104;        //104表示“保持按下S4”
        }
    }

    return value;
}

bit Delay(uchar number,uint period)
{
	while(T2H==0xff);
	if(Count[number]>=period)
	{
		Count[number]=0;
		return 1;
	}
	return 0;
}
void Timer2_Service() interrupt 12
{
 uchar i = 0;
    static uchar count_dsp = 0;
    static uchar count_pwm = 0;

/*********************** LED软件PWM输出 ***********************/
    if(++count_pwm > 4)
    {
        count_pwm = 1;
    }

    if(count_pwm <= Bright_Level)
    {
        Control(0x80, ~(L[8]<<7 | L[7]<<6 | L[6]<<5 | L[5]<<4 |
                        L[4]<<3 | L[3]<<2 | L[2]<<1 | L[1]<<0));
    }
    else
    {
        Control(0x80, 0xff);     //PWM灭灯阶段
    }
	
	Control(0xa0,Buzzer<<6|Relay<<4);
	if(++count_dsp>8)
	{
		count_dsp = 1;
	}
	Control(0xc0,0);
	Control(0xe0,(Bit[count_dsp]>='.')?(Code[Bit[count_dsp]-'.']&0x7f):Code[Bit[count_dsp]]);
	Control(0xc0,1<<(count_dsp-1));
	for(i=0;i<9;++i)
	{
		++Count[i];
	}
}
void System_Init()
{
	uchar i=0;
	
	Control(0x80,0xff);
	Control(0xa0,0);

	for(i=0;i<=7;++i)
	{
		Work_Mode[1][i]=0x01<<i;
		Work_Mode[2][i]=0x80>>i;
	}
	for(i=0;i<=3;++i)
	{
		Work_Mode[3][i]=Work_Mode[1][i]|Work_Mode[2][i];
		Work_Mode[4][i]=Work_Mode[1][3-i]|Work_Mode[2][3-i];		
	}
	/***************** 读取LED各工作模式的流转间隔 *****************/
	for(i=1; i<=4; ++i)
	{
			Run_Period[i] = Read_Eeprom(i) * 10;

    if((Run_Period[i] < 400) || (Run_Period[i] > 1200) ||  (Run_Period[i] % 100 != 0)) 
			{
					Run_Period[i] = 800;     //读到脏数据时给默认值
			}

    RunPeriod_Dsp[i] = Run_Period[i];
}
	/***************** 读取LED各工作模式的流转间隔 *****************/
	Set_Time(0x23, 0x59, 0x55);
Uart_Init();
Delay_5ms();
		Timer2_Init();
}
void Display_Task()
{
    uint period = RunPeriod_Dsp[RunMode_Dsp];

    if(Interface == RUN_PERIOD_SET)
    {
        if((Set_Bit == 1) && (State_Flash == 0))
        {
            memset(&Bit[1], OFF, 3);
        }
        else
        {
            Bit[1] = _;
            Bit[2] = RunMode_Dsp;
            Bit[3] = _;
        }

        Bit[4] = OFF;

        if((Set_Bit == 2) && (State_Flash == 0))
        {
            memset(&Bit[5], OFF, 4);
        }
        else
        {
            Bit[5] = (period >= 1000) ? (period / 1000 % 10) : OFF;
            Bit[6] = period / 100 % 10;
            Bit[7] = period / 10 % 10;
            Bit[8] = period % 10;
        }
    }
		else if(Interface == TIME)
{
    Bit[1] = Hour / 10 % 10;
    Bit[2] = Hour % 10;
    Bit[3] = _;

    Bit[4] = Minute / 10 % 10;
    Bit[5] = Minute % 10;
    Bit[6] = _;

    Bit[7] = Second / 10 % 10;
    Bit[8] = Second % 10;
}
    else if(Interface == BRIGHT_LEVEL)
    {
        memset(&Bit[1], OFF, 6);
        Bit[7] = _;
        Bit[8] = Bright_Level;
    }
    else if(Interface == SHUT_OFF)
    {
        memset(Bit, OFF, 9);
    }
	
}
void Data_Task()
{
    uchar adc_value = 0;
		uchar save_value = 0;
	/*********************** EEPROM保存任务 ***********************/
    if(T2H < 0xd9)
    {
        if(Flag_Save == 1)
        {
            if(Delay(4, 5))      //每5ms保存1个地址
            {
                save_value = Run_Period[Save_Addr] / 10;
                Write_Eeprom(Save_Addr, save_value);

                if(++Save_Addr > End_Addr)
                {
                    Flag_Save = 0;
                }
            }
        }
    }
/*********************** EEPROM保存任务 ***********************/

    if(T2H < 0xd9)
    {
        if(Delay(3, 150))       //150ms读取一次RB2
        {
            adc_value = Read_Adc(0x43);

            Bright_Level = adc_value / 64 + 1;

            if(Bright_Level > 4)
            {
                Bright_Level = 4;
            }
        }
    }
		/*********************** DS1302时间读取任务 ***********************/
if(T2H < 0xd9)
{
    if(Delay(5, 150))
    {
        uchar time[3];

        time[2] = Read_Ds1302_Byte(0x85);   //时
        time[1] = Read_Ds1302_Byte(0x83);   //分
        time[0] = Read_Ds1302_Byte(0x81);   //秒

        Hour   = time[2] / 16 * 10 + time[2] % 16;
        Minute = time[1] / 16 * 10 + time[1] % 16;
        Second = time[0] / 16 * 10 + time[0] % 16;
    }
}
/*********************** DS1302时间读取任务 ***********************/
/*********************** 串口发送任务 ***********************/
if(Flag_Send == 1)
{
    Flag_Send = 0;

    printf("\r\n/********* LanQiao Report *********/\r\n");
    printf("Time: %bu:%bu:%bu\r\n", Hour, Minute, Second);

    printf("Mode1 Period: %u ms\r\n", Run_Period[1]);
    printf("Mode2 Period: %u ms\r\n", Run_Period[2]);
    printf("Mode3 Period: %u ms\r\n", Run_Period[3]);
    printf("Mode4 Period: %u ms\r\n", Run_Period[4]);

    printf("Bright Level: %bu\r\n", Bright_Level);
}
}
void Key_Task()
{
    uchar key_value = 0;

    if(Delay(1, 1))
    {
        key_value = Read_4Key();
    }

    if(key_value == 4)          // S4：设置状态下减
{
    if(Interface == RUN_PERIOD_SET)
    {
        if(Set_Bit == 1)    // 减运行模式编号
        {
            if(RunMode_Dsp > 1)
            {
                --RunMode_Dsp;
                Relay = 0;  // 有效操作，关闭继电器
            }
            else
            {
                Relay = 1;  // 已经是最小值，再减无效
            }
        }
        else if(Set_Bit == 2)   // 减流转间隔
        {
            if(RunPeriod_Dsp[RunMode_Dsp] > 400)
            {
                RunPeriod_Dsp[RunMode_Dsp] -= 100;
                Relay = 0;      // 有效操作，关闭继电器
            }
            else
            {
                Relay = 1;      // 已经是400，再减无效
            }
        }
    }
}
else if(key_value == 5)     // S5
{
    if(Interface == RUN_PERIOD_SET)
    {
        if(Set_Bit == 1)    // 加运行模式编号
        {
            if(RunMode_Dsp < 4)
            {
                ++RunMode_Dsp;
                Relay = 0;
            }
            else
            {
                Relay = 1;
            }
        }
        else if(Set_Bit == 2)   // 加流转间隔
        {
            if(RunPeriod_Dsp[RunMode_Dsp] < 1200)
            {
                RunPeriod_Dsp[RunMode_Dsp] += 100;
                Relay = 0;
            }
            else
            {
                Relay = 1;
            }
        }
    }
    else                        // 非设置界面下，S5串口上报
    {
        Relay = 0;
        Flag_Send = 1;
    }
}
    else if(key_value == 6)     // S6：设置键
    {
			Relay = 0;
        if(Interface != RUN_PERIOD_SET)
        {
            Interface = RUN_PERIOD_SET;
            Set_Bit = 1;
            State_Flash = 1;
            Count[0] = 0;
        }
        else if(Interface == RUN_PERIOD_SET)
        {
            if(Set_Bit == 1)
            {
                Set_Bit = 2;
                State_Flash = 1;
                Count[0] = 0;
            }
            else if(Set_Bit == 2)
            {
                uchar i = 0;

                for(i=1; i<=4; ++i)
                {
                    Run_Period[i] = RunPeriod_Dsp[i];
                }

                Flag_Save = 1;
                Save_Addr = 1;
                End_Addr = 4;

                Interface = SHUT_OFF;
                Set_Bit = 1;
                State_Flash = 1;
                Count[0] = 0;
            }
        }
    }
    else if(key_value == 7)     // S7：启动/停止
    {
			Relay = 0;
        Flag_Led = 1;
        Flag_LedRun ^= 1;
    }
    else if(key_value == 104)   // 非设置状态下，按住S4显示亮度等级
    {
			Relay = 0;
        Interface = BRIGHT_LEVEL;
    }
    else if(key_value == 1)     // 松开按键
    {
        if(Interface == BRIGHT_LEVEL)
        {
            Interface = SHUT_OFF;
        }
    }
			if((key_value >= 4) || (key_value == 104))
{
    Count[6] = 0;      //有操作，清空无操作计时
}
}
void Logic_Task()
{
	static uchar state_bit=0;
	static uint count_period=0;
	
	uchar i=0;
	uchar run_times=0;
	/******************** 10秒无操作跳转时间界面 ********************/
if(Interface != TIME)
{
    if(Delay(6, 10000))
    {
        Interface = TIME;
    }
}
else
{
    Count[6] = 0;
}
/******************** 10秒无操作跳转时间界面 ********************/
	if(Interface==RUN_PERIOD_SET)
	{
		if(Delay(0,800))
		{
			State_Flash^=1;
		}
	}
	if(Flag_LedRun==1)
	{
		if(Delay(2,10))
		{
			++count_period;
		}
		if(count_period>=(Run_Period[Run_Mode]/10))
		{
			count_period=0;
			run_times=(Run_Mode>2)?4:8;
			if(++state_bit>=run_times)
			{
				state_bit=0;
				if(++Run_Mode>4)
				{
					Run_Mode=1;
				}
			}
		}
	}
	else
	{
		Count[2]=0;
		count_period=0;
	}
	if(Flag_Led==1)
	{
		if(Flag_LedRun==1)
		{
			Led_Value=Work_Mode[Run_Mode][state_bit];
		}
	}
	else
	{
		Led_Value=0x00;
	}
	for(i=1;i<=8;++i)
	{
		L[i]=(Led_Value>>(i-1))&0x01;
	}
	
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











