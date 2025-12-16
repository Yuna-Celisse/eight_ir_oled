#include "bsp_ir.h"
#include "stdio.h"
#include "string.h"

// 定义串口接收缓冲区长度
// Define USART receive buffer length
#define USART_RECEIVE_LENGTH 200
u8 IR_Data_number[IR_Num];// 存储解析出的数字值 (Store parsed numeric values)
char IR_recv_buff[USART_RECEIVE_LENGTH];    // 串口接收缓冲区 (USART receive buffer)
volatile int IR_recv_length = 0;            // 接收数据长度 (Length of received data)
volatile u8 IR_recv_complete_flag = 0;    // 接收完成标志位 (Receive completion flag)
volatile u8 oledbuf[20] = {0};  // OLED显示缓冲区 (OLED display buffer)

/**
 * @brief 红外接收串口配置函数
 * @brief IR receive USART configuration function
 */
void IR_usart_config(void)
{
    // 清除串口中断标志
    // Clear USART interrupt flag
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    // 使能串口中断
    // Enable USART interrupt
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

}

/**
 * @brief 红外数据解析函数
 * @brief IR data analysis function
 */
void IRDataAnalysis(void)
{
    char temp[60] = {0};  // 临时数据缓冲区 (Temporary data buffer)
    char *buff = NULL;    // 指向有效数据起始地址的指针 (Pointer to start of valid data)
    int head = 0;         // 头指针，用于查找数据头 (Head pointer for finding data header)
    int end = 0;          // 尾指针，用于查找数据尾 (Tail pointer for finding data tail)


    // 没有接收到数据 或者 数据没有接收完成 则不进行处理
    // Do not process if no data is received or data reception is incomplete
    if (IR_recv_complete_flag == 0) return;

    // 关中断，防止在解析过程中接收到新数据
    // Disable interrupts to prevent new data reception during parsing
    __disable_irq();

    // 找到格式的头的第一个 '$'
    // Find the first '$' of the format header
    while ((IR_recv_buff[head] != '$') && (head < IR_recv_length))
    {
        head++;
    }
    if (head == IR_recv_length)
    {
        // 未找到数据头，清除接收标志，等待下一次接收
        // No data header found, clear receive flag and wait for next reception
        IR_recv_complete_flag = 0;
        IR_recv_length = 0;
        __enable_irq();
        return;
    }
    buff = &IR_recv_buff[head];  // 设置有效数据起始地址 (Set start address of valid data)

    // 找到结尾 '#'
    // Find the ending '#'
    while ((buff[end] != '#') && (end < IR_recv_length))
    {
        end++;
    }
    if ((head + end) == IR_recv_length)
    {
        // 未找到数据尾，清除接收标志，等待下一次接收
        // No data tail found, clear receive flag and wait for next reception
        IR_recv_complete_flag = 0;
        IR_recv_length = 0;
        __enable_irq();
        return;
    }

    // 复制数据到 temp 缓冲区
    // Copy data to temp buffer
    if (end + 1 < sizeof(temp))
    {
        strncpy(temp, buff, end + 1);
        temp[end + 1] = '\0';  // 确保字符串终止 (Ensure string termination)

        // 检查数据格式是否正确（$D,...）
        // Check if data format is correct ($D,...)
        if (temp[0] == '$' && temp[1] == 'D')
        {
            char *token = strtok(temp, ",");  // 分割字符串 (Split string)
            int index = 0;

            while (token != NULL && index < IR_Num)
            {
                // 解析 x1:1, x2:1, ..., x8:1 格式的数据
                // Parse data in x1:1, x2:1, ..., x8:1 format
                if (strstr(token, "x") != NULL)
                {
                    char *colon = strchr(token, ':');  // 查找冒号位置 (Find colon position)
                    if (colon != NULL)
                    {
                        // 提取 ':' 后的数字字符并转换为数值
                        // Extract the numeric character after ':' and convert to value
                        IR_Data_number[index] = (colon[1] - '0');  // 提取 '0' 或 '1' (Extract '0' or '1')
                        index++;
                    }
                }
                token = strtok(NULL, ",");  // 继续分割下一个字段 (Continue splitting next field)
            }

//            // 显示解析结果
//            // Display parsing result
//            sprintf(oledbuf, "%d%d%d%d%d%d%d%d",
//                IR_Data_number[0], IR_Data_number[1],
//                IR_Data_number[2], IR_Data_number[3],
//                IR_Data_number[4], IR_Data_number[5],
//                IR_Data_number[6], IR_Data_number[7]);

////            OLED_Clear();
//            OLED_ShowString(0, 40, (uint8_t *)oledbuf, 8, 1);
//            OLED_Refresh();
        }
    }

    // 清除接收完成标志位，重置缓冲区，等待下一次接收
    // Clear receive completion flag, reset buffer, wait for next reception
    IR_recv_complete_flag = 0;
    IR_recv_length = 0;
    memset(IR_recv_buff, 0, USART_RECEIVE_LENGTH);
    __enable_irq();  // 重新使能中断 (Re-enable interrupts)
}

/**
 * @brief UART0中断处理函数
 * @brief UART0 interrupt handler function
 */
void UART_0_INST_IRQHandler(void)
{
    uint8_t RecvDATA = 0;  // 接收的字节数据 (Received byte data)

    // 检查中断来源
    // Check interrupt source
    switch (DL_UART_getPendingInterrupt(UART_0_INST))
    {
        case DL_UART_IIDX_RX: // 接收中断 (Receive interrupt)
            RecvDATA = DL_UART_Main_receiveData(UART_0_INST);  // 读取接收的数据 (Read received data)

            // 检查缓冲区是否已满
            // Check if buffer is full
            if (IR_recv_length >= USART_RECEIVE_LENGTH - 1)
            {
                // 缓冲区满，重置接收状态（丢弃数据）
                // Buffer is full, reset receive status (discard data)
                IR_recv_complete_flag = 0;
                IR_recv_length = 0;
                break;
            }

            // 存储接收的数据到缓冲区
            // Store received data into buffer
            IR_recv_buff[IR_recv_length++] = RecvDATA;
            IR_recv_buff[IR_recv_length] = '\0';  // 确保字符串终止 (Ensure string termination)

            // 收到 '#' 时标记接收完成
            // Mark reception complete when '#' is received
            if (RecvDATA == '#')
            {
                IR_recv_complete_flag = 1;
            }
            break;

        default: // 其他中断（如发送中断） (Other interrupts, e.g., transmit interrupt)
            break;
    }
}