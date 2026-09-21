#ifndef __LIBRARY_H__
#define __LIBRARY_H__
#include "stdio.h"
#include "intrins.h"
#include <stc15.h>
#define Control(x,y) P0 = y;P2 = x,P2 = 0
typedef unsigned char uchar;
typedef unsigned int uint;
void Timer2_Init();
uchar Read_4Key();
#endif


