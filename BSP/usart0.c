#include "usart0.h"


#define RE_0_BUFF_LEN_MAX	128

volatile uint8_t  recv0_buff[RE_0_BUFF_LEN_MAX] = {0};
volatile uint16_t recv0_length = 0;
volatile uint8_t  recv0_flag = 0;

void USART_Init(void)
{
	SYSCFG_DL_init();
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
}

void uart0_send_char(char ch)
{
	while( DL_UART_isBusy(UART_0_INST) == true );
	DL_UART_Main_transmitData(UART_0_INST, ch);

}

void uart0_send_string(char* str)
{
	while(*str!=0&&str!=0)
	{
		uart0_send_char(*str++);
	}
}






