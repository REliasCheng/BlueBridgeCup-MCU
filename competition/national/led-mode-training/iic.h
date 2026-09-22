#ifndef __IIC_H
#define __IIC_H

#include <library.h>
uchar Read_Eeprom(uchar addr);
void Write_Eeprom(uchar addr, uchar value);
uchar Read_Adc(uchar addr);//0x43

#endif