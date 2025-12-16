#include "AllHeader.h"


int main(void)
{
	// 系统配置初始化  System configuration initialization
	SYSCFG_DL_init();       
	 // OLED显示屏初始化  OLED display initialization
	OLED_Init();            
	 // 串口通信初始化  USART communication initialization	
	USART_Init();           
	 
	Init_Motor_PWM();
   //初始化电机编码器
  encoder_init(); 
//  电机控速pid初始化
  PID_Param_Init();

	uart0_send_string("$0,0,1#");
	
	while (1)                
	{

		if( IR_recv_complete_flag)
			{    
				OLED_ShowString(0,25,"IR:",8,1);
				for (int i = 0; i < 8; i++) 
						OLED_ShowNum(20 + i * 10, 25,IR_Data_number[i], 1, 8, 1);        
			}
				
				LineWalking();
				OLED_Refresh();
	}	

}













