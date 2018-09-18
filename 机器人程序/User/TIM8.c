#include "TIM8.h"
#include "bsp_SysTick.h"
#include "bsp_usart.h"
#include "MicroStepDriver.h" 
PD Steer;
int steerduty=3900;
void PD_init(PD *pdvar)
{
    pdvar->point = 120;  
    pdvar->error = 0;
    pdvar->last_error = 0;
    pdvar->duty = 3900;
    pdvar->last_output=0;
		
    pdvar->Proportion_1 = 0.5;
    pdvar->Derivative_1 = 0.1;    
}

void PD_cal(PD *pdvar,int Mid)
{

    pdvar->error =  pdvar->point-Mid;   //����
 //   pdvar->Gyro_error=(GetData(GYRO_XOUT_H)>>3)+5;
    
    //if(Speed_UP == 1 || Speed_Mid == 1)
    //{
        pdvar->duty = pdvar->Proportion_1 * pdvar->error
              //      - pdvar->Derivative_1*pdvar->Gyro_error
                      + pdvar->last_output;    
   if(pdvar->duty >= DUTY_MAX)                                //���PID��ֹ��ߺ���������
     pdvar->duty = DUTY_MAX;
   else if(pdvar->duty <= DUTY_MIN)
      pdvar->duty = DUTY_MIN; 
		TIM_SetCompare3(TIM8,pdvar->duty);
	 pdvar->last_output=pdvar->duty;
}
static void ADVANCE_TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // ����Ƚ�ͨ�� GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);

/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=19999;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= 35;	//144mhz
	//1000 000
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ��û�õ����ù�
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
  TIM_ClearITPendingBit(TIM8,TIM_IT_Update);
	TIM_UpdateRequestConfig(TIM8,TIM_UpdateSource_Regular); //����ֻ�м��������Ϊ�����ж�
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

	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM8, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
	
	// ʹ�ܼ�����
	TIM_Cmd(TIM8, ENABLE);	
	// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ

}
void TIM8_NVIC_Config(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;


 
	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���


							 
}
void TIM8_Init(void)
{
ADVANCE_TIM_GPIO_Config();	
	ADVANCE_TIM_Mode_Config();
	//TIM8_NVIC_Config();
}
void Steeringtest(void)//2500��
{
	int iii;
		for(iii=2900;iii<4900;iii+=100){//2300  1200
		TIM_SetCompare3(TIM8,iii);delay_msmoni(300);printf("%d\r\n",iii);	 }
		iii=4900;
		for(;iii>2900;iii-=100)
		{
			TIM_SetCompare3(TIM8,iii);delay_msmoni(300);	 printf("%d\r\n",iii);
		}
}
void TIM8Comtrol(void)
{
		if(status.up==1)
	{
		steerduty+=100;
		if(steerduty>4900)steerduty=4900;
		TIM_SetCompare3(TIM8,steerduty);
		

		status.up=0;
	}
	if(status.down==1)
	{
		steerduty-=100;
		if(steerduty<2900)steerduty=2900;
		TIM_SetCompare3(TIM8,steerduty);
		status.down=0;
	}
}
