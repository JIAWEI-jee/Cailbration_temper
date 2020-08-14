#include "timer.h"
#include "key.h"
#include "flash.h"
#include "uart.h"
#include "lcddisplay.h"

//----------------time---------------
u16 time_cnt = 0;
 u8 write_temper_std = 1;






void time0_init ( void )
{
	/**********************************TIM0配置初始化**************************************/
	TCON1 = 0x00;						//Tx0定时器时钟为Fosc/12
	TMOD = 0x00;						//16位重装载定时器/计数器

	//Tim0计算时间 	= (65536 - 0xFACB) * (1 / (Fosc /Timer分频系数))
	//				= 1333 / (16000000 / 12)
	//				= 1 ms

	//定时1ms
	//反推初值 	= 65536 - ((1/1000) / (1/(Fosc / Timer分频系数)))
	//		   	= 65536 - ((1/1000) / (1/(16000000 / 12)))
	//			= 65536 - 1333
	//			= 0xFACB
//    TH0 = 0xCB;  10ms
//	TL0 = 0xEB;
	TH0 = 0xF5;
	TL0 = 0x96;							//T0定时时间2ms     0x96     10ms   0xFB
	IE |= 0x02;							//打开T0中断
	TCON |= 0x10;						//使能T0
}



//10ms
void TIMER0_Rpt ( void ) interrupt TIMER0_VECTOR
{
	if ( get_device_state() == OFF ) //flash_info.timer != TIMER_ON &&
	{
		time_cnt++;
		if ( time_cnt >= 100 )
		{
			if (write_temper_std == 1)
		  LED2 = !LED2;
			else if (write_temper_std == 2)
			{
				LED1 = 0;
				LED2 = 1;
      }			
			time_cnt = 0;
		}
			
	}

	LCD_Display();
}

