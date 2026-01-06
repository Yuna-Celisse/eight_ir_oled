#ifndef __USART0_H_
#define __USART0_H_

#include "AllHeader.h"

void USART_Init(void);
void uart0_send_char(char ch);
void uart0_send_string(char* str);


#endif

