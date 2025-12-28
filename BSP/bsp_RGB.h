#ifndef _BSP_WS2812_H_
#define _BSP_WS2812_H_
 
#include "ti_msp_dl_config.h"
#include "delay.h"
#include <stdint.h>

#define WS2812_MAX        2   // LED灯珠总数
#define WS2812_NUMBERS    2   // 灯珠个数
#define RGB_COLOR_COUNT (sizeof(rgbColors)/sizeof(rgbColors[0]))

// 用户修改参数区 - 如果ti_msp_dl_config.h中没有定义RGB端口，请在syscfg中配置或手动定义
#ifndef RGB_PORT
#define RGB_PORT          GPIOB
#define RGB_WQ2812_PIN    DL_GPIO_PIN_10
#endif

#define RGB_PIN_L()       DL_GPIO_clearPins(RGB_PORT, RGB_WQ2812_PIN)  // 控制彩灯引脚，需要根据为强推挽输出
#define RGB_PIN_H()       DL_GPIO_setPins(RGB_PORT, RGB_WQ2812_PIN)    // 控制彩灯引脚，需要根据为强推挽输出


typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_Color;

extern unsigned int colorIndex;

#define RED               0xff0000                  // 红色
#define ORANGE            0xffa500                  // 橙色
#define GREEN             0x00ff00                  // 绿色
#define BLUE              0x0050ff                  // 蓝色（增加绿色分量提高亮度）
#define YELLOW            0xffff00                  // 黄色
#define PURPLE            0xff00ff                  // 紫色
#define CYAN              0x00ffff                  // 青色
#define RGB_BLACK         0x000000                  // 熄灭
#define RGB_WHITE         0xffffff                  // 白色


static const unsigned long rgbColors[] = {
    RED,
    ORANGE,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    CYAN,
    RGB_WHITE,
    RGB_BLACK
};

void Ws2812b_WriteByte(unsigned char byte);
void rgb_SetColor(unsigned char LedId, unsigned long color);
void rgb_SetRGB(unsigned char LedId, unsigned long red, unsigned long green, unsigned long blue);
void rgb_SendArray(void);
void RGB_LED_Reset(void);
void delay_0_25us(void);
void ws2812_send(void);
void set_led_color(uint8_t id, uint8_t red, uint8_t green, uint8_t blue);
void RGB_Test_Simple(void);  // 简单测试函数
void set_ALL_RGB_COLOR(unsigned long color);
void turn_off_all_leds(void);

#endif
