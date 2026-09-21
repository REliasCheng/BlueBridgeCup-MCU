#ifndef __DS1302_H
#define __DS1302_H

#include <library.h>

unsigned char Read_Ds1302_Byte(unsigned char address);
void Set_Time(uchar hor, uchar min, uchar sec);

#endif