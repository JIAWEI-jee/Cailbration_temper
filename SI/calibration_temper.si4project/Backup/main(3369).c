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

#define SKU 8104
#define SOFT_VER "1.00.00"

const float Rp = 10000.0;
const float Bx = 3950.0;
const float T2 = ( 273.15 + 25 );
const float Ka = 273.15;



u16 adc_cnt = 0;
u8  first_heat_std = 0;
u8  error_std = 0;

void Clock ( void );
void Set_Temp ( void );
void Controll_Heat ( void );
//void Protect ( void );
void Detection_Input ( void );
u8 temperature_handle ( void );



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
	u8 temper = 0;
	key_val = key_scan();

	if ( key_val == KEY_1_PRES )
	{
		//LED1 = !LED1;

	}

	else if ( key_val == KEY_2_PRES )
	{
		temper =  temperature_handle (  );
		
		producte_send_cmd ( 0X01, temper );
		
	}
	else if ( key_val == KEY_3_PRES ) //??
	{
		//LED3 = !LED3;

	}

}

/***************************************************/
/*
�������ƣ�temp_calc()
�������ܣ��¶ȴ�����
��ڲ�����uRt ,uRw
���ڲ�����temperature
����˵��������ADC��⵽��ֵ����ʵ���¶�ֵ
*/
/********************************************************/

u16 temp_calc ( u16 uRt,u16 uRw )
{
	u16 i = 0;
	u16 basi_tmp = 40;

	float u1 = 0;
	float u3 = 0;
	if ( uRt < 50 )
	{
		return 0xff;
	}

	u1 = ( float ) uRt/1000;
	u3 = 	( float ) uRw /1000;
	u1 = (  u3 - u1 ) / u1 * 750;
	if ( u1 < 0 )
	{
		return 0xff;
	}
	//gm_printf ( "R = %f  \r\n",u1 );


	if ( u1 > Temperature_Value )
	{
		i = ( u1 - Temperature_Value ) /0.4;
		//gm_printf("over 40  i:%d \r\n",i);
		basi_tmp = basi_tmp + i;
	}

	else
	{
		i = ( Temperature_Value - u1 ) /0.4;
		//  gm_printf("under 40  i:%d \r\n",i);
		basi_tmp = basi_tmp - i;
	}

//	gm_printf("basi_tmp:%d \r\n",basi_tmp);
	if ( flash_info.gap == GAP_3 )
	{
		basi_tmp = basi_tmp - 5;
	}

	return  basi_tmp;
}


/*********************************************************/
/*
�������ƣ�temperature_handle()
�������ܣ��¶ȴ�����
��ڲ�����null
���ڲ�����null
����˵��������ADC��⵽��ֵ����¶�ֵ��ͨ���¶ȵ�ֵ�����¶ȿ���
*/
/********************************************************/

u8 temperature_handle ( void )
{
	u8 temp = 0,temp2 = 0;
	u16 adc_val1 = 0,ntc_val = 0;
	float ntc_res = 0,ntc_temper = 0;

	adc_cnt = 0;
	get_voltage ( &adc_val1,&ntc_val );
	ntc_temper = ( float ) ntc_val/4095*3;
	ntc_res = 3.3 - ntc_temper;
	ntc_res =  ntc_res/10000 ;
	ntc_res =  ntc_temper / ntc_res;
	ntc_res = ( 1/ ( log ( ntc_res/Rp ) /Bx+ ( 1/T2 ) ) )-273.15+0.5;
	temp2 = ( u8 ) ntc_res;
	temp = temp2%10;

	lcd_display_gap ( temp );
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
	Detection_Input();

	gm_printf ( "\r\n==================================\r\n" );
	gm_printf ( "sku:K%d \r\n", ( u16 ) SKU );
	gm_printf ( "soft version:%s \r\n",SOFT_VER );
	gm_printf ( "gap %d \r\n", ( u16 ) flash_info.gap );      //��λ
	gm_printf ( "timer %d \r\n", ( u16 ) flash_info.timer );  // ��ʱ
	gm_printf ( "Input_Voltage_std = %d\r\n",Input_Voltage_std ); //�����ѹ
	gm_printf ( "==================================\r\n" );

	while ( 1 )
	{


		key_handle ();
        
		clear_wdt();

	}
}


void Rx_Data_handle(void)
{
    if (usart_rx_flag == 3)
        {
            usart_rx_cnt = 0;
            usart_rx_flag = 0;
            switch (usart_rx_buffer[1])
            {
                case 0:
                    break;
                case 1:   //���Կ�ʼ

                    break;
                case 2:     //���Խ���
                  
                    break;
                case 3:   //���Դ���
                    
                case 4:     //����ͨ��
                    if (Relay_std == 1)
                    {
                        LedxOn((LedList)LED_PASS);
                        LedxOff((LedList)LED_START);
                    }
                    start_flag = 0;
                    break;
            }
        }

}


void Clock ( void )
{
	/************************************ϵͳ��ʼ��****************************************/
	CLKSWR = 0x51;						//ѡ���ڲ���ƵRCΪϵͳʱ�ӣ��ڲ���ƵRC 2��Ƶ��Fosc=4MHz
	CLKDIV = 0x01;						//Fosc 1��Ƶ�õ�Fcpu��Fcpu=16MHz
}


/***************************************************/
/*
�������ƣ�Controll_Heat()
�������ܣ�����PWM����
��ڲ�����null
���ڲ�����null
����˵�������ݿ����㷨�õ���ռ�ձ��������
*/
/********************************************************/

void Controll_Heat ( void )
{
	//gm_printf ( " spid.iPriVal = %d \r\n",  spid.iPriVal);
	u8 heat_step_val = 100;
	if ( Input_Voltage_std == V_24_status )
	{
		spid.iPriVal = spid.iPriVal/2;
		heat_step_val = 50;
	}
	if ( heat_step == 1 )
	{
		set_pwm ( heat_step_val );
	}
	else if ( heat_step == 0 )

	{
		set_pwm ( ( u8 ) spid.iPriVal ); //(u8) spid.iPriVal
	}
}

/***************************************************/
/*
�������ƣ�Set_Temp()
�������ܣ��趨Ŀ���¶�
��ڲ�����null
���ڲ�����null
����˵�������ݲ�ͬ��λ�趨Ҫ���ȵ���Ŀ���¶�
*/
/********************************************************/

void Set_Temp ( void )
{


	switch ( flash_info.gap )
	{
		case GAP_WARM:

			break;
		case GAP_1:
			spid.iSetVal = GAP_1_temp*10;
			break;
		case GAP_2:
			spid.iSetVal = GAP_2_temp*10;
			break;
		case GAP_3:
			spid.iSetVal = GAP_3_temp*10;
			break;
		case GAP_H:
			spid.iSetVal = GAP_4_temp*10;
			break;
	}
}

/*********************************************************/
/*
�������ƣ�Protect()
�������ܣ�PID����
��ڲ�����null
���ڲ�����null
����˵��������Gap1��λ����¼��ȳ���2Сʱ������Gap1
*/
/********************************************************/
//void Protect ( void )
//{
//	if ( over_rang_time_std == 1 )
//	{
//		if ( flash_info.gap > GAP_1 )
//		{
//			flash_info.gap = GAP_1;
//			Set_Temp ( );
//			flah_save_data();
//			over_rang_time_std = 0;
//		}
//	}
//}


void Detection_Input ( void )
{
	u16 input = 0;
	input = Detection_Input_Voltage( );
	if ( input > 1600 )
	{
		//gm_printf ( "input24 = %d\r\n" ,input); //�����ѹ
		Input_Voltage_std = V_24_status;
	}
	else
	{
		//gm_printf ( "input12 = %d\r\n" ,input); //�����ѹ
		Input_Voltage_std = V_12_status;
	}
}

