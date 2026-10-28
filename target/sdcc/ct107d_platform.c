#include "ct107d_platform.h"
#include "iap15_registers.h"
#include "peripheral_policy.h"

#ifndef CT107D_ON_CHIP_EMULATION
#error CT107D_ON_CHIP_EMULATION must be provided by the target build
#endif

__xdata volatile struct Ct107dDiagnostics ct107d_diagnostics;
__code const unsigned char ct107d_abi_sizes[3] = {
    sizeof(struct Ct107dDiagnostics), sizeof(void *), sizeof(void (*)(void))
};

void Ct107d_ProcessSyntheticKey(unsigned char scan_due, unsigned char key)
{
    unsigned char candidate = ct107d_diagnostics.last_valid_key;
    ++ct107d_diagnostics.processed;
    if (PeripheralPolicy_KeyValue(scan_due, key, &candidate)) {
        ct107d_diagnostics.last_valid_key = candidate;
        ++ct107d_diagnostics.accepted;
    } else {
        ++ct107d_diagnostics.rejected;
        ct107d_diagnostics.error_latch |= 0x01U;
    }
}

void Ct107d_CheckPinConflict(unsigned char wants_uart1_on_p30_p31)
{
    /* STC's on-chip emulation reserves these pins. No UART is configured. */
#if CT107D_ON_CHIP_EMULATION
    if (wants_uart1_on_p30_p31) {
        ct107d_diagnostics.resource_conflict = 1U;
        ct107d_diagnostics.error_latch |= 0x02U;
    }
#else
    (void)wants_uart1_on_p30_p31;
#endif
}

void Iap15_CaptureReadOnlyPorts(void)
{
    ct107d_diagnostics.p0_snapshot = IAP15_P0;
    ct107d_diagnostics.p2_snapshot = IAP15_P2;
    ct107d_diagnostics.port_switch_snapshot = IAP15_P_SW1;
}
