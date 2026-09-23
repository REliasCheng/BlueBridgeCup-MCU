# 外设驱动修正版实践

这个目录针对训练工程中已经能够从代码确定的问题建立独立修正版。`projects/`和`competition/`中的原训练源码保持不变，便于比较“原来怎么写”和“为什么调整”。

## 修正范围

| 模块 | 原代码路径 | practice处理 |
| --- | --- | --- |
| I²C / PCF8591 | `projects/i2c-adc-demo/iic.c` | 接收变量从0开始；发送返回ACK结果；读取结束由主机发送NACK；调用方能识别地址阶段失败 |
| DS18B20 | `projects/ds18b20-demo/onewire.c` | 读字节变量初始化；把`0x44`启动转换与读取暂存器分开；阻塞接口等待最长750 ms |
| 独立按键 | `projects/key-demo/main.c` | 键值只在扫描到期后使用，消除未赋值局部变量路径 |
| DS1302 | `projects/uart-demo/main.c` | 启动时先检查时分秒和CH位，只在RTC值无效时写默认时间 |
| UART | `projects/uart-demo/uart.c`与`main.c` | TI统一由串口ISR清除并置发送完成标志；RI只入环形缓冲，主循环读取和解析 |

## 接口设计

```text
UART RX中断 → rx_buffer → UartSafe_ReadByte() → main解析
UART发送     → SBUF → TI中断 → tx_done → 发送函数返回

DS18B20_StartConversion()
        ↓ 至少750 ms
DS18B20_ReadTemperature()

RTC_InitIfInvalid()
        ├─ 时间有效：保留当前RTC
        └─ CH置位/BCD越界：写入调用方提供的默认值
```

I²C API把“总线动作成功”与“ADC数值”分开返回，避免把NACK后的字节继续当作有效采样。接收最后一个字节时由主机发送NACK并停止，这与PCF8591单字节读取的主机角色一致。

## 资源策略

`uart_safe.c`选择“TI由ISR统一处理”：发送函数等待`tx_done`，不再与ISR同时清`TI`。RX ISR只做字节入队，帧边界和命令解释留给主循环。固定32字节队列满时设置溢出标志，调用方可以丢弃当前帧并重新同步。

## 验证边界

本轮完成源码路径检查、C51接口检查和原版/修正版差异检查。当前环境没有Keil C51和CT107D实板，因此没有写入虚构的编译、波形或传感器精度结论。板端复测应记录ACK、原始ADC字节、DS18B20转换周期、RTC断电保持以及UART连续输入结果。
