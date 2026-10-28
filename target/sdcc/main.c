#include "ct107d_platform.h"

void main(void)
{
    /* Synthetic input only. No GPIO writes, interrupts, timers or latches. */
    Iap15_CaptureReadOnlyPorts();
    Ct107d_ProcessSyntheticKey(0U, 4U);
    Ct107d_ProcessSyntheticKey(1U, 3U);
    Ct107d_ProcessSyntheticKey(1U, 6U);
    Ct107d_ProcessSyntheticKey(1U, 7U);
    Ct107d_CheckPinConflict(1U);
    for (;;) {
    }
}
