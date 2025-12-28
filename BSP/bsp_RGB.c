#include "bsp_RGB.h"

#define LED_NUM 1          // LED数量
#define DELAY_0H 3         // 0码高电平时间(us)
#define DELAY_0L 9         // 0码低电平时间(us)
#define DELAY_1H 6         // 1码高电平时间(us)
#define DELAY_1L 6         // 1码低电平时间(us)
#define RESET_DELAY 50     // 复位时间(us)

unsigned char LedsArray[WS2812_MAX * 3];      // 保存灯珠颜色数据存储数组
unsigned int  ledsCount   = WS2812_NUMBERS;   // 保存实际彩灯默认个数
unsigned int  nbLedsBytes = WS2812_NUMBERS*3; // 保存实际彩灯颜色数据个数
unsigned int colorIndex = 0;

// 延时0.25us
void delay_0_25us(void)
{
    // 根据系统时钟调整，80MHz时使用13
    for(int i = 0; i < 13; i++);
}

/******************************************************************
 * 函 数 名：rgb_SetColor
 * 功能说明：设置彩灯颜色
 * 形    参：LedId控制的第几个灯  color颜色代码
 * 返 回 值：无
******************************************************************/
void rgb_SetColor(unsigned char LedId, unsigned long color)
{
    if(LedId >= ledsCount)
    {
        return;    // 避免溢出
    }
    // WS2812使用GRB顺序
    LedsArray[LedId * 3]     = (color>>8)&0xff;   // G
    LedsArray[LedId * 3 + 1] = (color>>16)&0xff;  // R
    LedsArray[LedId * 3 + 2] = (color>>0)&0xff;   // B
}

/******************************************************************
 * 函 数 名：rgb_SetRGB
 * 功能说明：设置彩灯颜色(用原色配置)
 * 形    参：LedId控制的第几个灯 red颜色代码  green颜色代码  blue颜色代码
 * 返 回 值：无
******************************************************************/
void rgb_SetRGB(unsigned char LedId, unsigned long red, unsigned long green, unsigned long blue)
{
    unsigned long Color = red<<16 | green<<8 | blue;
    rgb_SetColor(LedId, Color);
}

/******************************************************************
 * 函 数 名：rgb_SendArray
 * 功能说明：发送彩灯数据
 * 形    参：无
 * 返 回 值：无
******************************************************************/
void rgb_SendArray(void)
{
    unsigned int i;
    
    // 先发送复位
    RGB_PIN_L();
    delay_us(300);
    
    // 关闭所有中断确保时序准确
    __disable_irq();
    
    // 发送数据
    for(i=0; i<nbLedsBytes; i++)
        Ws2812b_WriteByte(LedsArray[i]);
    
    // 发送复位信号
    RGB_PIN_L();
    for(volatile int j=0; j<5000; j++);  // 长复位
    
    // 重新开启中断
    __enable_irq();
}

/******************************************************************
 * 函 数 名：RGB_LED_Reset
 * 功能说明：复位ws2812
 * 形    参：无
 * 返 回 值：无
******************************************************************/
void RGB_LED_Reset(void)
{
    RGB_PIN_L();              
    delay_us(285);
}

/******************************************************************
 * 函 数 名：Ws2812b_WriteByte
 * 功能说明：向WS2812写入单字节数据
 * 形    参：byte写入的字节数据
 * 返 回 值：无
 * 备    注：1码时序 = 高电平580ns~1us    后低电平220ns~420ns
 *           0码时序 = 高电平220ns~380ns  后低电平580ns~1us
******************************************************************/
void Ws2812b_WriteByte(unsigned char byte)
{
    for(int i = 0; i < 8; i++)
    {
        if(byte & (0x80 >> i))  // 当前位为1
        { 
            RGB_PIN_H();
            delay_us(1);  // 高电平1us
            RGB_PIN_L();
            delay_0_25us();  // 低电平0.25us
        }
        else  // 当前位为0
        {
            RGB_PIN_H();
            delay_0_25us();  // 高电平0.25us
            RGB_PIN_L();
            delay_us(1);  // 低电平1us
        }
    }
}
  
void ws2812_send(void)
{
    __disable_irq(); // 关闭中断确保时序准确
    
    for(int i = 0; i < WS2812_MAX*3; i++) {
        uint8_t byte_val = LedsArray[i];
        
        for(int bit = 7; bit >= 0; bit--) {
            if(byte_val & (1 << bit)) {
                // 发送'1'码
                RGB_PIN_H();
                delay_us(1);
                RGB_PIN_L();
                delay_0_25us();
            } else {
                // 发送'0'码
                RGB_PIN_H();
                delay_0_25us();
                RGB_PIN_L();
                delay_us(1);
            }
        }
    }
    
    // 发送复位信号
    RGB_PIN_L();
    delay_us(RESET_DELAY);
    
    __enable_irq(); // 重新开启中断
}

void set_led_color(uint8_t id, uint8_t red, uint8_t green, uint8_t blue)
{
    // WS2812使用GRB顺序
    LedsArray[id*3] = green;
    LedsArray[id*3+1] = red;
    LedsArray[id*3+2] = blue;
}

/******************************************************************
 * 函 数 名：set_ALL_RGB_COLOR
 * 功能说明：设置所有LED为相同颜色
 * 形    参：color颜色代码
 * 返 回 值：无
******************************************************************/
void set_ALL_RGB_COLOR(unsigned long color)
{
    for(int i = 0; i < WS2812_MAX; i++)
    {
        rgb_SetColor(i, color);
    }
}

/******************************************************************
 * 函 数 名：turn_off_all_leds
 * 功能说明：关闭所有LED
 * 形    参：无
 * 返 回 值：无
******************************************************************/
void turn_off_all_leds(void)
{
    for(int i = 0; i < WS2812_MAX * 3; i++)
    {
        LedsArray[i] = 0;
    }
}

// 简单测试函数 - 直接发送原始数据测试
void RGB_Test_Simple(void)
{
    // 两个LED都设置为中等亮度的红色
    LedsArray[0] = 0;    // G
    LedsArray[1] = 128;  // R
    LedsArray[2] = 0;    // B
    LedsArray[3] = 0;    // G
    LedsArray[4] = 128;  // R  
    LedsArray[5] = 0;    // B
    
    rgb_SendArray();
}
