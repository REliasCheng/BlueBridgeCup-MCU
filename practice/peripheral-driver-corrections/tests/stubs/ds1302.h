#ifndef BLUEBRIDGE_HOST_DS1302_STUB_H
#define BLUEBRIDGE_HOST_DS1302_STUB_H

#include <library.h>

uchar Read_Ds1302_Byte(uchar address);
void Set_Time(uchar hour, uchar minute, uchar second);

#endif
