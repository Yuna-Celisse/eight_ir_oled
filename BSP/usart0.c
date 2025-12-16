#include "usart0.h"


#define RE_0_BUFF_LEN_MAX	128

volatile uint8_t  recv0_buff[RE_0_BUFF_LEN_MAX] = {0};
volatile uint16_t recv0_length = 0;
volatile uint8_t  recv0_flag = 0;

void USART_Init(void)
{
	// SYSCFG初始化
	// SYSCFG initialization
	SYSCFG_DL_init();
	//清除串口中断标志
	//Clear the serial port interrupt flag
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	//使能串口中断
	//Enable serial port interrupt
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
}

//串口发送单个字符
//Send a single character through the serial port
void uart0_send_char(char ch)
{
	//当串口0忙的时候等待，不忙的时候再发送传进来的字符
	//Wait when serial port 0 is busy, and send the incoming characters when it is not busy
	while( DL_UART_isBusy(UART_0_INST) == true );
	//发送单个字符
	//Send a single character
	DL_UART_Main_transmitData(UART_0_INST, ch);

}
//串口发送字符串	Send string via serial port
void uart0_send_string(char* str)
{
	//当前字符串地址不在结尾 并且 字符串首地址不为空
	//The current string address is not at the end and the string's first address is not empty
	while(*str!=0&&str!=0)
	{
		//发送字符串首地址中的字符，并且在发送完成之后首地址自增
		//Send the characters in the first address of the string, and the first address will increment automatically after the sending is completed.
		uart0_send_char(*str++);
	}
}






