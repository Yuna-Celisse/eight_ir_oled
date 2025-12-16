#ifndef _BSP_OPENMV4_H_
#define _BSP_OPENMV4_H_

#include "AllHeader.h"

/* 串口缓冲区的数据长度 */
#define USART_RECEIVE_LENGTH  200
#define IR_Num 8 //探头数量

extern volatile u8 IR_recv_complete_flag;
extern u8 IR_Data_number[];
extern volatile u8 oledbuf[];  
void uart1_send_char(char ch);
void uart1_send_string(char* str);
void IR_usart_config(void);
void IRDataAnalysis(void);
#endif
