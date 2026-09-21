#ifndef __DS1302_H
#define __DS1302_H
#include <library.h>
unsigned char Read_Ds1302_Byte ( unsigned char address );
void Set_Time(unsigned char hor,unsigned char min,unsigned char sec);
#endif