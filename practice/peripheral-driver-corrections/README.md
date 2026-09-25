# 外设驱动修正版实践

这个目录保存 I²C、DS18B20、按键、DS1302 和 UART 的独立修正版。下表列出训练工程中的对应实现和调整内容。

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

## 主机逻辑测试

`peripheral_policy.c`保存不依赖寄存器的判断逻辑，C51 修正版与主机测试共用同一实现：

- RTC 时、分、秒的 BCD 范围检查；
- 按键扫描周期和值域检查；
- DS18B20 750 ms 转换完成条件；
- UART 环形队列索引回绕。

```powershell
gcc -std=c11 -Wall -Wextra -Werror -pedantic `
  peripheral_policy.c tests/test_peripheral_policy.c `
  -I . -o peripheral-policy-test.exe
./peripheral-policy-test.exe
```

主机测试覆盖可移植的状态与边界策略；I²C、1-Wire 和 UART 的电气时序在板端复测。

## 板端复测

修正版需要在 Keil C51 和 CT107D 上依次检查：

1. I²C 地址阶段和数据阶段的 ACK，以及 PCF8591 原始 ADC 字节。
2. DS18B20 启动转换到读取暂存器之间的时间间隔。
3. RTC 正常时间不被启动流程覆盖，CH 置位或 BCD 越界时才写默认值。
4. UART 连续输入时的队列溢出、帧恢复和发送完成路径。

基线接口与修正版差异已经逐项核对；构建输出、逻辑波形和传感器读数按实际复测记录。
