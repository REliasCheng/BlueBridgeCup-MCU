# 学习路线与当前进度

这里记录仓库里确实有工程可看的阶段，不把“资料中出现过”写成“我已经做完”。

| 阶段 | 当前工程 | 接下来要补的证据 |
|---|---|---|
| GPIO 与锁存器 | [LED 练习](../projects/led-demo/README.md) | LED/继电器实际运行照片、运行节拍 |
| 显示 | [数码管练习](../projects/segment-display-demo/README.md) | 8 位显示照片、刷新与重影检查 |
| 输入 | [独立按键练习](../projects/key-demo/README.md) | 按键响应、消抖与变量疑点复测 |
| 通信 | [UART 小工程](../projects/uart-demo/README.md) | 串口参数、命令与实际输出截图 |
| I²C/ADC | [PCF8591 练习](../projects/i2c-adc-demo/README.md) | 电压输入与显示读数对照 |
| 温度传感器 | [DS18B20 练习](../projects/ds18b20-demo/README.md) | 温度变化过程与稳定性记录 |
| 比赛训练 | [国赛方向模拟训练](../competition/national/16th-practice/README.md) | 功能清单、板端逐项检查及未完成项 |

这些工程都保留了原始 Keil 项目和源码。原资料中的构建记录显示它们曾经编译通过；本仓库仍需在复制后的路径重新构建和实物验证。

## 后续顺序

1. 先完成上述项目的板端验证、照片和调试记录。
2. 增加 EEPROM/AT24C02、DS1302、矩阵键等独立练习。
3. 将 PWM、超声波、定时器资源分配做成可复现的实验。
4. 完成一个经过实测的省赛综合项目，再完善国赛方向的复杂串口与任务调度。
5. 逐步建立 STM32、FreeRTOS、Linux 和 ROS 的独立实践；目前它们只是计划。

每新增一个工程都先写清目标、接线/配置、输入、预期输出、实际结果，再更新根 README。
