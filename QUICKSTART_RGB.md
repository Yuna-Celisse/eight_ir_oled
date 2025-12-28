# 🚀 RGB循迹功能 - 快速开始指南

## 一键启动

### 步骤1: 编译项目
```
打开: keil/eight_ir_oled.uvprojx
操作: 按 F7 编译
```

### 步骤2: 烧录程序
```
操作: 按 F8 下载到MCU
```

### 步骤3: 运行测试
```
1. 按K1键启动系统
2. 观察RGB灯效果：
   - 白色背景 → RGB显示白色 ⚪
   - 黑线上方 → RGB显示红色 🔴
```

## 🎯 功能说明

### RGB指示含义
| 颜色 | 状态 | 说明 |
|------|------|------|
| ⚪ 白色 | 正常 | 传感器在白色背景上 |
| 🔴 红色 | 检测到线 | 任意传感器检测到黑线 |

### 工作原理
```
8路传感器 → 检测黑线 → RGB自动变色
   ↓              ↓            ↓
(x1~x8)    (line_detected)  (红/白)
```

## 📌 硬件连接

### RGB LED (WS2812)
- **数据线**: GPIOB PIN_10
- **电源**: 5V
- **地线**: GND
- **数量**: 2个LED

### 连接示意
```
MCU (MSPM0G3507)      WS2812 RGB LED
─────────────────    ──────────────
GPIOB PIN_10  ────→  DIN (数据输入)
5V            ────→  VCC
GND           ────→  GND
```

## ⚙️ 配置选项

### 如需更改引脚
编辑 `BSP/bsp_RGB.h`:
```c
#define RGB_PORT          GPIOB      // ← 修改端口
#define RGB_WQ2812_PIN    DL_GPIO_PIN_10  // ← 修改引脚
```

### 如需更改颜色
编辑 `APP/app_rgb.c` 中的 `RGB_LineTracking_Control()`:
```c
void RGB_LineTracking_Control(uint8_t line_detected)
{
    if(line_detected) {
        Control_RGB_ALL(Red_RGB);     // ← 改成其他颜色
    } else {
        Control_RGB_ALL(White_RGB);   // ← 改成其他颜色
    }
}
```

可用颜色：`Red_RGB`, `Green_RGB`, `Blue_RGB`, `Yellow_RGB`, `Purple_RGB`, `Cyan_RGB`, `White_RGB`, `OFF`

## 🔍 故障排查

### RGB不亮
```
检查项：
☐ 硬件连接是否正确
☐ 5V电源是否供应
☐ 引脚定义是否匹配
☐ 程序是否正确烧录
```

### 颜色异常
```
检查项：
☐ 系统时钟是否为80MHz
☐ GND是否共地
☐ 数据线是否有干扰
```

### 响应不及时
```
原因：正常情况下响应极快（<1ms）
如仍觉得慢，检查主循环delay
```

## 📊 状态指示

### 启动流程
1. 系统初始化
2. RGB引脚配置 ✓
3. OLED显示"Press K1 Start"
4. **按K1启动** ← 此时开始RGB指示
5. RGB开始工作

### 运行状态
```
小车未启动 → RGB不工作
按K1启动 → RGB实时指示
小车行驶 → RGB跟随传感器变化
```

## 💡 高级功能

### 自定义颜色
在 `bsp_RGB.h` 中添加新颜色：
```c
#define ORANGE  0xffa500  // 橙色
#define PINK    0xff69b4  // 粉色
```

### 闪烁效果
修改 `Control_RGB_ALL()` 添加延时：
```c
Control_RGB_ALL(Red_RGB);
delay_ms(100);
Control_RGB_ALL(OFF);
delay_ms(100);
```

### 渐变效果
调用 `set_led_color()` 逐步改变亮度：
```c
for(int i = 0; i < 255; i++) {
    set_led_color(0, i, 0, 0);  // 红色渐亮
    ws2812_send();
    delay_ms(5);
}
```

## 📖 相关文档

- `RGB_README.md` - 详细使用说明
- `RGB_INTEGRATION_REPORT.md` - 完整集成报告

## 🆘 需要帮助？

### 常见问题
1. **Q: RGB灯一直不变色？**  
   A: 确认已按K1启动系统，未启动时RGB不工作。

2. **Q: 颜色不纯正？**  
   A: 检查系统时钟是否为80MHz，或调整延时参数。

3. **Q: 只有一个灯亮？**  
   A: 正常，代码控制两个LED同时显示相同颜色。

4. **Q: 想要更多颜色？**  
   A: 在 `bsp_RGB.h` 中定义新颜色值（24位RGB）。

---

**🎉 享受您的RGB循迹小车！**

```
     ⚪⚪        正常行驶
    ║  ║
   ╔════╗
   ║ 🚗 ║
   ╚════╝
   
     🔴🔴        检测到线
    ║  ║
   ╔════╗
   ║ 🚗 ║
   ╚════╝
  ━━━━━━━━      黑线
```
