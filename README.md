# BlueBridgeCup-MCU

基于 CT107D 与 IAP15/STC15 兼容目标的蓝桥杯单片机训练工程。仓库包含 9 个 Keil C51 项目，从 P0/P2 锁存控制、数码管和按键扩展到 UART、PCF8591、DS18B20、参数保存与多任务综合程序。

## 工程

| 项目 | 实现 |
| --- | --- |
| [LED 与锁存器](projects/led-demo/README.md) | P0 数据、P2 选通、LED 位移和继电器定时翻转 |
| [数码管动态扫描](projects/segment-display-demo/README.md) | 段码表、位选、消影和整数拆位 |
| [独立按键](projects/key-demo/README.md) | P3 输入、按下确认和 LED 状态更新 |
| [UART 命令](projects/uart-demo/README.md) | Timer1 波特率、接收中断、time1 / temp2 命令 |
| [PCF8591 ADC](projects/i2c-adc-demo/README.md) | P2.0/P2.1 软件 I²C、ADC 读取与电压显示 |
| [DS18B20 温度](projects/ds18b20-demo/README.md) | P1.4 单总线、温度转换和周期显示 |
| [省赛综合控制](competition/province/integrated-control/README.md) | ADC、温度、按键、显示与输出联动 |
| [LED 模式综合训练](competition/national/led-mode-training/README.md) | AT24C02、ADC、软件 PWM、RTC、UART 和四种灯效 |
| [国赛方向模拟训练](competition/national/16th-practice/README.md) | 矩阵键、RTC、多页面和 Timer2 任务节拍 |

[外设驱动修正版](practice/peripheral-driver-corrections/README.md)单独实现 I²C、DS18B20、按键、RTC 和 UART 的改进接口，便于和训练源码逐项对照。

## CT107D 平台

- 开发板资料覆盖 V3.1 与 V4.0。
- 板载 MCU 为 IAP15F2K61S2；Keil 工程选用 STC15F2K60S2 Series 兼容目标。
- 多数工程按 12 MHz 配置，使用 Keil C51 / µVision 构建并由 STC-ISP 下载。
- V4.0 下载 IAP15 时，J9、J14 以板上丝印和硬件说明为准。

    P0 输出 8 位数据
    P2.5–P2.7 → 74HC138 → 74HC573
                             ├─ LED
                             ├─ 蜂鸣器 / 继电器
                             ├─ 数码管位选
                             └─ 数码管段选

Control(0x80, value) 和 Control(0xC0, value) 先写 P0，再通过 P2 选择锁存通道。数码管扫描按“关位 → 写段 → 开位”更新，Timer2 同时为显示、按键和周期任务提供节拍。

端口、外设和板卡版本见 [CT107D 硬件分析](docs/CT107D硬件分析.md)；Timer、UART、I²C、1-Wire 与 P3 复用见[资源分配与工程结构](docs/CT107D资源分配与竞赛工程结构.md)。

## 编译与下载

1. 进入项目目录，用 Keil C51 打开同目录的 .uvproj。
2. 核对 MCU 兼容目标、12 MHz 配置、板卡版本和拨码/跳线。
3. Build 后使用 STC-ISP 下载 HEX。
4. 按项目 README 检查输入、显示和串口输出。

部分中文注释原为 GBK，仓库通过 .gitattributes 保持 GitHub 页面可读。本地没有可用的 Keil C51 工具链，因此当前版本只核对了工程文件、源码接口和静态调用关系。

综合工程中的固定 temp、level、vol、wt 属于界面和任务调度用的模拟数据，不代表板端采样。相关检查项记录在[调试记录](docs/调试记录.md)，课程与第三方资料说明见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。
