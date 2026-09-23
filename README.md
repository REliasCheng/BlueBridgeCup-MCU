# BlueBridgeCup-MCU

## 项目简介

这是我的蓝桥杯单片机竞赛训练与嵌入式学习记录。基于 CT107D、IAP15/STC15 兼容目标和 C51，从锁存控制、显示与按键，推进到通信、传感器驱动，再把采样、参数保存和任务调度放进综合工程。源码保留训练时的写法，项目页说明硬件关系、程序结构和代码检查中发现的问题。

目前有 **9 个独立的 Keil C51 工程**：6 个基础/驱动工程、1 个省赛综合控制工程，以及 2 个国赛方向训练工程。每个工程保留源码、工程文件和对应说明；另有一组[外设驱动修正版实践](practice/peripheral-driver-corrections/README.md)，用于对比I²C、1-Wire、按键、RTC和UART接口的改进方式。

## 项目列表

| 项目 | 已写入的功能 | 可以从代码中看到什么 |
|---|---|---|
| [LED 与锁存器](projects/led-demo/README.md) | LED 位置变化、继电器定时翻转 | P0 数据、P2 选通、Timer2 节拍与状态更新 |
| [数码管动态扫描](projects/segment-display-demo/README.md) | 8 位扫描显示固定数值 | 段码表、位选、消影顺序、整数拆位 |
| [独立按键](projects/key-demo/README.md) | 按键映射为 LED 状态 | P3 输入、按下确认、事件与输出的分离 |
| [UART 命令](projects/uart-demo/README.md) | `time1` / `temp2` 命令与数据输出 | Timer1 波特率、接收中断、查询发送与主循环命令处理 |
| [PCF8591 ADC](projects/i2c-adc-demo/README.md) | 采样模拟量并显示电压 | P2.0/P2.1 软件 I²C、转换与显示 |
| [DS18B20 温度](projects/ds18b20-demo/README.md) | 单总线读取并显示温度 | P1.4 时序、温度数据处理、周期采样 |
| [省赛综合控制训练](competition/province/integrated-control/README.md) | ADC、温度、按键、显示与输出联动 | 采样节拍、阈值判断、多外设任务分工 |
| [LED 模式综合训练](competition/national/led-mode-training/README.md) | 四种流转模式、参数保存、亮度控制与串口报告 | AT24C02、ADC、软件 PWM、RTC、UART 和状态调度 |
| [国赛方向模拟训练](competition/national/16th-practice/README.md) | 数据/参数/校准界面与 RTC 读取 | Timer2 统一节拍、矩阵键、状态与多任务组织 |

## 平台环境

| 项目 | 当前仓库使用情况 |
|---|---|
| 开发板 | CT107D（资料覆盖 V31、V40） |
| 板载 MCU | IAP15F2K61S2；Keil 工程使用 `STC15F2K60S2 Series` 兼容目标 |
| 语言与 IDE | C、Keil C51 / µVision |
| 下载与调试 | STC-ISP、串口调试工具；多个练习工程按 12 MHz 配置 |

V40 下载 IAP15 时，J9、J14 的设置要以板上丝印和硬件说明为准。具体外设引脚见 [CT107D 硬件分析](docs/CT107D硬件分析.md)。

## 技术能力与代码证据

| 能力 | 工程中的实现与入口 |
|---|---|
| GPIO 与锁存控制 | [LED 工程](projects/led-demo/README.md)沿 P0 数据、P2 选通、74HC138/74HC573 到 LED 与继电器追踪输出；[硬件分析](docs/CT107D硬件分析.md)记录这条通路。 |
| 定时器、中断与输入显示 | [数码管工程](projects/segment-display-demo/README.md)在 Timer2 中断中逐位扫描；[按键工程](projects/key-demo/README.md)用节拍确认按下并更新 LED 状态。 |
| 串口通信 | [UART 工程](projects/uart-demo/README.md)用 Timer1 配波特率、中断接收命令，主循环处理后经 `putchar()` 查询 `TI` 发送。 |
| 外设时序与数据处理 | [PCF8591 工程](projects/i2c-adc-demo/README.md)实现软件 I²C 与 ADC 换算；[DS18B20 工程](projects/ds18b20-demo/README.md)处理 1-Wire 命令与温度字节。 |
| 多模块程序组织 | [省赛综合控制](competition/province/integrated-control/README.md)连接采样、按键、显示和输出；[LED 模式训练](competition/national/led-mode-training/README.md)加入 EEPROM、RTC 与软件 PWM；[多界面训练](competition/national/16th-practice/README.md)组织矩阵键、页面状态和任务节拍。 |
| 驱动检查与修正 | [practice修正版](practice/peripheral-driver-corrections/README.md)将ACK结果、温度转换阶段、RTC启动策略和UART ISR职责写入独立接口，原训练源码保持可对照。 |

## 学习路线

```text
基础控制（P0/P2、LED）
  ↓
显示（段码、位选、Timer2 扫描）
  ↓
输入（独立按键、消抖）
  ↓
通信（UART 中断接收、查询发送与命令处理）
  ↓
外设（I²C/ADC、DS18B20、RTC、EEPROM）
  ↓
省赛综合控制（采样、按键、显示、输出）
  ↓
国赛方向训练（模式控制、参数保存、界面与任务调度）
```

前六个工程分别处理单点问题，后三个工程练习跨模块协作。[学习路线](docs/学习路线.md)列出了对应的源码链路。

## 硬件与软件的对应

```text
IAP15 / STC15 兼容工程
  ├─ P0 输出 8 位数据
  └─ P2.5～P2.7 → 74HC138 译码 → 74HC573 锁存
                                       ├─ LED
                                       ├─ 蜂鸣器 / 继电器
                                       ├─ 数码管位选
                                       └─ 数码管段选
```

`Control(0x80, value)`、`Control(0xC0, value)` 这样的调用背后是先写 P0，再用 P2 选锁存通道。数码管在中断里按“关位 → 写段 → 开位”刷新；显示机制需要结合中断而不只是 `main()` 来看。[CT107D 硬件分析](docs/CT107D硬件分析.md)把各项目中的端口、函数和外设对应起来。

[CT107D 资源分配与竞赛工程结构](docs/CT107D资源分配与竞赛工程结构.md)进一步汇总 Timer0/1/2、UART、I²C、1-Wire、RTC 和 P3 复用关系，并说明基础驱动、综合训练与修正版 practice 的职责边界。


## 编译与运行

1. 进入某个项目目录，先读它的 README，再用 Keil C51 打开同目录的 `.uvproj`。
2. 核对兼容目标器件、12 MHz 配置、具体板型与拨码/跳线。不同工程共用 P3、P2 引脚时尤其要检查连接。
3. Build 后用 STC-ISP 下载生成的 HEX，按项目 README 的输入和预期输出逐项观察。
4. 将板端现象、照片或串口输出记录到对应项目，并在 [调试记录](docs/调试记录.md)中关联问题和修改提交。

仓库保留各工程的 C 源码和 Keil 相对路径；部分中文注释原为 GBK 编码，`.gitattributes` 负责 GitHub 页面上的文本显示。外设测量以板端记录为准，代码结构、编译结果和实际采样分别留证。

## 调试与竞赛训练

[调试记录](docs/调试记录.md)记录按键变量赋值、RTC 启动设时、ADC 换算和单总线转换等待等检查点，并区分代码检查与板端结果。[蓝桥杯竞赛体系](docs/蓝桥杯竞赛体系.md)展示如何将单模块工程组织成综合训练程序。

课程与第三方内容的许可边界集中见[来源与许可说明](THIRD_PARTY_NOTICES.md)。

## 后续方向

当前仓库的实现基于 C51 与 CT107D。接下来先补板端照片、串口输出和测量对照记录，核查 I²C 接收与 DS18B20 转换时序；超声波与 PCA 测距在形成可复现工程后再加入。之后把这里练过的驱动分层、定时任务和调试方法带到 STM32、RTOS 项目中，相关代码完成后再单独展示。

## 技术路线中的位置

[Embedded-Systems-Foundations](https://github.com/REliasCheng/Embedded-Systems-Foundations) → [Embedded-C-Cpp-Learning](https://github.com/REliasCheng/Embedded-C-Cpp-Learning) → [stc89c52-learning](https://github.com/REliasCheng/stc89c52-learning) → **BlueBridgeCup-MCU**
