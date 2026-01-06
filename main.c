#include "AllHeader.h"

extern unsigned char LedsArray[];
void Beep_Times(uint8_t times);
void Display_Time(void);
void Display_Countdown(uint8_t seconds);
void Check_Wide_Line(void);
void RGB_Running_Effect(void);

volatile uint8_t system_running = 0;
volatile uint8_t race_finished = 0;
extern volatile uint32_t run_time_ms;

int main(void)
{
	SYSCFG_DL_init();
	
	DL_GPIO_initDigitalOutput(IOMUX_PINCM27);
	DL_GPIO_enableOutput(RGB_PORT, RGB_WQ2812_PIN);
	DL_GPIO_clearPins(RGB_PORT, RGB_WQ2812_PIN);
	
	for(volatile int i=0; i<100000; i++);
	
	Control_RGB_ALL(Red_RGB);
	delay_ms(200);
	Control_RGB_ALL(Green_RGB);
	delay_ms(200);
	Control_RGB_ALL(OFF);
	delay_ms(10);
	       
	OLED_Init();
	USART_Init();
	Init_Motor_PWM();
	encoder_init();
  OLED_Clear();
  OLED_ShowString(0, 0, (uint8_t*)"Press K1 Start", 8, 1);
  OLED_Refresh();

	uart0_send_string("$0,0,1#");
	
	uint32_t loop_count = 0;
	
	while (1)                
	{
		loop_count++;
		
		uint32_t gpio_raw = DL_GPIO_readPins(KEY_PORT, KEY_K1_PIN);
		uint8_t raw_key = KEY_Read();
	
		uint8_t key_value = KEY_Scan();
		if (key_value)
		{
			if (system_running == 0)
			{
				OLED_Clear();
				OLED_ShowString(0, 0, (uint8_t*)"Starting in 1s", 8, 1);
				OLED_Refresh();
				
				delay_ms(1000);
				
				system_running = 1;
				run_time_ms = 0;
				
				OLED_Clear();
				OLED_ShowString(0, 0, (uint8_t*)"Running...", 8, 1);
				OLED_Refresh();
				
				DL_GPIO_setPins(BEEP_PORT, BEEP_Buzzer_PIN);
				delay_ms(100);
				DL_GPIO_clearPins(BEEP_PORT, BEEP_Buzzer_PIN);
				
				for(int i=0; i<3; i++) {
					Control_RGB_ALL(Red_RGB);
					delay_ms(100);
					Control_RGB_ALL(OFF);
					delay_ms(100);
				}
			}
			else
			{
				system_running = 0;
				Motor_Set(0, 0, 0, 0);
				
				Control_RGB_ALL(OFF);
				
			}
			delay_ms(200);
		}
		
		if (system_running == 0)
		{
			if (race_finished)
			{
					uint32_t sec = run_time_ms / 1000;
					uint32_t ms = run_time_ms % 1000;
					
					OLED_Clear();
					OLED_ShowNum(0, 0, sec, 2, 8, 1);
					OLED_ShowString(16, 0, (uint8_t*)".", 8, 1);
					OLED_ShowNum(30, 0, ms, 3, 8, 1);
					OLED_ShowString(50, 0, (uint8_t*)"s", 8, 1);
					OLED_Refresh();
				while(1);
			}
		}
		
		if (system_running == 1)
		{
		uint32_t sec = run_time_ms / 1000;
		uint32_t ms = run_time_ms % 1000;
		
		OLED_ShowString(0, 0, (uint8_t*)"T:  .   s", 8, 1);
		OLED_ShowNum(16, 0, sec, 2, 8, 1);
		OLED_ShowNum(40, 0, ms, 3, 8, 1);
		
		OLED_Refresh();
		}
	}
}
