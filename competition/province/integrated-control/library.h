#ifndef __LIBRARY_H__
#define __LIBRARY_H__
#include "stdio.h"
#include "intrins.h"
#include <stc15.h>
typedef unsigned char uchar;
typedef unsigned int uint;
#define Control(x,y) P0=y;P2=x;P2=0
void Timer2_Init(void);
uchar Read_4Key(void);




#endif