/**
*	************************************************************************************
*	@Examle Version		K8104
*	@Demo	Version		V1.00.00
*	@Date				2020.6��23
*	************************************************************************************
*
**/

#define	ALLOCATE_EXTERN
#include "HC89F3541.H"
#include "delay.h"
#include "adc.h"
#include "flash.h"
#include "key.h"
#include "uart.h"
#include "pwm.h"
#include "wdt.h"
#include "timer.h"
#include "lcddisplay.h"
#include "PID.h"
#include "math.h"
#include "string.h"

#define SKU 8104
#define SOFT_VER "1.00.00"

const float Rp = 10000.0;
const float Bx = 3950.0;
const float T2 = ( 273.15 + 25 );
const float Ka = 273.15;



u16  temper = 0;

void Clock ( void );
u8 temperature_handle ( void );
void Rx_Data_handle(void);


/***************************************************/
/*
�������ƣ�key_handle()
�������ܣ�����������
��ڲ�����null
���ڲ�����null
����˵�������������صļ�ֵ������Ӧ����
*/
/********************************************************/

static void key_handle ( void )
{
	u8 key_val = 0;
	key_val = key_scan();

	if ( key_val == KEY_1_PRES )
	{
		//LED1 = !LED1;
   write_temper_std = 1;
	}

	else if ( key_val == KEY_2_PRES )
	{
		
		temper = temperature_handle (  );
		if (write_temper_std == 1)
		producte_send_cmd ( temper, 0x01 );
		temper = temper%10;
  	lcd_display_gap ( temper );
		
	}
	else if ( key_val == KEY_3_PRES ) //??
	{
		//LED3 = !LED3;
  
	}

}



/*********************************************************/
/*
�������ƣ�temperature_handle()
�������ܣ��¶ȴ�����
��ڲ�����null
���ڲ�����null
����˵��������ADC��⵽��ֵ����¶�ֵ
*/
/********************************************************/

u8 temperature_handle ( void )
{
	u8 temp = 0,temp2 = 0,i = 0;
	u16 adc_val1 = 0,ntc_val = 0;
	float ntc_res = 0,ntc_temper = 0,ntc_sum =0;

	for (i =0;i<10;i++)
	{
	get_voltage ( &adc_val1,&ntc_val );
	ntc_temper = ( float ) ntc_val/4095*5;
	ntc_res = 5 - ntc_temper;
	ntc_res =  ntc_res/10000 ;
	ntc_res =  ntc_temper / ntc_res;
	ntc_sum += ( 1/ ( log ( ntc_res/Rp ) /Bx+ ( 1/T2 ) ) )-273.15+0.5;
		
	}	
	temp2 = ( u8 ) (ntc_sum/10);

	return temp2;

}








void main()
{
	Clock();
	uart_init();
	adc_init();
	EA = 1;
	time0_init ();						//�����ж�
	key_init();
	flash_init();
	PID_Init();
	pwm_init ( 200 );
	wdt_init ( 2 );
	LCD_Init();
	lcd_display_On();
	delay_ms ( 1200 );
	lcd_clear_all ();


	while ( 1 )
	{


		key_handle ();
   Rx_Data_handle();     
		clear_wdt();

	}
}


void Rx_Data_handle(void)
{
    if (usart_rx_flag == 3)
        {
            
            usart_rx_flag = 0;
		      //	LED3 = !LED3;
			    	write_temper_std = 2;

        }
}


void Clock ( void )
{
	/************************************ϵͳ��ʼ��****************************************/
	CLKSWR = 0x51;						//ѡ���ڲ���ƵRCΪϵͳʱ�ӣ��ڲ���ƵRC 1��Ƶ��Fosc=16MHz
	CLKDIV = 0x01;						//Fosc 1��Ƶ�õ�Fcpu��Fcpu=16MHz
}




