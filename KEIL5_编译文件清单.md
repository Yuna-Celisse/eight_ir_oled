# Keil5 编译文件清单

## 📋 必须勾选参与编译的文件

### 1️⃣ lib 组（库文件）
```
✅ driverlib.a
   路径: ..\..\mspm0_sdk_2_08_00_03\source\ti\driverlib\lib\keil\m0p\mspm0g1x0x_g3x0x\driverlib.a
   说明: TI官方驱动库，必须包含
```

### 2️⃣ Source 组（核心源文件）
```
✅ startup_mspm0g350x_uvision.s
   路径: keil\startup_mspm0g350x_uvision.s
   说明: 启动文件（汇编），必须包含

✅ ti_msp_dl_config.c
   路径: ti_msp_dl_config.c
   说明: 系统配置文件，由SysConfig自动生成，必须包含

✅ main.c
   路径: main.c
   说明: 主程序文件，包含改进后的完整逻辑

⚪ empty.syscfg
   路径: empty.syscfg
   说明: 配置文件，不参与编译但用于生成代码

⚪ ti_msp_dl_config.h
   路径: ti_msp_dl_config.h
   说明: 头文件，不需要勾选

⚪ AllHeader.h
   路径: BSP\AllHeader.h
   说明: 头文件，不需要勾选
```

### 3️⃣ BSP 组（板级支持包）
```
✅ delay.c
   路径: BSP\delay.c
   说明: 延时函数

✅ usart0.c
   路径: BSP\usart0.c
   说明: 串口通信

✅ bsp_beep_led.c
   路径: BSP\bsp_beep_led.c
   说明: 蜂鸣器和LED控制（重要！用于蜂鸣器响应）

✅ oled.c
   路径: BSP\OLED\oled.c
   说明: OLED显示驱动（重要！用于时间显示-20秒）

✅ bsp_ir.c
   路径: BSP\bsp_ir.c
   说明: 红外传感器处理

✅ bsp_encoder.c
   路径: BSP\Motor\bsp_encoder.c
   说明: 编码器读取

✅ bsp_motor.c
   路径: BSP\Motor\bsp_motor.c
   说明: 电机驱动

✅ bsp_timer.c
   路径: BSP\bsp_timer.c
   说明: 定时器（重要！用于精确计时）
```

### 4️⃣ APP 组（应用层）
```
✅ app_motor.c
   路径: APP\app_motor.c
   说明: 电机控制应用层

✅ bsp_PID_motor.c
   路径: APP\bsp_PID_motor.c
   说明: 电机PID控制

✅ app_irtracking_eight.c
   路径: APP\app_irtracking_eight.c
   说明: 8路循线算法（重要！核心巡线逻辑）
```

---

## ⚠️ 不需要勾选的文件（已在项目中但不参与编译）

### 备份文件（.bak）
- ❌ app_ir_track.c.bak
- ❌ app_motor.h.bak
- ❌ app_rgb.c.bak
- ❌ app_speech.c.bak
- ❌ app_speech.h.bak
- ❌ bsp_ir.h.bak
- ❌ bsp_timer.c.bak
- ❌ delay.c.bak
- ❌ oled.h.bak
- ❌ oledfont.h.bak
- ❌ bsp_motor.c.bak

### 未使用的功能模块
- ❌ app_irremote.c （红外遥控，当前未启用）
- ❌ app_ir_track.c （旧版4路循线，已被8路替代）
- ❌ app_rgb.c （RGB灯效，如果硬件没有可不编译）
- ❌ app_speech.c （语音模块，当前未使用）
- ❌ bsp_oled.c （另一个OLED实现，使用oled.c即可）
- ❌ oled_fonts.c （字体文件，如果不需要可不编译）
- ❌ bsp_iic.c （软件IIC，如果使用硬件IIC可不编译）

### 所有头文件（.h）
头文件会被自动包含，不需要在项目中勾选编译

---

## 📊 编译配置检查清单

### Include Path（包含路径）✅
确保以下路径已添加到编译器的Include Path中：
```
..\..\mspm0_sdk_2_08_00_03\source\third_party\CMSIS\Core\Include
..\..\mspm0_sdk_2_08_00_03\source
..\..\mspm0_sdk_2_08_00_03\source\ti\driverlib\m0p\sysctl
..\..\eight_ir_oled
..\BSP
..\BSP\OLED
..\BSP\Motor
..\APP
```

### Preprocessor Defines（预处理器定义）✅
```
__MSPM0G3507__
```

### Linker Script（链接脚本）✅
```
mspm0g3507.sct
```

---

## 🎯 快速配置步骤

### 方法1：使用现有项目文件（推荐）
1. 打开Keil5
2. 点击 `Project` → `Open Project`
3. 选择文件：`d:\TI\eight_ir_oled\keil\eight_ir_oled.uvprojx`
4. 项目会自动加载所有已配置的文件

### 方法2：手动添加文件
如果需要添加新文件到项目：

1. 在Project窗口中右键点击对应的组（lib/Source/BSP/APP）
2. 选择 `Add Existing Files to Group`
3. 浏览并选择要添加的.c或.s文件
4. 点击 `Add` 然后 `Close`

### 方法3：设置文件编译选项
对于某个特定文件：
1. 在Project窗口中右键点击该文件
2. 选择 `Options for File...`
3. 在弹出窗口中勾选/取消勾选 `Include in Target Build`

---

## 🔍 验证编译配置

### 检查文件是否参与编译
- ✅ 已包含：文件前有复选框且被勾选
- ❌ 不包含：文件前复选框未勾选或显示为灰色
- 📁 仅浏览：头文件一般只用于浏览，不影响编译

### 编译测试
```
1. 点击 Project → Build Target (F7)
2. 查看 Build Output 窗口
3. 确认以下信息：
   - 0 Error(s)
   - 编译的.c文件数量应为 14 个（不含头文件）
```

---

## 📝 文件统计

| 类型 | 数量 | 说明 |
|-----|------|------|
| 库文件(.a) | 1 | driverlib.a |
| 启动文件(.s) | 1 | startup_mspm0g350x_uvision.s |
| C源文件(.c) | 12 | 核心功能模块 |
| 总计 | 14 | 参与编译的文件 |

---

## ⚡ 常见问题

### Q1: 编译时找不到头文件？
**A**: 检查 `C/C++` 选项卡中的 `Include Paths` 是否正确配置

### Q2: 链接时出现 undefined reference？
**A**: 检查对应的.c文件是否已添加到项目并勾选编译

### Q3: 编译后文件过大？
**A**: 在 `C/C++` 选项卡中：
- 设置 `Optimization` 为 `Level 3 (-O3)`
- 确保未包含不必要的调试信息

### Q4: 某个功能不工作？
**A**: 确认以下文件已正确编译：
- 按键启动：main.c
- 蜂鸣器响应：main.c, bsp_beep_led.c
- OLED计时：main.c, bsp_timer.c, oled.c
- 循线：app_irtracking_eight.c

---

## 🎓 额外说明

### RGB灯效（可选-5秒）
如果硬件有RGB灯珠且想实现RGB灯效：
1. 查找并添加 `bsp_RGB.c` 到BSP组（需要从示例代码复制）
2. 确保在main.c中 `#define Control_RGB_ALL` 已定义
3. 重新编译项目

### 调试模式
编译时选择 `Debug` 模式可以：
- 保留更多调试信息
- 支持单步调试
- 查看变量值

发布时选择 `Release` 模式：
- 优化代码大小
- 提高运行速度
- 减少内存占用

---

**最后更新**: 2025年12月16日  
**项目版本**: v1.0  
**编译器**: ARM Compiler 6.22 (ARMCLANG)
