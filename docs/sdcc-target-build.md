# IAP15F2K61S2 SDCC software target foundation

This is a real SDCC MCS-51 compile, link and Intel HEX verification of the
repository's original `peripheral_policy.c`. The default entry uses synthetic
key events, records MCU XRAM diagnostics and reads three SFRs. It never writes
P0/P2/P3, selects a latch, starts a timer/UART, touches a board, or flashes a
device. The image is a software foundation, not a validated CT107D firmware.

## Device evidence and profiles

The [STC15 series data sheet](https://www.stcmicro.com/datasheet/STC15F2K60S2-en.pdf)
and [STC product table](https://www.stcmicro.com/cn/stc/stc15f2k60s2.html)
distinguish IAP15F2K61S2 (61 KiB program Flash, 2 KiB SRAM, on-chip emulator)
from STC15F2K60S2 (60 KiB program Flash, 2 KiB SRAM). The series has 256 bytes
of directly/indirectly addressed IRAM and 1792 bytes of auxiliary RAM reached
via MOVX. The data sheet's on-chip emulation section reserves CODE
`0xDC00–0xF3FF`, XRAM `0x0400–0x06FF` and P3.0/P3.1; user CODE is
`0x0000–0xDBFF` (55 KiB). It says no DATA/IDATA bytes are reserved by the
emulator. The default profile therefore limits CODE to 55 KiB and XRAM to
`0x0000–0x03FF` (1 KiB). `--profile standalone` permits 61 KiB CODE and
1792 bytes XRAM, but is only a separate compilation contract; no board mode
was observed. Both profiles use 256 bytes IRAM, SDCC small model, stack-auto,
64 bytes link-reserved stack and SDCC's generic startup/runtime. The memory
report and HEX checker reject allocations outside the selected limits.
This is a conservative *resource* profile, not a verified on-chip debugger
startup/link contract. In particular, the manual's illustrative `ORG 100H`
debug layout is not implemented by the generic SDCC startup in this image.

`target/sdcc/iap15_registers.h` contains only original declarations for P0
`0x80`, P2 `0xA0`, P3 `0xB0` (data sheet pp. 302–303) and P_SW1 `0xA2`
(p. 58). The entry reads P0, P2 and P_SW1 into diagnostic snapshots. It does
not access P3, including emulation-reserved P3.0/P3.1. P_SW1 is a
device-specific port switching SFR; no mux bits are changed. The C52
simulator's readback values are simulator defaults, not board measurements.
UART1 pin routing, timer clock mode, actual board oscillator, V31/V40
jumpers and latch output polarities require board revision and schematic
confirmation before any physical integration.

The software resource enum names LED `0x80`, buzzer/relay `0xA0`, digit
select `0xC0` and segment select `0xE0` as documented in this repository's
CT107D analysis. No function uses these values to write P2. A synthetic
request for UART1 on P3.0/P3.1 in emulation mode records a conflict; no UART
is initialized. The standalone compilation profile disables that emulation
conflict rule; it still does not configure or verify a real UART.

## Reproduce

Supply an installed SDCC with the MCS-51 backend, `packihx` and optionally
`s51`. Every output path must be new and outside this checkout.

```text
python3 scripts/build_sdcc_target.py --output /tmp/bluebridge-first
python3 scripts/build_sdcc_target.py --output /tmp/bluebridge-second
cmp /tmp/bluebridge-first/bluebridge_iap15_policy.hex /tmp/bluebridge-second/bluebridge_iap15_policy.hex
python3 scripts/run_sdcc_simulator.py --build /tmp/bluebridge-first
python3 scripts/build_sdcc_target.py --profile standalone --output /tmp/bluebridge-standalone
```

The script compiles `peripheral_policy.c`, `ct107d_platform.c`, and `main.c`
separately with `-mmcs51 --std-c11 --model-small --stack-auto --Werror`,
then links their REL files with the selected CODE/XRAM limits. It checks the
MAP for `PeripheralPolicy_KeyValue`, `Ct107d_ProcessSyntheticKey`, the read-only
SFR adapter and diagnostic object; it also checks the actual target assembly
call to the original policy. Other functions in `peripheral_policy.c` can be
present in the map due to SDCC's object granularity but are not claimed as
executed. The float decoder may add code even though this entry never calls
it. The script parses `.mem`, validates every IHX/HEX record and checksum,
EOF, reset vector, address bounds and overlaps, compares IHX and packed HEX
data, and reports a SHA-256 digest. `build.log`, `build-report.json` and the
simulator's raw/structured logs remain outside the repository.

The generic C52 uCsim check executes 500,000 MCS-51 instructions from the
linked IHX. It reads real XRAM diagnostics: four processed synthetic events,
two accepted, two rejected, last valid key 7, an emulation pin conflict and
two error bits. This proves the target CPU/software path and original policy
call, not IAP15 peripherals, exact instruction timing, latch electronics,
CT107D wiring or hardware runtime. A linker's available stack address range
is not measured worst-case stack depth.

## Current validation boundary

GCC and Clang Host CI still covers the original portable policies and
Keil-style practice modules. `uart_safe.c`, `iic_safe.c`, `ds18b20_safe.c`,
`rtc_startup.c` and `key_task_safe.c` are not in this target image: they need
legal board-level dependencies, SDCC-specific syntax adaptation and electrical
timing/clock or bus verification. The existing Host workflow only performs
syntax checking for two driver files. Neither this build nor Host CI verifies
real UART, PCF8591, DS18B20, DS1302, LED latch, display, relay or buzzer.

The existing CT107D documents summarize historical training material. The
original V31/V40 schematics and hardware-version notes are not in this public
repository and their redistribution rights were not established here. The
device profile is verified from STC's public manual; the actual board revision,
clock, jumpers, wiring and download configuration remain unverified.
