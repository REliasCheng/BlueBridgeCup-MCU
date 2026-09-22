#ifndef __DS1302_H
#define __DS1302_H

#include <library.h>

unsigned char Read_Ds1302_Byte(unsigned char address);
void Write_Ds1302_Byte(unsigned char address, unsigned char dat);
void Set_Time(uchar hor, uchar min, uchar sec);

#endif