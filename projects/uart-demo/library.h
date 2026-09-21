#ifndef __LIBRARY_H
#define __LIBRARY_H

#include <stc15.h>
#include <intrins.h>
#include <stdio.h>

typedef unsigned char uchar;
typedef unsigned int  uint;

#define Control(x, y)  P0 = y; P2 = x; P2 = 0

void Timer2_Init();

#endif