#ifndef BLUEBRIDGE_CT107D_PLATFORM_H
#define BLUEBRIDGE_CT107D_PLATFORM_H

/* Software resource labels, not a verified physical board revision. */
enum Ct107dLatchChannel {
    CT107D_LED = 0x80,
    CT107D_BUZZER_RELAY = 0xA0,
    CT107D_DIGIT_SELECT = 0xC0,
    CT107D_SEGMENT_SELECT = 0xE0
};

struct Ct107dDiagnostics {
    unsigned char processed;
    unsigned char accepted;
    unsigned char rejected;
    unsigned char last_valid_key;
    unsigned char resource_conflict;
    unsigned char error_latch;
    unsigned char p0_snapshot;
    unsigned char p2_snapshot;
    unsigned char port_switch_snapshot;
};

extern __xdata volatile struct Ct107dDiagnostics ct107d_diagnostics;
void Ct107d_ProcessSyntheticKey(unsigned char scan_due, unsigned char key);
void Ct107d_CheckPinConflict(unsigned char wants_uart1_on_p30_p31);
void Iap15_CaptureReadOnlyPorts(void);

#endif
