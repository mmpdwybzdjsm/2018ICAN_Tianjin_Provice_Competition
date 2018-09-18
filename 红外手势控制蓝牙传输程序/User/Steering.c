#include "Steering.h"
#include "bsp_SysTick.h"
#include "bsp_usart.h"
PD Steer;
void PD_init(PD *pdvar)
{
    pdvar->setpoint = 120;  
    pdvar->error = 0;
    pdvar->last_error = 0;
    pdvar->duty = 120;
    pdvar->last_output=0;
		
    pdvar->Proportion_1 = 0.5;
    pdvar->Derivative_1 = 0.1;    
}

void PD_cal(PD *pdvar,int Mid)
{

    pdvar->error =   Mid-pdvar->setpoint;   
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
		TIM_SetCompare2(TIM2,pdvar->duty);
	 pdvar->last_output=pdvar->duty;
}

void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����
							 
}





//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM2_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
//	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5    
 
   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	// �����������
//	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; 	
	// ����ռ�ձȴ�С
//	TIM_OCInitStructure.TIM_Pulse = 30;


  TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	// ���ͨ�����е�ƽ��������
//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;	

  TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
//	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
    //ʹ��TIM1Ԥװ�ؼĴ���
  TIM_ARRPreloadConfig(TIM2, ENABLE);  
    //�����ж�Դ��ֻ�����ʱ���ж�
  TIM_UpdateRequestConfig(TIM2,TIM_UpdateSource_Regular);
	// ����жϱ�־λ
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    // ʹ���ж�
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM3
	

}
void Steeringtest(void)//2500��
{
	int i;
		for(i=1100;i<2500;i+=100){//2300  1200
		TIM_SetCompare2(TIM2,i);Delay_ms(500);printf("%d\r\n",i);	 }
		i=2400;
		for(;i>1000;i-=100)
		{
			TIM_SetCompare2(TIM2,i);Delay_ms(500);	 printf("%d\r\n",i);
		}
}

