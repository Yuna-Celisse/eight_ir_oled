#include "AllHeader.h"

int main(void)
{
    SYSCFG_DL_init();
    
    OLED_Init();
    
    OLED_Clear();
    OLED_ShowString(0, 0, (uint8_t*)"GPIO Test", 8, 1);
    OLED_ShowString(0, 16, (uint8_t*)"PA18: ", 8, 1);
    OLED_Refresh();
    
    uint16_t press_count = 0;
    
    while (1)
    {
        uint32_t gpio_raw = DL_GPIO_readPins(KEY_PORT, KEY_K1_PIN);
        OLED_ShowString(48, 16, (uint8_t*)"      ", 8, 1);
        OLED_ShowNum(48, 16, gpio_raw, 6, 8, 1);
        
        OLED_Refresh();
    }
}
