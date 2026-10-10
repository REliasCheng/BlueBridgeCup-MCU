# 外设驱动修正版实践

这个目录保存 I²C、DS18B20、按键、DS1302 和 UART 的独立实践。公开仓库没有早期训练工程的 `projects/` 目录；下表只指向现有文件，不把历史材料当作可访问或可再分发的源码。

## 修正范围

| 模块 | 当前文件 | practice 处理 |
| --- | --- | --- |
| I²C / PCF8591 | [`iic_safe.c`](iic_safe.c)、[`peripheral_policy.c`](peripheral_policy.c) | 检查总线可用性和 ACK，丢弃前次转换字节，对当前字节发送 NACK；错误保持调用方输出不变 |
| DS18B20 | [`ds18b20_safe.c`](ds18b20_safe.c)、[`peripheral_policy.c`](peripheral_policy.c) | 启动与读取分离；完整 9 字节 scratchpad CRC 校验；失败不伪装为 0℃ |
| 独立按键 | [`key_task_safe.c`](key_task_safe.c) | 键值只在扫描到期后使用 |
| DS1302 | [`rtc_startup.c`](rtc_startup.c) | 启动时检查时分秒和 CH 位 |
| UART | [`uart_safe.c`](uart_safe.c) | RX 中断入队；TX 使用有界轮询预算，超时后保持在途状态 |

## 接口设计

```text
UART RX中断 → rx_buffer → UartSafe_ReadByte() → main解析
UART 发送    → SBUF → TI 中断 → 完成 / 超时未知 / 忙碌

DS18B20_StartConversion()
        ↓ 至少 750 ms
DS18B20_ReadTemperature(elapsed_ms, &temperature_c) → 成功状态 + 输出值

RTC_InitIfInvalid()
├─ 时间有效：保留当前 RTC
        └─ CH置位/BCD越界：写入调用方提供的默认值
```

PCF8591 单端通道仅接受 0–3。按 [NXP PCF8591 数据手册](https://www.nxp.com/docs/en/data-sheet/PCF8591.pdf)，读周期第一字节是前次转换结果；事务先 ACK 并丢弃该字节，再读取当前结果、发送 NACK 并 STOP。Host mock 测试覆盖此序列，尚无 CT107D 电气实测。总线被占用时不盲目发恢复脉冲；起始失败不改变输出，STOP 失败有单独状态。

## 资源策略

`uart_safe.c` 选择“TI 由 ISR 统一处理”：`UartSafe_SendByte(value, poll_budget)` 返回 `OK`、`BUSY`、`TIMEOUT` 或 `INTERRUPTS_DISABLED`。`poll_budget` 是有界迭代次数，不是校准毫秒。TIMEOUT 表示完成状态未知，不能盲目重发；迟到的 TI 中断可释放忙碌状态，否则需要诊断后重新初始化。RX ISR 只做字节入队，帧边界和命令解释留给主循环。固定 32 字节队列满时设置溢出标志。

DS18B20 先调用 `DS18B20_StartConversion()` 并检查状态，在外部测量经过至少 750 ms 后调用 `DS18B20_ReadTemperature(elapsed_ms, &temperature_c)`。按 [Analog Devices DS18B20 数据手册](https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf)核对 9 字节 scratchpad/CRC。未校准的阻塞延时接口已移除；底层 1-Wire 时序仍须 Keil 目标构建和板端测量。

## 主机逻辑测试

`peripheral_policy.c`保存不依赖寄存器的判断逻辑，C51 修正版与主机测试共用同一实现：

- RTC 时、分、秒的 BCD 范围检查；
- 按键扫描周期和值域检查；
- DS18B20 750 ms 转换条件、合法 0℃、负温度、CRC 错误和输出保持；
- UART 环形队列索引回绕。
- UART 有界等待、超时、迟到完成和 RX/TX 并发状态。
- PCF8591 通道、前次转换字节、当前字节、ACK/NACK 和失败路径。

```powershell
gcc -std=c11 -Wall -Wextra -Werror -pedantic `
  peripheral_policy.c tests/test_peripheral_policy.c `
  -I . -o peripheral-policy-test.exe
./peripheral-policy-test.exe
```

GitHub Actions 使用 GCC 与 Clang 执行上述 Host 测试。它不构成 Keil C51 Target Build、CT107D 板端验证或电气时序证据。

## 板端复测项

Keil C51 与 CT107D 复测包括：

1. I²C 空闲、SCL/SDA 占用、NACK、STOP 失败、PCF8591 首字节及当前通道结果；恢复策略须结合 CT107D 电路核实。
2. DS18B20 启动转换到读取暂存器之间的时间间隔。
3. RTC 正常时间不被启动流程覆盖，CH 置位或 BCD 越界时才写默认值。
4. UART 连续输入时的队列溢出、帧恢复、TX 超时和迟到 TI 中断。

基线接口与修正版差异已逐项核对；构建输出、逻辑波形和传感器读数应与具体复测条件一起记录。
