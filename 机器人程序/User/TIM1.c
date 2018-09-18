#include "TIM1.h"
#include "MicroStepDriver.h" 
#include "bsp_SysTick.h"
#include "bsp_led.h"
#include "bjdj.h"
static void ADVANCE_TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // ����Ƚ�ͨ�� GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);	
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);	
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
// PWM �źŵ����� T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)

static void ADVANCE_TIM_Mode_Config(void)
{
  // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=1399;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= 71;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ��û�õ����ù�
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/*--------------------����ȽϽṹ���ʼ��-------------------*/		
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// ����ΪPWMģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// ���ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// �������ʹ��
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable; 
	// ����ռ�ձȴ�С
	TIM_OCInitStructure.TIM_Pulse = 1000;
	// ���ͨ����ƽ��������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	// ���ͨ�����е�ƽ��������
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	// �������ͨ�����е�ƽ��������
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);


	
	// ʹ�ܼ�����
	TIM_Cmd(TIM1, ENABLE);	
	// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}
void TIM1_NVIC_Config(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;


 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���


							 
}
void TIM1_Init(void)
{
	ADVANCE_TIM_GPIO_Config();
	ADVANCE_TIM_Mode_Config();		
	//TIM1_NVIC_Config();
}
void TIM1Control(void)
{

	if(status.bjdjt == 1)
    { 
		//	printf("[%d,%d]",x,y);
				LED0=!LED0;	
				PCout(1)=0;//˳
				PCout(2)=1;//˳
				delay_msmoni(1400);	
				LED1=!LED1;
				PCout(1)=0;//˳
				PCout(2)=0;//˳		
				delay_msmoni(1400);	
				PCout(1)=1;//˳
				PCout(2)=1;//˳
				LED0=!LED0;
				LED1=!LED1; 	
			 status.bjdjt = 0;
		}
	if(status.left==1)
	{
		Anticlockwise();//��ʱ��
		delay_msmoni(30);	
		BJDJ_STOP();
		status.left=0;
	}
	if(status.right==1)
	{
		Clockwise();//˳ʱ��
		delay_msmoni(30);
		BJDJ_STOP();
		status.right=0;
	}
	
}

