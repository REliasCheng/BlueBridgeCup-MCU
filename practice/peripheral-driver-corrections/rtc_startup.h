#ifndef __RTC_STARTUP_H__
#define __RTC_STARTUP_H__

#include <library.h>

bit RTC_HasValidTime(void);
bit RTC_InitIfInvalid(uchar default_hour, uchar default_minute, uchar default_second);

#endif
