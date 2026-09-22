# BlueBridgeCup-MCU 本轮交付记录

## 1. 修改内容

- 根 `README.md` 将模块清单改为“技术能力与代码证据”：从 GPIO、定时中断、UART、I²C/1-Wire 到综合任务组织，均可直接打开对应工程。
- 根 `README.md` 区分当前 C51/CT107D 代码与后续 STM32、RTOS 方向，不把计划写成已实现能力。
- 核对了 9 个项目说明及 4 篇 `docs/` 文档。现有工程已覆盖 LED、数码管、按键、UART、ADC、DS18B20、省赛综合控制、LED 模式与多界面训练，本轮未为凑数量移动或复制项目。

## 2. 新增内容

- `PROJECT_REVIEW.md`：记录升级前的优点、问题与本轮取舍。
- 本文件：记录交付范围和检查结论。

## 3. 删除内容

无。没有删除原有工程、源码或资料。

## 4. 项目结构

```text
BlueBridgeCup-MCU/
├─ README.md
├─ PROJECT_REVIEW.md
├─ PROJECT_FINAL_REPORT.md
├─ projects/                 6 个基础与驱动工程
│  ├─ led-demo/
│  ├─ segment-display-demo/
│  ├─ key-demo/
│  ├─ uart-demo/
│  ├─ i2c-adc-demo/
│  └─ ds18b20-demo/
├─ competition/
│  ├─ province/integrated-control/
│  └─ national/
│     ├─ led-mode-training/
│     └─ 16th-practice/
└─ docs/                     硬件分析、学习路线、竞赛体系、调试记录
```

每个展示工程有独立 README、C 源码和 Keil 工程文件。`docs/` 用已有代码解释硬件路径、任务安排和具体检查点；不是教材附件目录。

## 5. README 优化情况

首页第一段说明了蓝桥杯单片机训练、CT107D 平台与 9 个工程；项目表可以直接进入具体代码。新增的能力表把抽象技术词落到可检查的工程入口。学习路线从基础控制、显示和输入，推进到通信、传感器及省赛/国赛方向综合训练。

## 6. 技术能力展示点

| 能力 | 代码证据 |
|---|---|
| GPIO、译码与锁存 | LED 工程的 P0/P2 输出；数码管工程的位选/段选扫描 |
| 定时器与中断 | Timer2 扫描/任务节拍；UART 工程使用 Timer1 配波特率 |
| 通信与驱动 | UART 中断接收、查询发送；软件 I²C/PCF8591；DS18B20 单总线 |
| 数据与状态组织 | 省赛工程的 ADC/温度/输出联动；LED 模式工程的 EEPROM/RTC/PWM；多界面工程的矩阵键与任务计数 |
| 工程检查 | `docs/调试记录.md` 区分源码中已定位的问题与尚需板端复测的行为 |

这些是仓库现有代码和文档能展示的范围。项目页没有把历史编译记录写成实物测试，也没有把模拟训练工程写成正式比赛成绩。

## 7. Git 提交记录

已有提交沿基础工程、硬件映射、UART/驱动检查、省赛综合工程、国赛方向 LED 模式工程逐步增加内容；可用 `git log --oneline` 查看各次代码与文档变化。本轮提交信息为“补充技术能力证据与嵌入式学习路线”，只包含 Markdown 文档变化，不修改 C/H/Keil 工程。
