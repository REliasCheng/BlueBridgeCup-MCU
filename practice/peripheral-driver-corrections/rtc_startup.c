#include "rtc_startup.h"
#include "peripheral_policy.h"
#include <ds1302.h>

bit RTC_HasValidTime(void)
{
    uchar second = Read_Ds1302_Byte(0x81);
    uchar minute = Read_Ds1302_Byte(0x83);
    uchar hour = Read_Ds1302_Byte(0x85);
    if(second & 0x80) {
        return 0;
    }
    return PeripheralPolicy_IsValidBcd(second & 0x7f, 59) &&
           PeripheralPolicy_IsValidBcd(minute, 59) &&
           PeripheralPolicy_IsValidBcd(hour, 23);
}

bit RTC_InitIfInvalid(uchar default_hour, uchar default_minute, uchar default_second)
{
    if(RTC_HasValidTime()) {
        return 0;
    }
    Set_Time(default_hour, default_minute, default_second);
    return 1;
}
