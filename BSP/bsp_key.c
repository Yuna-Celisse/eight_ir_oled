#include "bsp_key.h"

/**
 * @brief 读取K1按键状态
 * @return 0-按下, 1-释放
 * @note 按键使用下拉电阻，按下时为高电平
 */
uint8_t KEY_Read(void)
{
    return DL_GPIO_readPins(KEY_PORT, KEY_K1_PIN) ? KEY_PRESSED : KEY_RELEASED;
}

/**
 * @brief 检测 KEY1（下拉，按下时高电平）
 * @return 1-按下, 0-释放
 */
uint8_t KEY1_IsPressed(void)
{
    return DL_GPIO_readPins(KEY_PORT, KEY_K1_PIN) != 0;
}

/**
 * @brief 检测 KEY2（上拉，按下时低电平）
 * @return 1-按下, 0-释放
 */
uint8_t KEY2_IsPressed(void)
{
    // 如果有KEY2，使用相应的PORT和PIN
    // return DL_GPIO_readPins(KEY_PORT, KEY_K2_PIN) == 0;
    return 0;  // 当前项目可能只有一个按键
}

/**
 * @brief 检测 KEY3（上拉，按下时低电平）
 * @return 1-按下, 0-释放
 */
uint8_t KEY3_IsPressed(void)
{
    // 如果有KEY3，使用相应的PORT和PIN
    // return DL_GPIO_readPins(KEY_PORT, KEY_K3_PIN) == 0;
    return 0;  // 当前项目可能只有一个按键
}

/**
 * @brief K1按键扫描（带消抖）
 * @return 0-未按下, 1-按下
 */
uint8_t KEY_Scan(void)
{
    static uint8_t key_state = 1;  // 1=释放，0=按下
    
    // 当前按键状态
    uint8_t current = KEY_Read();
    
    // 检测按键按下（从释放到按下的跳变）
    if (current == KEY_PRESSED && key_state == KEY_RELEASED)
    {
        key_state = KEY_PRESSED;
        return 1;  // 返回按键按下
    }
    else if (current == KEY_RELEASED)
    {
        key_state = KEY_RELEASED;
    }
    
    return 0;  // 无按键按下
}

/**
 * @brief 翻转 LED 状态
 */
void LED_Toggle(void)
{
    DL_GPIO_togglePins(LED_PORT, LED_MCU_PIN);
}

/**
 * @brief GROUP1 中断服务函数中的按键处理逻辑
 * @note 此函数应在bsp_encoder.c的GROUP1_IRQHandler中调用
 */
void KEY_IRQ_Handler(void)
{
    // 检查按键是否被按下（高电平触发后变为高电平）
    if(DL_GPIO_readPins(KEY_PORT, KEY_K1_PIN) > 0)
    {
        // 软件消抖延时
        for (int j = 0; j < 80000; j++)  
        {
            __asm__("nop"); // 空操作延时
        }
        
        // 再次确认按键状态
        if(DL_GPIO_readPins(KEY_PORT, KEY_K1_PIN) > 0)
        {
            // 翻转LED状态
            LED_Toggle();
        }
    }
}
