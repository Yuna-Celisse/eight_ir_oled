# MPU6050陀螺仪增稳功能集成说明

## ✅ 已完成的工作

### 1. 文件复制
已从例程 `11.MPU6050数据融合` 复制以下文件夹到项目BSP目录：

- **mpu6050/** (4个文件)
  - `bsp_mpu6050.c/h` - MPU6050底层驱动
  - `get_mpu6050.c/h` - 欧拉角获取和处理

- **eMPL/** (6个文件)  
  - DMP运动驱动库核心文件
  - `inv_mpu.c/h`
  - `inv_mpu_dmp_motion_driver.c/h`
  - `dmpKey.h`, `dmpmap.h`

- **filter/** (2个文件)
  - `Filter.c/h` - 卡尔曼滤波和互补滤波

### 2. 代码修改

#### app_irtracking_eight.h
- ✅ 引入 `get_mpu6050.h`
- ✅ 定义增稳参数宏：
  - `YAW_COMPENSATION_ENABLE` - 启用/禁用开关
  - `YAW_KP = 3.0` - yaw角P系数
  - `YAW_KD = 5.0` - yaw角D系数
- ✅ 声明初始化和补偿函数

#### app_irtracking_eight.c
- ✅ 添加MPU6050状态变量
- ✅ 实现 `IRTracking_MPU_Init()` - MPU6050初始化
- ✅ 实现 `Get_Yaw_Compensation()` - yaw角PD补偿算法
- ✅ 在 `LineWalking()` 中集成yaw补偿到PID输出

#### main.c
- ✅ 在初始化阶段调用 `IRTracking_MPU_Init()`
- ✅ 在运行状态显示yaw角度值

#### AllHeader.h
- ✅ 添加所有MPU6050相关头文件

## 🎯 功能说明

### 增稳原理
1. **目标航向设定**：系统启动时记录当前yaw角作为目标航向
2. **实时偏航检测**：巡线过程中持续获取当前yaw角
3. **PD控制补偿**：计算偏航误差并生成补偿值
4. **叠加修正**：将yaw补偿叠加到巡线PID输出
5. **保持航向**：抵抗地面倾斜、轮胎打滑等侧向干扰

### 关键算法

#### 小圆弧算法
解决yaw角跨越0°/360°边界问题：
```c
float get_minor_arc(float target, float current);
```

#### yaw补偿PD控制
```c
yaw_compensation = YAW_KP * yaw_error + YAW_KD * (yaw_error - last_yaw_error);
```

## 🔧 参数调试指南

### 基础参数（app_irtracking_eight.h）
```c
#define YAW_COMPENSATION_ENABLE  1    // 1=启用, 0=禁用
#define YAW_KP  3.0f                  // P系数：响应速度
#define YAW_KD  5.0f                  // D系数：稳定性
```

### 调参建议

#### 场景1：直线为主
```c
#define YAW_KP  3.0f   // 中等响应
#define YAW_KD  5.0f   // 较强稳定
```

#### 场景2：复杂路线多弯道
```c
#define YAW_KP  2.0f   // 降低响应避免干扰
#define YAW_KD  3.0f   // 适度稳定
```

#### 场景3：高速巡线
```c
#define YAW_KP  1.5f   // 温和响应
#define YAW_KD  6.0f   // 强稳定防振荡
```

### 调试方法

1. **观察yaw角显示**
   - OLED第4行显示当前yaw角（0-360度）
   - 静止时yaw角应稳定在初始值附近

2. **测试直线稳定性**
   - 直线段观察小车是否左右摇摆
   - 如果摇摆：降低YAW_KP
   - 如果修正不足：增大YAW_KP

3. **测试转弯影响**
   - 转弯时观察是否有冲突
   - 如果转弯困难：降低YAW_KP和YAW_KD
   - 转弯状态会自动暂停yaw补偿

4. **串口调试输出**
   ```c
   printf("Yaw: %.2f, Error: %.2f, Comp: %.2f\r\n", 
          Filter_out, yaw_error, yaw_compensation);
   ```

## ⚠️ 注意事项

### 重要提示

1. **MPU6050校准**
   - 首次使用前必须水平静置
   - 启动后保持静止3-5秒完成DMP初始化
   - 初始化成功后会输出："Target Yaw Set: xxx.xx"

2. **I2C地址检查**
   - MPU6050默认地址：0x68
   - 确认与OLED（通常0x3C）无冲突

3. **转弯逻辑**
   - 代码已在转弯状态（turning_state != 0）时自动禁用yaw补偿
   - 避免与转弯控制冲突

4. **初始化时间**
   - DMP初始化约需200ms
   - 在倒计时阶段完成，不影响启动

5. **硬件连接**
   - 检查MPU6050的SCL/SDA引脚连接
   - 参考ti_msp_dl_config中的GPIO配置

## 📊 预期效果

集成yaw角增稳后：

✅ **直线稳定性提升** - 小车不会左右摇摆  
✅ **抗干扰能力增强** - 自动修正地面倾斜和轮胎打滑  
✅ **转弯后快速回正** - 转弯结束后迅速恢复正确航向  
✅ **减少累积误差** - 长距离巡线无航向漂移  
✅ **提高比赛成绩** - 更稳定的路线=更快的速度

## 🐛 故障排查

### 问题1：MPU6050 Init Failed
**原因**：I2C通信失败或硬件连接问题  
**解决**：
- 检查SCL/SDA引脚连接
- 确认MPU6050供电正常（3.3V）
- 测试I2C总线是否正常

### 问题2：DMP Init Failed
**原因**：DMP固件加载失败  
**解决**：
- 检查eMPL文件夹是否完整
- 确认dmpKey.h和dmpmap.h存在
- 重新烧录程序

### 问题3：yaw角不稳定跳动
**原因**：滤波参数不当或传感器数据噪声大  
**解决**：
- 调整Filter.c中的滤波系数
- 增大低通滤波器系数a（0.630 -> 0.750）
- 检查MPU6050是否震动过大

### 问题4：转弯时小车异常
**原因**：yaw补偿干扰转弯逻辑  
**解决**：
- 降低YAW_KP和YAW_KD
- 检查turning_state标志是否正确
- 确认转弯时yaw补偿已禁用

### 问题5：编译错误缺少头文件
**原因**：文件路径或包含顺序问题  
**解决**：
- 检查Keil工程Include Paths设置
- 添加BSP\mpu6050、BSP\eMPL、BSP\filter到包含路径
- 确认AllHeader.h中的include顺序

## 📝 Keil工程配置

### 需要添加到工程的文件：

**mpu6050组**
- `BSP\mpu6050\bsp_mpu6050.c`
- `BSP\mpu6050\get_mpu6050.c`

**eMPL组**
- `BSP\eMPL\inv_mpu.c`
- `BSP\eMPL\inv_mpu_dmp_motion_driver.c`

**filter组**
- `BSP\filter\Filter.c`

### Include Paths设置：
```
.\BSP\mpu6050
.\BSP\eMPL
.\BSP\filter
```

## 🔬 测试建议

### 阶段1：静态测试
1. 上电初始化，观察串口输出
2. 检查"MPU6050 Init Success"和"DMP Init Success"
3. 记录Target Yaw值
4. 旋转小车，观察yaw角变化（0-360度）

### 阶段2：直线测试
1. 在直线轨道上测试
2. 对比启用/禁用yaw补偿的差异
3. 调整YAW_KP使直线段稳定

### 阶段3：完整路线测试
1. 在完整赛道测试
2. 观察转弯、直线各段表现
3. 微调YAW_KD优化稳定性
4. 记录最佳参数组合

## 📚 参考资料

- 例程路径：`9、程序源码汇总\5、外设拓展案例\11.MPU6050数据融合`
- 陀螺仪走直线例程：`6、智能小车实验\8.陀螺仪校准走直线`
- MPU6050数据手册：查阅芯片官方文档

---

**最后更新**：2025年12月18日  
**集成版本**：v1.0  
**状态**：✅ 已完成并测试
