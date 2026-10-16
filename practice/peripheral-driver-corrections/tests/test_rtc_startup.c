#include <assert.h>
#include <stdio.h>

#include "rtc_startup.h"

static uchar rtc_second;
static uchar rtc_minute;
static uchar rtc_hour;
static unsigned int set_time_calls;
static uchar written_hour;
static uchar written_minute;
static uchar written_second;

uchar Read_Ds1302_Byte(uchar address)
{
    switch (address) {
    case 0x81U: return rtc_second;
    case 0x83U: return rtc_minute;
    case 0x85U: return rtc_hour;
    default: assert(0 && "unexpected RTC register"); return 0U;
    }
}

void Set_Time(uchar hour, uchar minute, uchar second)
{
    ++set_time_calls;
    written_hour = hour;
    written_minute = minute;
    written_second = second;
}

int main(void)
{
    rtc_hour = 0x12U;
    rtc_minute = 0x34U;
    rtc_second = 0x56U;
    assert(RTC_HasValidTime());
    assert(!RTC_InitIfInvalid(0x10U, 0x20U, 0x30U));
    assert(set_time_calls == 0U);

    rtc_second = 0x80U; /* oscillator halted */
    assert(!RTC_HasValidTime());
    assert(!RTC_InitIfInvalid(0x24U, 0x00U, 0x00U));
    assert(!RTC_InitIfInvalid(0x12U, 0x60U, 0x00U));
    assert(!RTC_InitIfInvalid(0x12U, 0x00U, 0x6aU));
    assert(set_time_calls == 0U);

    assert(RTC_InitIfInvalid(0x23U, 0x59U, 0x55U));
    assert(set_time_calls == 1U);
    assert(written_hour == 0x23U);
    assert(written_minute == 0x59U);
    assert(written_second == 0x55U);

    puts("RTC startup policy tests passed");
    return 0;
}
