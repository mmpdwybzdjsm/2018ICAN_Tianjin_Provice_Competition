
#include "stm32f10x_it.h"
#include "MicroStepDriver.h"  
#include "bsp_usart.h"
#include "bsp_usart3.h"
#include "bsp_led.h"
#include "stdlib.h"
#include "openmv.h"
#include "Steering.h"
#include "ultrasonic.h"
#include "TIM6.h"
#include "bsp_SysTick.h"
#include "encodertim3.h"
#include "TIM4.h"
#include "key.h"
int timer_mode=0;
int tim1_step=0;
unsigned char tim1_mode=2;
extern PID sPID1;
extern PID sPID2;
extern PID sPID3;
extern PID sPID4;
extern volatile unsigned int CaptureNumberA1;
extern volatile unsigned int CaptureNumberA2;
extern volatile unsigned int CaptureNumberB1;
extern volatile unsigned int CaptureNumberB2;
//��������ָ��
extern unsigned char UART_RxPtr;
extern unsigned char UART_RxPtr3;
extern void TimingDelay_Decrement(void);
int i=0;
int j=4;
int x=0;
int y=0;
int rxtx_mode=0;
u32 time6;
int flag_5ms=0;
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
// �����жϷ�����

void DEBUG_USART_IRQHandler(void)

{	
    unsigned char data;
    
	if(USART_GetITStatus(DEBUG_USARTx,USART_IT_RXNE)!=RESET)

	{	

		data = USART_ReceiveData(DEBUG_USARTx);
        if(status.running == FALSE && status.out_ena == TRUE)
        {
            //���Ϊ�˸��
            if(data == '\b')
            {
              //���ָ�벻������Ŀ�ʼλ��
              if(UART_RxPtr)
              {
                Usart_SendByte(DEBUG_USARTx,'\b');
                Usart_SendByte(DEBUG_USARTx,' ');
                Usart_SendByte(DEBUG_USARTx,'\b');
                UART_RxPtr--;
                UART_RxBuffer[UART_RxPtr]=0x00;
              }
            }
            //��������˸��
            else
            {
              //��������������UART_RxBuffer
              //���ҽ������һ��Ԫ�������������������д�����һ��Ԫ��Ϊֹ
              if(UART_RxPtr < (UART_RX_BUFFER_SIZE - 1))
              {
                UART_RxBuffer[UART_RxPtr] = data;
                UART_RxBuffer[UART_RxPtr + 1]=0x00;
                UART_RxPtr++;
              }
              else
              {
                UART_RxBuffer[UART_RxPtr - 1] = data;
                Usart_SendByte(DEBUG_USARTx,'\b');
              }
              //���Ϊ�س�������ʼ����������
              if(data == 13)
              {
                status.cmd = TRUE;
              }
              else
              {
                Usart_SendByte(DEBUG_USARTx,data);
              }
            }
        }

    }	 

}
void USART3_IRQHandler(void)

{	

    unsigned char data;
    
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)

	{				//len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���atoi((char const *)UART_RxBuffer+2);

			//printf("%d",len);
		data = USART_ReceiveData(USART3);
		
		if(data==0x5b){rxtx_mode=1;}//[
		else if(data==0x2c){rxtx_mode=2;}//,
		else if(data==0x5d){rxtx_mode=3;}//]
		if(rxtx_mode==1){
			UART_RxBuffer3[i]=(data-0x30);
			i++;
		}
		else if(rxtx_mode==2){
			UART_RxBuffer3[j]=(data-0x30);
			j++;
		}
		else if(rxtx_mode==3){
			if(i==4){x=UART_RxBuffer3[1]*100+UART_RxBuffer3[2]*10+UART_RxBuffer[3];}
				else if(i==3){x=UART_RxBuffer3[1]*10+UART_RxBuffer3[2];}
					else if(i==2){x=UART_RxBuffer3[1];}
			if(j==8){y=UART_RxBuffer3[5]*100+UART_RxBuffer3[6]*10+UART_RxBuffer[7];}
				else if(j==7){y=UART_RxBuffer3[5]*10+UART_RxBuffer3[6];}
					else if(j==6){y=UART_RxBuffer3[5];}
			i=0;
			j=4;
					status.cmd = TRUE;
		}
		
	//		if(data==0x5b)printf("�ҵ�[\r\n");
	//		if((data>='0')&&(data<='9'))printf("�ҵ�����\r\n");
		//	if(data==0x2c)printf("�ҵ�����\r\n");
		//	if(data==0x5d)printf("�ҵ�]\r\n");
	//	data=atoi((unsigned char *)data);
		
		//USART_SendData(USART1,data);
		//printf("\r\n");
	//	if(data=='[')printf("[");
	//	if((data>='0')&&(data<='9'))USART_SendData(USART1,data);
	//	if(data==',')printf(",");
	//	if(data==']')printf("]");

    }	 

}
void UART4_IRQHandler(void)
{
  unsigned char ucTemp;
	if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)
	{		
		ucTemp = USART_ReceiveData(UART4);
		if(ucTemp==0x30){timer_mode=0;}//0
		if(ucTemp==0x31){timer_mode=1;}//1
		if(ucTemp==0x32){timer_mode=2;}//2
		if(ucTemp==0x33){timer_mode=3;}//3
	  if(ucTemp==0x34){timer_mode=4;}//4
		if(ucTemp==0x35){status.left = 1;}//5
		if(ucTemp==0x36){status.right=1;}//6
		if(ucTemp==0x37){status.up=1;}//7
		if(ucTemp==0x38){status.down=1;}//8
		if(ucTemp==0x39){status.bjdjt=1;}//9
		LED2_TOGGLE;									
   // USART_SendData(UART4,ucTemp);    
	}	 
}
//��ʱ��4�жϷ������
#if 0
void TIM4_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 

		}
}
#endif
void TIM2_IRQHandler(void)
{
	// ��Ҫ��������źŵ����ڴ��ڶ�ʱ�������ʱʱ����ʱ���ͻ���������������ж�
	// ���ʱ��������Ҫ�������Ķ�ʱ���ڼӵ������źŵ�ʱ������ȥ
	if ( TIM_GetITStatus ( TIM2, TIM_IT_Update) != RESET )               
	{	
		ultrasonic1.Capture_Period ++;
		ultrasonic2.Capture_Period ++;
		ultrasonic3.Capture_Period ++;
		ultrasonic4.Capture_Period ++;		
		TIM_ClearITPendingBit ( TIM2, TIM_FLAG_Update ); 		
	}

	// �����ز����ж�
	if ( TIM_GetITStatus (TIM2, TIM_IT_CC1 ) != RESET)
	{
		// ��һ�β���
		if ( ultrasonic1.Capture_StartFlag == 0 )
		{
			// ��������0
			TIM_SetCounter ( TIM2, 0 );
			// �Զ���װ�ؼĴ������±�־��0
			ultrasonic1.Capture_Period = 0;
      // �沶��ȽϼĴ�����ֵ�ı�����ֵ��0			
			ultrasonic1.Capture_CcrValue = 0;

			// ����һ�β���������֮�󣬾ͰѲ����������Ϊ�½���
			TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Falling);
      // ��ʼ�����׼��1			
			ultrasonic1.Capture_StartFlag = 1;			
		}
		// �½��ز����ж�
		else // �ڶ��β���
		{
			// ��ȡ����ȽϼĴ�����ֵ�����ֵ���ǲ��񵽵ĸߵ�ƽ��ʱ���ֵ
			ultrasonic1.Capture_CcrValue = 
			TIM_GetCapture1(TIM2);

			// ���ڶ��β����½���֮�󣬾ͰѲ����������Ϊ�����أ��ÿ����µ�һ�ֲ���
			TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Rising);
      // ��ʼ�����־��0		
			ultrasonic1.Capture_StartFlag = 0;
      // ������ɱ�־��1			
			ultrasonic1.Capture_FinishFlag = 1;		
		}

		TIM_ClearITPendingBit (TIM2,TIM_IT_CC1);	    
	}		
	// �����ز����ж�2
	if ( TIM_GetITStatus (TIM2, TIM_IT_CC2 ) != RESET)
	{
		// ��һ�β���
		if ( ultrasonic2.Capture_StartFlag == 0 )
		{
			// ��������0
			TIM_SetCounter ( TIM2, 0 );
			// �Զ���װ�ؼĴ������±�־��0
			ultrasonic2.Capture_Period = 0;
      // �沶��ȽϼĴ�����ֵ�ı�����ֵ��0			
			ultrasonic2.Capture_CcrValue = 0;

			// ����һ�β���������֮�󣬾ͰѲ����������Ϊ�½���
			TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Falling);
      // ��ʼ�����׼��1			
			ultrasonic2.Capture_StartFlag = 1;			
		}
		// �½��ز����ж�
		else // �ڶ��β���
		{
			// ��ȡ����ȽϼĴ�����ֵ�����ֵ���ǲ��񵽵ĸߵ�ƽ��ʱ���ֵ
			ultrasonic2.Capture_CcrValue = 
			TIM_GetCapture2(TIM2);

			// ���ڶ��β����½���֮�󣬾ͰѲ����������Ϊ�����أ��ÿ����µ�һ�ֲ���
			TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Rising);
      // ��ʼ�����־��0		
			ultrasonic2.Capture_StartFlag = 0;
      // ������ɱ�־��1			
			ultrasonic2.Capture_FinishFlag = 1;		
		}

		TIM_ClearITPendingBit (TIM2,TIM_IT_CC2);	    
	}	
	// �����ز����ж�3
	if ( TIM_GetITStatus (TIM2, TIM_IT_CC3 ) != RESET)
	{
		// ��һ�β���
		if ( ultrasonic3.Capture_StartFlag == 0 )
		{
			// ��������0
			TIM_SetCounter ( TIM2, 0 );
			// �Զ���װ�ؼĴ������±�־��0
			ultrasonic3.Capture_Period = 0;
      // �沶��ȽϼĴ�����ֵ�ı�����ֵ��0			
			ultrasonic3.Capture_CcrValue = 0;

			// ����һ�β���������֮�󣬾ͰѲ����������Ϊ�½���
			TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Falling);
      // ��ʼ�����׼��1			
			ultrasonic3.Capture_StartFlag = 1;			
		}
		// �½��ز����ж�
		else // �ڶ��β���
		{
			// ��ȡ����ȽϼĴ�����ֵ�����ֵ���ǲ��񵽵ĸߵ�ƽ��ʱ���ֵ
			ultrasonic3.Capture_CcrValue = 
			TIM_GetCapture3(TIM2);

			// ���ڶ��β����½���֮�󣬾ͰѲ����������Ϊ�����أ��ÿ����µ�һ�ֲ���
			TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Rising);
      // ��ʼ�����־��0		
			ultrasonic3.Capture_StartFlag = 0;
      // ������ɱ�־��1			
			ultrasonic3.Capture_FinishFlag = 1;		
		}

		TIM_ClearITPendingBit (TIM2,TIM_IT_CC3);	    
	}	
	// �����ز����ж�4
	if ( TIM_GetITStatus (TIM2, TIM_IT_CC4 ) != RESET)
	{
		// ��һ�β���
		if ( ultrasonic4.Capture_StartFlag == 0 )
		{
			// ��������0
			TIM_SetCounter ( TIM2, 0 );
			// �Զ���װ�ؼĴ������±�־��0
			ultrasonic4.Capture_Period = 0;
      // �沶��ȽϼĴ�����ֵ�ı�����ֵ��0			
			ultrasonic4.Capture_CcrValue = 0;

			// ����һ�β���������֮�󣬾ͰѲ����������Ϊ�½���
			TIM_OC4PolarityConfig(TIM2, TIM_ICPolarity_Falling);
      // ��ʼ�����׼��1			
			ultrasonic4.Capture_StartFlag = 1;			
		}
		// �½��ز����ж�
		else // �ڶ��β���
		{
			// ��ȡ����ȽϼĴ�����ֵ�����ֵ���ǲ��񵽵ĸߵ�ƽ��ʱ���ֵ
			ultrasonic4.Capture_CcrValue = 
			TIM_GetCapture4(TIM2);

			// ���ڶ��β����½���֮�󣬾ͰѲ����������Ϊ�����أ��ÿ����µ�һ�ֲ���
			TIM_OC4PolarityConfig(TIM2, TIM_ICPolarity_Rising);
      // ��ʼ�����־��0		
			ultrasonic4.Capture_StartFlag = 0;
      // ������ɱ�־��1			
			ultrasonic4.Capture_FinishFlag = 1;		
		}

		TIM_ClearITPendingBit (TIM2,TIM_IT_CC4);	    
	}		
}
void TIM6_IRQHandler(void)//10khz 100us
{
	if ( TIM_GetITStatus( TIM6, TIM_IT_Update) != RESET ) 
	{	
		time6++;
		flag_5ms++;
		if(flag_5ms==50){
		//forward();	
		//backward();
		//leftward();
		//rightward();
					switch(timer_mode){
			case 0:{
				Stop_it();
				break;
			}
			case 1:{
				forward();
			//	Stop_it();
				break;
			}
			case 2:{
				backward();//1234
			//	Stop_it();
				break;
			}
			case 3:{
			leftward();
		//		Stop_it();
				break;
			}
			case 4:{
			rightward();
			//	Stop_it();
				break;
			}			
			default:{break;}
		}	
		flag_5ms=0;}
	//	if(time6==500){
		if(time6<2){
			PEout(0)=1;
			PEout(1)=0;
			PEout(6)=0;
		}
			//Delay_us(15);
		else if(time6<30000) {
			PEout(0)=0;
			PEout(1)=0;
			PEout(6)=0;
		}	
		else if(time6<30002) {
			PEout(0)=0;
			PEout(1)=1;
			PEout(6)=0;
		}
		else if(time6<60000) {
			PEout(0)=0;
			PEout(1)=0;
			PEout(6)=0;
		}			
		else if(time6<60002) {
			PEout(0)=0;
			PEout(1)=0;
			PEout(6)=1;
		}			
		else if(time6<80000) {
			PEout(0)=0;
			PEout(1)=0;
			PEout(6)=0;
		}		
		else if(time6<80002) {
			PEout(0)=0;
			PEout(1)=0;
			PEout(6)=1;
		}		
		else  {
			PEout(0)=0;
			PEout(1)=0;
			PEout(6)=0;
		}	
		if(time6==50){

		}
	//	}
		if(time6>100000){time6=0;LED1_TOGGLE;}
		TIM_ClearITPendingBit(TIM6 , TIM_FLAG_Update);  		 
	}		 	
}
void TIM3_IRQHandler(void)
{
 	if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET )              //�����ж�
	{
     /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
    CaptureNumberA1++;  // ÿ��һ�������ؼ�һ
	}
 	if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET )              //�����ж�
	{
     /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3,TIM_IT_CC2);
    CaptureNumberA2++;  // ÿ��һ�������ؼ�һ
	}
 	if(TIM_GetITStatus(TIM3, TIM_IT_CC3) == SET )              //�����ж�
	{
     /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3,TIM_IT_CC3);
    CaptureNumberB1++;  // ÿ��һ�������ؼ�һ
	}
 	if(TIM_GetITStatus(TIM3, TIM_IT_CC4) == SET )              //�����ж�
	{
     /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3,TIM_IT_CC4);
    CaptureNumberB2++;  // ÿ��һ�������ؼ�һ
	}	
}
void TIM1_UP_IRQHandler(void)
{
	if(tim1_mode==2){tim1_step=0;}
		else if(tim1_mode==1){tim1_step++;}
	else if(tim1_mode==0){tim1_step--;}
	
}


/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
