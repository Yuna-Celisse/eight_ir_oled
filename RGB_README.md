# RGB循迹功能说明

## 功能描述
本项目已集成RGB LED功能，用于指示循迹传感器的检测状态。

### RGB颜色指示
- 🔴 **红色** - 检测到黑线（传感器检测到线）
- ⚪ **白色** - 正常状态（传感器在白色背景上）

## 硬件连接
- **RGB LED类型**: WS2812
- **默认引脚**: GPIOB PIN_10
- **LED数量**: 2个

## 代码结构

### 驱动文件
- `BSP/bsp_RGB.h` - RGB驱动头文件
- `BSP/bsp_RGB.c` - RGB驱动实现（WS2812协议）

### 应用文件
- `APP/app_rgb.h` - RGB应用层头文件
- `APP/app_rgb.c` - RGB应用层实现，包含线检测控制函数

### 集成位置
- `main.c` - 主循环中调用 `RGB_LineTracking_Control(line_detected)`

## 使用说明

### 1. 硬件配置
如果使用不同的GPIO引脚，请修改 `BSP/bsp_RGB.h` 中的引脚定义：
```c
#define RGB_PORT          GPIOB
#define RGB_WQ2812_PIN    DL_GPIO_PIN_10
```

### 2. SysConfig配置（推荐）
在TI SysConfig工具中配置RGB GPIO：
- 端口：GPIOB
- 引脚：PIN_10
- 模式：强推挽输出

### 3. 初始化
RGB引脚在 `main.c` 的 `SYSCFG_DL_init()` 后自动初始化。

### 4. 控制逻辑
主循环中自动根据传感器状态更新RGB颜色：
```c
line_detected = (x1 || x2 || x3 || x4 || x5 || x6 || x7 || x8);
RGB_LineTracking_Control(line_detected);
```

## API接口

### RGB_LineTracking_Control()
```c
void RGB_LineTracking_Control(uint8_t line_detected);
```
- **参数**: `line_detected` - 1表示检测到黑线，0表示正常
- **功能**: 自动设置RGB颜色

### Control_RGB_ALL()
```c
void Control_RGB_ALL(RGB_Color_t color);
```
- **参数**: `color` - RGB颜色枚举值
- **功能**: 设置所有RGB LED的颜色
- **可用颜色**: Red_RGB, Green_RGB, Blue_RGB, Yellow_RGB, Purple_RGB, Cyan_RGB, White_RGB, OFF

## 注意事项

1. **时序要求**: WS2812需要精确的时序控制，发送数据时会临时关闭中断
2. **CPU频率**: 代码针对80MHz优化，如果使用其他频率需要调整 `delay_0_25us()` 函数
3. **延迟**: RGB更新非常快速，不会阻塞主循环

## 编译配置

确保在Keil或CCS项目中包含以下文件：
- `BSP/bsp_RGB.c`
- `APP/app_rgb.c`

## 故障排除

### RGB不亮
1. 检查硬件连接
2. 确认GPIO引脚配置正确
3. 检查电源供电（WS2812需要5V）

### 颜色不准确
1. 检查系统时钟频率
2. 调整 `bsp_RGB.c` 中的延时参数

### 闪烁或不稳定
1. 确保电源稳定
2. 添加去耦电容
3. 缩短信号线长度

## 扩展功能

可以通过修改 `app_rgb.c` 实现更多功能：
- 呼吸灯效果
- 渐变效果
- 自定义颜色模式
- 根据速度改变颜色

---
*最后更新: 2025年12月28日*
