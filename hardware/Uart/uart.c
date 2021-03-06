#include "uart.h"
#include "key.h"

extern volatile u8 usart_rx_buffer[30] = {0};
extern volatile u8 usart_rx_cnt;
extern  u8 usart_rx_flag;



#define PRODUCTE_CMD_HEADER_VALUE                   0x55
#define PRODUCTE_CMD_TAIL1_VALUE                    0x0D
#define PRODUCTE_CMD_TAIL2_VALUE                    0x0A


struct producte_cmd_struct
{
	u8 header;
	u8 funcode;
	u8 Data;
	u8 tail1;
	u8 tail2;
};



//初始化串口9600
void uart_init ( void )
{
#if _DEBUG_PRINTF_
	P2M6 =  0xC1;		//P26设置为推挽输出   0xC1
	P2M5 = 0x60;		//P25设置为上拉输入 0x60
	TXD_MAP = 0x26;		//TXD映射P26
	RXD_MAP = 0x25;		//RXD映射P25
	T4CON = 0x06;		//T4工作模式：UART1波特率发生器
	//波特率计算
	//波特率 = 1/16 * (T4时钟源频率 / 定时器4预分频比) / (65536 - 0xFF98)
	//       = 1/16 * ((16000000 / 1) / 104)
	//		 = 9615.38(误差0.16%)

	//波特率9600
	//反推初值 = (65536 - ((T4时钟源频率 / 定时器4预分频比) * (1 / 16)) / 波特率)
	//		   = (65536 - (16000000 * (1 / 16) / 9600))
	//		   = (65536 - 104.167)
	//         = FF98
	// TH4 = 0xFF;
	// TL4 = 0x98;			//波特率9600
//	TH4 = 0xFF;//9600  4M 波特率9600
//	TL4 = 0x98;
//	T4CON = 0x06;		//T4工作模式：UART1波特率发生器
//	SCON2 = 0x02;		//8位UART，波特率可变 方式一
//	SCON = 0x10;		//允许串行接收
//	ES1 = 1;			//使能串口中断

	TH4 = 0xFF;
	TL4 = 0x98;							//波特率9600
	SCON2 = 0x02;						//8位UART，波特率可变
	SCON = 0x10;						//允许串行接收
	IE |= 0x10;							//使能串口中断
	EA = 1;								//使能总中断
#endif
}


//串口发送一个字符
static void hal_uart_putchar ( char Char )
{
//	ES1= 0;				//失能UART1中断
//	SBUF = Char;
//	while(!TI);
//	TI = 0;
//	ES1= 1;				//UART1接收使能

	IE &=~ 0x10;				//失能UART1中断
	SBUF =  Char;			//发送8位串口数据
	while ( ! ( SCON & 0x02 ) );
	SCON &=~ 0x02;				//清除发送中断标志位
	IE |= 0x10;					//UART1中断使能
	SCON |= 0x10;				//UART1接收使能
}


/* @function:串口发送一个字节
 * @param:需要发送的字节
 * @return:None
 */
static void producte_com_send_one_bit ( char Data )
{
	hal_uart_putchar ( Data );
}
/* @function:串口发送数组
 * @param:data:数组头指针
 *        len:数组长度
 * @return:None
 */
static void producte_com_send_data ( u8* Data, u8 len )
{
	u8 i;
	for ( i = 0; i < len; i++ )
	{
		producte_com_send_one_bit ( Data[i] );
	}
}

/* @function:发送命令
 * @param:funcode:功能码
 *        data:数据
 * @return:None
 */
void producte_send_cmd ( u8 funcode, u8 Data )
{
	struct producte_cmd_struct cmd_t;
	cmd_t.header = PRODUCTE_CMD_HEADER_VALUE;
	cmd_t.funcode = funcode;
	cmd_t.Data = Data;
	cmd_t.tail1 = PRODUCTE_CMD_TAIL1_VALUE;
	cmd_t.tail2 = PRODUCTE_CMD_TAIL2_VALUE;
	producte_com_send_data ( ( u8* ) &cmd_t, sizeof ( struct producte_cmd_struct ) );
}


//重写stdio 中的putchar函数  实现printf
char putchar ( char c )
{
	hal_uart_putchar ( c );
	return c;
}


void UART1_Rpt ( void ) interrupt UART1_VECTOR
{
	u8 Data = 0;
	if ( SCON & 0x01 )						//判断接收中断标志位
	{
		Data = SBUF;				//转存8位串口接收数据
		SCON &=~ 0x01;              	//清除接收中断标志位
		/* receive Data */
		switch ( usart_rx_flag )
		{
			case 0:
				if ( Data == 0x55 )
				{
					usart_rx_buffer[0] = Data;
					usart_rx_cnt++;
					//  putchar( usart_rx_cnt);
					usart_rx_flag = 1;
					hal_uart_putchar ( Data );
					// LED1 = !LED1;

				}
				break;
			case 1:
				if ( Data == 0x02 )
				{
					usart_rx_flag = 3;

				}
				break;

		}


	}
}



