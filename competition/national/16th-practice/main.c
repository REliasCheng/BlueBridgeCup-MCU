#include <library.h>
#include <ds1302.h>

#define OFF     10
#define DASH    11
#define P_      12
#define E_      13

#define DATA    1
#define PARA    2
#define CALIB   3

#define D_TIME  1
#define D_LEVEL 2
#define D_VOL   3
#define D_WT    4

code uchar Code[] =
{
    0xc0, 0xf9, 0xa4, 0xb0, 0x99,
    0x92, 0x82, 0xf8, 0x80, 0x90,
    0xff, 0xbf, 0x8c, 0x86
};

xdata uchar Bit[9] =
{
    OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF
};

xdata uchar Led[9] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

xdata uint Count[9] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

/******************** 简洁变量 ********************/
uchar ui;       // 主界面：1数据 2参数 3标定
uchar dm;       // 数据子界面
uchar pm;       // 参数编号
uchar em;       // 标定编号

uchar h, m, s;  // 时间
uchar buz, rel; // 蜂鸣器、继电器

uint h1, h2;    // 参数扩大10倍
uint fp;        // 频率参数
uchar st;       // 容器类型
uint rr, ll, ww, hh;

xdata uchar ev[5];   // 标定电压扩大10倍：0.0/1.0/2.0/4.0/5.0

/******************** 假数据：基础分用来显示 ********************/
uchar temp = 23;
uint level = 35;     // 0.35m
uint vol = 35;       // 3.5
uint wt = 35;        // 3.5

bit Delay(uchar num, uint time)
{
    while(T2H == 0xff);

    if(Count[num] >= time)
    {
        Count[num] = 0;
        return 1;
    }

    return 0;
}

void Timer2_Service(void) interrupt 12
{
    static uchar dsp = 0;
    uchar i = 0;

    /******************** LED 输出 ********************/
    Control(0x80, ~(Led[8]<<7 | Led[7]<<6 | Led[6]<<5 | Led[5]<<4 |
                    Led[4]<<3 | Led[3]<<2 | Led[2]<<1 | Led[1]<<0));

    /******************** 蜂鸣器 / 继电器输出 ********************/
    Control(0xa0, buz<<6 | rel<<4);

    /******************** 数码管扫描 ********************/
    if(++dsp > 8)
    {
        dsp = 1;
    }

    Control(0xc0, 0);     // 消影

    if(Bit[dsp] >= '.')
    {
        Control(0xe0, Code[Bit[dsp] - '.'] & 0x7f);
    }
    else
    {
        Control(0xe0, Code[Bit[dsp]]);
    }

    Control(0xc0, 1 << (dsp - 1));

    /******************** 时间片计数 ********************/
    for(i = 0; i < 9; i++)
    {
        Count[i]++;
    }
}

void System_Init(void)
{
    Control(0x80, 0xff);   // 关闭 LED
    Control(0xa0, 0x00);   // 关闭蜂鸣器/继电器

    buz = 0;
    rel = 0;

    ui = DATA;
    dm = D_TIME;
    pm = 1;
    em = 0;

    h1 = 10;       // 1.0m
    h2 = 1;        // 0.1m
    fp = 2000;     // 2000Hz
    st = 2;        // 长方体

    rr = 10;
    ll = 10;
    ww = 10;
    hh = 10;

    ev[0] = 0;     // 0.0V
    ev[1] = 10;    // 1.0V
    ev[2] = 20;    // 2.0V
    ev[3] = 40;    // 4.0V
    ev[4] = 50;    // 5.0V

    h = 23;
    m = 59;
    s = 50;

    Set_Time(0x23, 0x59, 0x50);

    Timer2_Init();
}

void Show_Time(void)
{
    Bit[1] = h / 10;
    Bit[2] = h % 10;
    Bit[3] = DASH;
    Bit[4] = m / 10;
    Bit[5] = m % 10;
    Bit[6] = DASH;
    Bit[7] = s / 10;
    Bit[8] = s % 10;
}

void Show_Level(void)
{
    Bit[1] = OFF;
    Bit[2] = OFF;
    Bit[3] = temp / 10;
    Bit[4] = temp % 10;
    Bit[5] = DASH;
    Bit[6] = level / 100 + '.';
    Bit[7] = level / 10 % 10;
    Bit[8] = level % 10;
}

void Show_Vol(void)
{
    Bit[1] = OFF;
    Bit[2] = 2;
    Bit[3] = 3;
    Bit[4] = DASH;
    Bit[5] = OFF;
    Bit[6] = OFF;
    Bit[7] = vol / 10 + '.';
    Bit[8] = vol % 10;
}

void Show_Wt(void)
{
    Bit[1] = OFF;
    Bit[2] = temp / 10;
    Bit[3] = temp % 10;
    Bit[4] = DASH;
    Bit[5] = OFF;
    Bit[6] = OFF;
    Bit[7] = wt / 10 + '.';
    Bit[8] = wt % 10;
}

void Show_Para(void)
{
    uint v = 0;

    Bit[1] = P_;
    Bit[2] = pm;
    Bit[3] = DASH;
    Bit[4] = OFF;
    Bit[5] = OFF;
    Bit[6] = OFF;
    Bit[7] = OFF;
    Bit[8] = OFF;

    if(pm == 1)      v = h1;
    else if(pm == 2) v = h2;
    else if(pm == 3)
    {
        Bit[5] = (fp >= 1000) ? (fp / 1000 % 10) : OFF;
        Bit[6] = (fp >= 100)  ? (fp / 100 % 10)  : OFF;
        Bit[7] = (fp >= 10)   ? (fp / 10 % 10)   : OFF;
        Bit[8] = fp % 10;
        return;
    }
    else if(pm == 4)
    {
        Bit[8] = st;
        return;
    }
    else if(pm == 5) v = rr;
    else if(pm == 6) v = ll;
    else if(pm == 7) v = ww;
    else if(pm == 8) v = hh;

    Bit[7] = v / 10 + '.';
    Bit[8] = v % 10;
}

void Show_Calib(void)
{
    Bit[1] = E_;

    if(em == 0)
    {
        Bit[2] = 0;
        Bit[3] = 0;
    }
    else if(em == 1)
    {
        Bit[2] = 0;
        Bit[3] = 5;
    }
    else if(em == 2)
    {
        Bit[2] = 1;
        Bit[3] = 0;
    }
    else if(em == 3)
    {
        Bit[2] = 1;
        Bit[3] = 5;
    }
    else
    {
        Bit[2] = 2;
        Bit[3] = 0;
    }

    Bit[4] = OFF;
    Bit[5] = OFF;
    Bit[6] = OFF;
    Bit[7] = ev[em] / 10 + '.';
    Bit[8] = ev[em] % 10;
}

void Display_Task(void)
{
    if(ui == DATA)
    {
        if(dm == D_TIME)       Show_Time();
        else if(dm == D_LEVEL) Show_Level();
        else if(dm == D_VOL)   Show_Vol();
        else if(dm == D_WT)    Show_Wt();
    }
    else if(ui == PARA)
    {
        Show_Para();
    }
    else if(ui == CALIB)
    {
        Show_Calib();
    }
}

void Key_Task(void)
{
    uchar key = 0;

    if(Delay(0, 1))
    {
        key = Read_16Key();
    }
    else
    {
        return;
    }

    if(key == 4)       // S4：主界面
    {
        if(++ui > CALIB)
        {
            ui = DATA;
        }

        if(ui == DATA)
        {
            dm = D_TIME;
        }
        else if(ui == PARA)
        {
            pm = 1;
        }
        else if(ui == CALIB)
        {
            em = 0;
        }
    }
    else if(key == 5)  // S5：子界面
    {
        if(ui == DATA)
        {
            if(++dm > D_WT)
            {
                dm = D_TIME;
            }
        }
        else if(ui == PARA)
        {
            if(++pm > 8)
            {
                pm = 1;
            }
        }
        else if(ui == CALIB)
        {
            if(++em > 4)
            {
                em = 0;
            }
        }
    }
    else if(key == 8)  // S8：标定加
    {
        if(ui == CALIB)
        {
            if(ev[em] < 50)
            {
                ev[em]++;
            }
        }
    }
    else if(key == 9)  // S9：标定减
    {
        if(ui == CALIB)
        {
            if(ev[em] > 0)
            {
                ev[em]--;
            }
        }
    }
}

void Data_Task(void)
{
    uchar time[3];

    if(T2H < 0xd9)
    {
        if(Delay(1, 150))
        {
            time[2] = Read_Ds1302_Byte(0x85);
            time[1] = Read_Ds1302_Byte(0x83);
            time[0] = Read_Ds1302_Byte(0x81);

            h = time[2] / 16 * 10 + time[2] % 16;
            m = time[1] / 16 * 10 + time[1] % 16;
            s = time[0] / 16 * 10 + time[0] % 16;
        }
    }
}

void Logic_Task(void)
{
    uchar i = 0;

    for(i = 1; i <= 8; i++)
    {
        Led[i] = 0;
    }

    if(ui == DATA)
    {
        if(dm == D_TIME)       Led[1] = 1;
        else if(dm == D_LEVEL) Led[2] = 1;
        else if(dm == D_VOL)   Led[3] = 1;
        else if(dm == D_WT)    Led[4] = 1;
    }

    buz = 0;
    rel = 0;
}

int main(void)
{
    System_Init();

    while(1)
    {
        Display_Task();
        Key_Task();
        Data_Task();
        Logic_Task();
    }
}