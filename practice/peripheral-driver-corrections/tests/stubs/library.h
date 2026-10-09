#ifndef BLUEBRIDGE_HOST_LIBRARY_STUB_H
#define BLUEBRIDGE_HOST_LIBRARY_STUB_H

/* Host-only register stand-ins. This does not emulate 8051 timing or a CT107D. */
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned char bit;
#define xdata
#define _nop_() ((void)0)

extern volatile uchar SCON;
extern volatile uchar AUXR;
extern volatile uchar TMOD;
extern volatile uchar TL1;
extern volatile uchar TH1;
extern volatile uchar ET1;
extern volatile uchar TR1;
extern volatile uchar ES;
extern volatile uchar EA;
extern volatile uchar RI;
extern volatile uchar TI;
extern volatile uchar SBUF;

#endif
