#include "rtc_startup.h"
#include <ds1302.h>

static bit valid_bcd(uchar value, uchar max_decimal)
{
    uchar high = (value >> 4) & 0x0f;
    uchar low = value & 0x0f;
    return low <= 9 && high <= 9 && (high * 10 + low) <= max_decimal;
}

bit RTC_HasValidTime(void)
{
    uchar second = Read_Ds1302_Byte(0x81);
    uchar minute = Read_Ds1302_Byte(0x83);
    uchar hour = Read_Ds1302_Byte(0x85);
    if(second & 0x80) {
        return 0;
    }
    return valid_bcd(second & 0x7f, 59) && valid_bcd(minute, 59) && valid_bcd(hour, 23);
}

bit RTC_InitIfInvalid(uchar default_hour, uchar default_minute, uchar default_second)
{
    if(RTC_HasValidTime()) {
        return 0;
    }
    Set_Time(default_hour, default_minute, default_second);
    return 1;
}
