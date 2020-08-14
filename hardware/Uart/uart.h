#ifndef __UART_H__
#define __UART_H__
#include "HC89F3541.H"
#include "stdio.h"//printf

#define _DEBUG_PRINTF_ 1
#define KEY_DEBUG_PRINT   1
extern volatile u8 usart_rx_buffer[30];
extern volatile u8 usart_rx_cnt;
extern  u8 usart_rx_flag;


#if _DEBUG_PRINTF_
	#define	gm_printf printf
#else
	#define	gm_printf (...)
#endif


#if KEY_DEBUG_PRINT
	#define	KEY_printf printf
#else
	#define	KEY_printf (...)
#endif

void uart_init(void);
void producte_send_cmd(u8 funcode, u8 Data);

char putchar(char c);



#endif


