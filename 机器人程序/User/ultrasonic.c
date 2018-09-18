#include "ultrasonic.h"
#include "bsp_usart.h"
uint32_t TIM_PscCLK = 1000000;
//TIM2
// ��ʱ�����벶���û��Զ�������ṹ�嶨��
TIM_ICUserValueTypeDef ultrasonic1 = {0,0,0,0,0,0};
TIM_ICUserValueTypeDef ultrasonic2 = {0,0,0,0,0,0};
TIM_ICUserValueTypeDef ultrasonic3 = {0,0,0,0,0,0};
TIM_ICUserValueTypeDef ultrasonic4 = {0,0,0,0,0,0};
// �ж����ȼ�����
static void TIM2_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn ;	
		// ���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void TIM2_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // ���벶��ͨ�� GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
 	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_6;				 //LED0-->PB.5 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	 GPIO_ResetBits(GPIOE,GPIO_Pin_0);						 //Pe0��	
	 GPIO_ResetBits(GPIOE,GPIO_Pin_1);						 //Pe0��	
	 GPIO_ResetBits(GPIOE,GPIO_Pin_6);						 //Pe0��		 
}


///*
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *	TIM_CounterMode			     TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM�ź� ���ں�ռ�ձȵļ���--------------- */
// ARR ���Զ���װ�ؼĴ�����ֵ
// CLK_cnt����������ʱ�ӣ����� Fck_int / (psc+1) = 72M/(psc+1)
// PWM �źŵ����� T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)

static void TIM2_Mode_Config(void)
{
  // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

/*--------------------ʱ���ṹ���ʼ��-------------------------*/	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=0XFFFF;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= 71;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ��û�õ����ù�
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/*--------------------���벶��ṹ���ʼ��-------------------*/	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	// �������벶���ͨ������Ҫ���ݾ����GPIO������
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	// ���벶���źŵļ�������
	TIM_ICInitStructure.TIM_ICPolarity = GENERAL_TIM_STRAT_ICPolarity;
	// ����ͨ���Ͳ���ͨ����ӳ���ϵ����ֱ���ͷ�ֱ������
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// �������Ҫ��������źŵķ�Ƶϵ��
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// �������Ҫ��������źŵ��˲�ϵ��
	TIM_ICInitStructure.TIM_ICFilter = 0;
	// ��ʱ�����벶���ʼ��
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	
	// ������ºͲ����жϱ�־λ
  TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_IT_CC1);	
  // �������ºͲ����ж�  
	TIM_ITConfig (TIM2, TIM_IT_Update | TIM_IT_CC1, ENABLE );
	#if 0
	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_IT_CC2);	
  // �������ºͲ����ж�  
	TIM_ITConfig (TIM2, TIM_IT_Update | TIM_IT_CC2, ENABLE );
	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_IT_CC3);	
  // �������ºͲ����ж�  
	TIM_ITConfig (TIM2, TIM_IT_Update | TIM_IT_CC3, ENABLE );
	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_IT_CC4);	
  // �������ºͲ����ж�  
	TIM_ITConfig (TIM2, TIM_IT_Update | TIM_IT_CC4, ENABLE );
#endif

	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_IT_CC3);	
  // �������ºͲ����ж�  
	TIM_ITConfig (TIM2, TIM_IT_Update | TIM_IT_CC3, ENABLE );
	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_IT_CC4);	
  // �������ºͲ����ж�  
	TIM_ITConfig (TIM2, TIM_IT_Update | TIM_IT_CC4, ENABLE );
	// ʹ�ܼ�����
	TIM_Cmd(TIM2, ENABLE);
}

void TIM2_Init(void)
{
	TIM2_GPIO_Config();
	TIM2_NVIC_Config();
	TIM2_Mode_Config();		
}
void CSB1(void)
{
						if(ultrasonic1.Capture_FinishFlag == 1)
							{
								// ����ߵ�ƽʱ��ļ�������ֵ
								ultrasonic1.time = ultrasonic1.Capture_Period * (0XFFFF+1) + 
											 (ultrasonic1.Capture_CcrValue+1);
								ultrasonic1.distance=ultrasonic1.time%TIM_PscCLK*340/2/1000;
								// ��ӡ�ߵ�ƽ����ʱ��
								//printf ( "\r\n��øߵ�ƽ����ʱ�䣺%d.%d s\r\n",time/TIM_PscCLK,time%TIM_PscCLK );
								printf ( "\r\n��������1��%d mm\r\n",ultrasonic1.distance );
								ultrasonic1.Capture_FinishFlag = 0;			
							}	
}
#if 1
void CSB2(void)
{
						if(ultrasonic2.Capture_FinishFlag == 1)
							{
								// ����ߵ�ƽʱ��ļ�������ֵ
								ultrasonic2.time = ultrasonic2.Capture_Period * (0XFFFF+1) + 
											 (ultrasonic2.Capture_CcrValue+1);
								ultrasonic2.distance=ultrasonic2.time%TIM_PscCLK*340/2/1000;
								// ��ӡ�ߵ�ƽ����ʱ��
								//printf ( "\r\n��øߵ�ƽ����ʱ�䣺%d.%d s\r\n",time/TIM_PscCLK,time%TIM_PscCLK );
								printf ( "\r\n��������2��%d mm\r\n",ultrasonic2.distance );
								ultrasonic2.Capture_FinishFlag = 0;			
							}	
}
void CSB3(void)
{
						if(ultrasonic3.Capture_FinishFlag == 1)
							{
								// ����ߵ�ƽʱ��ļ�������ֵ
								ultrasonic3.time = ultrasonic3.Capture_Period * (0XFFFF+1) + 
											 (ultrasonic3.Capture_CcrValue+1);
								ultrasonic3.distance=ultrasonic3.time%TIM_PscCLK*340/2/1000;
								// ��ӡ�ߵ�ƽ����ʱ��
								//printf ( "\r\n��øߵ�ƽ����ʱ�䣺%d.%d s\r\n",time/TIM_PscCLK,time%TIM_PscCLK );
								printf ( "\r\n��������3��%d mm\r\n",ultrasonic3.distance );
								ultrasonic3.Capture_FinishFlag = 0;			
							}	
}
void CSB4(void)
{
						if(ultrasonic4.Capture_FinishFlag == 1)
							{
								// ����ߵ�ƽʱ��ļ�������ֵ
								ultrasonic4.time = ultrasonic4.Capture_Period * (0XFFFF+1) + 
											 (ultrasonic4.Capture_CcrValue+1);
								ultrasonic4.distance=ultrasonic4.time%TIM_PscCLK*340/2/1000;
								// ��ӡ�ߵ�ƽ����ʱ��
								//printf ( "\r\n��øߵ�ƽ����ʱ�䣺%d.%d s\r\n",time/TIM_PscCLK,time%TIM_PscCLK );
								printf ( "\r\n��������4��%d mm\r\n",ultrasonic4.distance );
								ultrasonic4.Capture_FinishFlag = 0;			
							}	
}
#endif

