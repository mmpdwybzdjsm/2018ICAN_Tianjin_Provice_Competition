#include "stm32f10x.h"
#include "bsp_led.h"
#include "MicroStepDriver.h" 
#include "bsp_usart.h"
#include "bsp_usart3.h"
#include "MSD_test.h" 
#include "beep.h"
#include "openmv.h"
#include "bsp_SysTick.h"
#include "timer.h"

#include "ultrasonic.h"
#include "TIM6.h"
#include "encodertim3.h"
#include "TIM4.h"
#include "TIM1.h"
#include "gesture.h"
#include "simulate.h"
#include "lcd.h"
#include "key.h"
#include "exti.h"
#include "bjdjt.h"
#include "TIM8.h"
#include "bjdj.h"
#include "uart4.h"
extern volatile unsigned int CaptureNumberA1;//��ǰ
extern volatile unsigned int CaptureNumberA2;//��ǰ
extern volatile unsigned int CaptureNumberB1;//���
extern volatile unsigned int CaptureNumberB2;//�Һ�
extern PD Steer;
extern int tim1_step;
int main(void)
{	int i;

    //��ʼ��LED
    LED_GPIO_Config();//
    //��ʼ��BEEP
    //BEEP_GPIO_Config();	
    //��ʼ������
  //  EXTI_Key_Config();
    //��ʼ������
    USART_Config();
	  USART_Config3();
	  UART_Config4();
	//���������ʼ��
	 // MSD_Init();
		SysTick_Init();//Ӳ����ʱ
		KEY_Init();	 //������ʼ��
		EXTIX_Init();//�ⲿ�жϳ�ʼ��
	  TIM1_Init();//��ʱ��1��ʼ���������
	  TIM8_Init();//��ʱ��8��ʼ�������ˢ���
	  
		BJDJT_GPIO_Config();//��������Ƶķ����ʹ�ܵ����ų�ʼ��
		BJDJ_GPIO_Config();//��ת�Ĳ�������ķ����ʹ�ܳ�ʼ��
	 // LCD_Init();	
	  //TIM2_Init();//������134
	  TIM6_Init();//����������
		TIM3_DCmotor_ENCODER_Init();//�������ĵĳ�ʼ��
		IncPIDInit();//pid�ĳ�ʼ��
	  TIM4_Init();//���ٵ����ʼ��
   	LED0=0;
		LED1=0;
		BJDJ_STOP();
	  PD_init(&Steer);

	 #if 0
 	while(!paj7620u2_init())//PAJ7620U2��������ʼ��
	{
	    printf("PAJ7620U2 Error!!!\r\n");
		//LCD_ShowString(30,140,200,16,16,"PAJ7620U2 Error!!!");
	    Delay_ms(500);
		//LCD_ShowString(30,140,200,16,16,"                  ");
		Delay_ms(500);
		LED0=!LED0;//DS0��˸
		
	}
	#endif
	#if 1
		for(i=900;i<2000;i+=100){//2300  1200

		TIM_SetCompare1(TIM8,i);
		TIM_SetCompare2(TIM8,i);
		Delay_ms(100);

		}	 
				for(i=1100;i<2600;i+=100){//2300  1200

		TIM_SetCompare1(TIM8,i);
		TIM_SetCompare2(TIM8,i);
		Delay_ms(500);

		}
				#endif
    while(1) 
    {
			TIM1Control();
			TIM8Comtrol();
			openmvData();
		//	Clockwise();//˳ʱ��
		//	delay_msmoni(5000);
		//	Anticlockwise();//��ʱ��
		//	delay_msmoni(5000);
		//	BJDJ_STOP();
		//	delay_msmoni(5000);
		//	Steeringtest();
//TIM_SetCompare1(TIM8,10000);
//TIM_SetCompare2(TIM8,10000);
			

	}	 
        //����������
			
			//printf("freq1:%d\r\n",CaptureNumberA1);  // ��ӡ����������ֵ
    //  CaptureNumberA1=0;    // ���㣬���㿪ʼ����
		//	printf("freq2:%d\r\n",CaptureNumberA2);  // ��ӡ����������ֵ
    //  CaptureNumberA2=0;    // ���㣬���㿪ʼ����
		//	printf("freq3:%d\r\n",CaptureNumberB1);  // ��ӡ����������ֵ
    //  CaptureNumberB1=0;    // ���㣬���㿪ʼ����
		//	printf("freq4:%d\r\n",CaptureNumberB2);  // ��ӡ����������ֵ
    //  CaptureNumberB2=0;    // ���㣬���㿪ʼ����				
			//LED1( ON );	
	//		LED2(OFF);
		//	Delay_ms(100);
			//paj7620u2_sensor_test();//PAJ7620U2����������
			//Gesrure_test(); 
		//	TIM_SetCEompare1(TIM4,100);

		//	CSB1();
			//CSB2();
			//CSB3();
		//	CSB4();
		//	PCout(8)=0;//˳
			//PCout(7)=1;//˳
	//		PCout(8)=0;//��
	//		LED1( OFF );	
    //  openmvData();
		//	Steeringtest();
	//		Steeringtest();
	//		LED2(ON);
	//		Delay_ms(500);
    }
                          


/*********************************************END OF FILE**********************/
