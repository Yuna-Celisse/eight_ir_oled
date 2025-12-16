# 配置和调试指南

## 必要的硬件配置

### 1. 按键配置 (在 ti_msp_dl_config.c 或 empty.syscfg 中)

需要添加一个GPIO输入配置用于按键：

```c
// 建议使用 GPIOA PIN_18
// 配置为：
// - Direction: Input
// - Internal Pull: Pull-up
// - Interrupt: 可选（如果需要中断方式）
```

**在代码中需要修改的位置：**
[main.c](main.c#L110-L114) 中的 `KEY_IsPressed()` 函数需要根据实际配置的引脚进行调整：

```c
uint8_t KEY_IsPressed(void) {
    // 修改为你实际使用的端口和引脚
    return (DL_GPIO_readPins(GPIOA, DL_GPIO_PIN_18) == 0);
}
```

### 2. 8路红外传感器配置

确保在 `ti_msp_dl_config.h` 中定义了以下宏：
- `IR_Track_PORT` - 红外传感器的GPIO端口
- `IR_Track_X1_PIN` 到 `IR_Track_X8_PIN` - 8个传感器引脚

当前使用的是：
- [app_irtracking_eight.c](APP/app_irtracking_eight.c) 中的 `deal_IRdata()` 函数
- 通过 `IR_Data_number[0-7]` 数组读取传感器数据

### 3. OLED配置

OLED使用I2C接口，需要配置：
- `OLED_PORT` - OLED的GPIO端口
- `OLED_SCL_PIN` - I2C时钟引脚
- `OLED_SDA_PIN` - I2C数据引脚

### 4. 蜂鸣器配置

确保在 `ti_msp_dl_config.h` 中定义了：
- `BEEP_PORT` - 蜂鸣器GPIO端口
- `BEEP_Buzzer_PIN` - 蜂鸣器引脚

### 5. 定时器配置

20ms定时器必须正确配置：
- 在 SysConfig 中配置一个定时器，名称为 `TIMER_20ms_INST`
- 设置为20ms周期中断
- 使能中断

## 编译配置

### Keil uVision

1. 打开项目：[keil/eight_ir_oled.uvprojx](keil/eight_ir_oled.uvprojx)
2. 确保所有源文件都已添加到项目中：
   - main.c
   - BSP文件夹下的所有.c文件
   - APP文件夹下的所有.c文件
   - ti文件夹下的驱动库文件

### 代码检查清单

在编译和烧录之前，请检查：

1. ✅ **AllHeader.h 包含路径**
   - 确保 BSP 文件夹在包含路径中

2. ✅ **按键引脚定义**
   - 在 [main.c](main.c#L110) 修改 `KEY_IsPressed()` 函数中的引脚定义

3. ✅ **传感器数据源**
   - 确认 `IR_Data_number[]` 数组在哪里更新
   - 通常在定时器中断或传感器读取函数中

4. ✅ **延时函数**
   - 确保 `delay_ms()` 函数已正确实现（在 delay.c 中）

5. ✅ **RGB功能（可选）**
   - 如果没有RGB硬件，预编译指令会自动跳过
   - 如果有RGB，需要实现 `Control_RGB_ALL()` 函数

## 调试步骤

### 第一步：基础功能测试

```c
// 1. 测试OLED显示
OLED_Clear();
OLED_ShowString(0, 0, "TEST OK", 8, 1);
OLED_Refresh();

// 2. 测试按键
while(1) {
    if(KEY_IsPressed()) {
        OLED_ShowString(0, 16, "KEY PRESSED", 8, 1);
        OLED_Refresh();
    }
}

// 3. 测试蜂鸣器
Beep_Times(3);  // 应该响3次

// 4. 测试传感器读取
for(int i = 0; i < 8; i++) {
    OLED_ShowNum(i*16, 32, IR_Data_number[i], 1, 8, 1);
}
OLED_Refresh();
```

### 第二步：状态机测试

1. 上电后应显示 "Press KEY Start"
2. 按下按键后应该：
   - 蜂鸣器响1次
   - 切换到运行状态
   - 开始显示时间

### 第三步：宽线检测测试

在不同位置测试宽线检测：

```c
// 在 Check_Wide_Line() 函数中添加调试输出
OLED_ShowNum(100, 16, white_count, 2, 8, 1);  // 显示检测到白线的传感器数量
OLED_ShowNum(100, 24, beep_point_count, 1, 8, 1);  // 显示当前点位计数
```

### 第四步：完整赛道测试

1. 确认起点检测正常
2. 确认各B点位蜂鸣器响应正确
3. 确认终点能够正确停车
4. 检查OLED时间显示是否准确

## 常见问题解决

### 问题1：编译错误 "无法打开源文件"

**解决方案：**
- 在 Keil 中：Project → Options → C/C++ → Include Paths
- 添加以下路径：
  - `.\BSP`
  - `.\BSP\OLED`
  - `.\APP`
  - `.\ti`

### 问题2：按键无响应

**检查项：**
1. 引脚配置是否正确（输入、上拉）
2. `KEY_IsPressed()` 函数中的端口和引脚是否匹配
3. 按键硬件是否正常（用万用表测试）

### 问题3：宽线检测不准确

**调整方案：**
1. 修改 [main.c](main.c#L135) 中的阈值：
   ```c
   if(white_count >= 6)  // 尝试改为 5 或 7
   ```
2. 调整 `wide_line_hold` 的值（防抖时间）
3. 检查传感器高度和角度

### 问题4：时间显示不准确

**检查项：**
1. 定时器周期是否确实为20ms
2. 在 [bsp_timer.c](BSP/bsp_timer.c#L24) 检查是否正确累加时间
3. 验证 `system_state` 变量值

### 问题5：蜂鸣器响应次数错误

**检查项：**
1. 查看 `beep_point_count` 的值是否正确累加
2. 确认 [main.c](main.c#L142-L151) 中的点位判断逻辑
3. 根据实际赛道布局调整点位序号

## 性能优化建议

### 速度调优

在 [app_irtracking_eight.c](APP/app_irtracking_eight.c) 中：

```c
#define IRR_SPEED 200  // 增加这个值可以提高速度，但可能降低稳定性
```

### PID参数调优

```c
#define IRTrack_Trun_KP (200)  // 比例系数，影响响应速度
#define IRTrack_Trun_KI (0.02) // 积分系数，影响稳态误差
#define IRTrack_Trun_KD (0)    // 微分系数，影响系统阻尼
```

调优方法：
1. 先调整KP，使小车能够基本跟随线
2. 如果出现持续偏差，适当增加KI
3. 如果出现振荡，增加KD或减小KP

## 赛前检查清单

- [ ] 电池电量充足
- [ ] 所有传感器工作正常
- [ ] 按键功能正常
- [ ] OLED显示清晰
- [ ] 蜂鸣器能正常发声
- [ ] 电机转动正常
- [ ] 在测试赛道上完整测试一次
- [ ] 确认时间显示准确（对比秒表）
- [ ] 确认所有宽线都能正确识别
- [ ] 确认终点能可靠停车

---
**提示**: 如果遇到其他问题，可以通过串口输出调试信息：
```c
printf("Debug: beep_count=%d, white_count=%d\n", beep_point_count, white_count);
```
