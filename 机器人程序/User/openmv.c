#include "openmv.h"  
#include <math.h>
#include <stdlib.h>//atoi
#include "bsp_usart3.h"
#include "bsp_exti.h" 
#include "bsp_led.h"
#include "MicroStepDriver.h" 
#include "bujindianji_dir.h"
#include "Steering.h"
#include "bsp_SysTick.h"
#include "bjdj.h"
int mid_input=0;
extern PD Steer;

extern int x;
extern int y;

void openmvData(void)
{
/*    static char showflag =1;
    char okCmd = FALSE;//���յ���ȷ��ָ���ΪTRUE
    if(status.cmd == TRUE)
    {        
      if(UART_RxBuffer3[0] == '[')
      {
								int i=1;
                while((UART_RxBuffer3[i] != ','))
									{                
                mid_input= atoi((char const *)UART_RxBuffer3+i);
								i++;
								}
				}

                okCmd = TRUE;
      }

      //��մ��ڽ��ջ�������
      status.cmd = FALSE;
      uart_FlushRxBuffer3();
			if((mid_input>0)&&(mid_input<161))
				{		
			  LED1( ON );	
				LED2(OFF);
				}
				else if(mid_input==0)
				{LED1(OFF); 
				LED2(ON);}
				*/
				    if(status.cmd == TRUE)//����
    { 
		//	printf("[%d,%d]",x,y);
			if(x>=180){		
		Anticlockwise();//��ʱ��
		delay_msmoni(30);	
		BJDJ_STOP();}//˳}
			
				if(x<=140){		
					Clockwise();//˳ʱ��
		delay_msmoni(30);
		BJDJ_STOP();}//��}
			 
				PD_cal(&Steer,y);
		status.cmd = FALSE;
		}
}


