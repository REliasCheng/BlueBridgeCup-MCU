#include <library.h>
#include <onewire.h>
#include <ds1302.h>
#include <uart.h>

#define RX_BUF_LEN  12

xdata uchar R[RX_BUF_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff,
                             0xff,0xff,0xff,0xff,0xff,0xff};

bit   Flag_SendTime;
bit   Flag_SendTemp;

uint  Count_Temp;
uchar Count_Time;
uchar Count_Recv;
uchar Recv_Number;

float Temp;
uint  Year = 2026;
uchar Hour, Minute, Second;

void Clear_Recv()
{
    uchar i;

    Recv_Number = 0;
    Count_Recv = 0;

    for(i=0; i<RX_BUF_LEN; ++i)
    {
        R[i] = 0xff;
    }
}

// 串口中断：接收数据
void Uart() interrupt 4
{
    if(RI == 1)
    {
        RI = 0;
        Count_Recv = 0;

        if(Recv_Number < 10)
        {
            R[++Recv_Number] = SBUF;
        }
    }

    if(TI == 1)
    {
        TI = 0;
    }
}

// 定时器2中断：1ms
void Timer2_Service() interrupt 12
{
    ++Count_Recv;
    ++Count_Temp;
    ++Count_Time;
}

void System_Init()
{
    uchar i;

    Control(0x80, 0xff);       //关闭LED
    Control(0xa0, 0x00);       //关闭蜂鸣器和继电器

    for(i=0; i<100; ++i)
    {
        Temp = Read_Temp();
    }

    Set_Time(0x23, 0x59, 0x55);

    Uart_Init();
    Delay_5ms();

    Timer2_Init();

    printf("\r\nUART Ready!\r\n");
}

void Data_Task()
{
    uchar time[3];

    if(T2H < 0xd9)
    {
        if(Count_Time >= 150)
        {
            Count_Time = 0;

            time[2] = Read_Ds1302_Byte(0x85);
            time[1] = Read_Ds1302_Byte(0x83);
            time[0] = Read_Ds1302_Byte(0x81);

            Hour   = time[2] / 16 * 10 + time[2] % 16;
            Minute = time[1] / 16 * 10 + time[1] % 16;
            Second = time[0] / 16 * 10 + time[0] % 16;
        }
    }

    if(Count_Temp >= 750)
    {
        Count_Temp = 0;
        Temp = Read_Temp();
    }

    if(Flag_SendTime == 1)
    {
        Flag_SendTime = 0;
        printf("BeiJing Time: %u  %bu:%bu:%bu\r\n", Year, Hour, Minute, Second);
    }

    if(Flag_SendTemp == 1)
    {
        Flag_SendTemp = 0;
        printf("Temperature: %4.2f C\r\n", Temp);
    }
}

void Logic_Task()
{
    if((R[1] == 't') && (R[2] == 'i') && (R[3] == 'm') &&
       (R[4] == 'e') && (R[5] == '1'))
    {
        Flag_SendTime = 1;
        Clear_Recv();
    }
    else if((R[1] == 't') && (R[2] == 'e') && (R[3] == 'm') &&
            (R[4] == 'p') && (R[5] == '2'))
    {
        Flag_SendTemp = 1;
        Clear_Recv();
    }

    // 超过100ms没有继续接收，认为一帧结束/无效
    if(Count_Recv >= 100)
    {
        Clear_Recv();
    }
}

int main()
{
    System_Init();

    while(1)
    {
        Data_Task();
        Logic_Task();
    }
}