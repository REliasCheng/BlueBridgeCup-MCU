#ifndef BLUEBRIDGE_IAP15_REGISTERS_H
#define BLUEBRIDGE_IAP15_REGISTERS_H

/* Minimal original declarations from STC15 series data sheet, pp. 302-303,
 * 58. The target entry only reads these SFRs; it never selects a latch. */
__sfr __at (0x80) IAP15_P0;
__sfr __at (0xA0) IAP15_P2;
__sfr __at (0xB0) IAP15_P3;
__sfr __at (0xA2) IAP15_P_SW1;

#endif
