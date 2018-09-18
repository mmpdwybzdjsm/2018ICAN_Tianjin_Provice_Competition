#include "TIM4.h"
#include "bsp_usart.h"
PID sPID1;
static PID *sptr1 = &sPID1;
PID sPID2;
static PID *sptr2 = &sPID2;
PID sPID3;
static PID *sptr3 = &sPID3;
PID sPID4;
static PID *sptr4 = &sPID4;
extern volatile unsigned int CaptureNumberA1;
extern volatile unsigned int CaptureNumberA2;
extern volatile unsigned int CaptureNumberB1;
extern volatile unsigned int CaptureNumberB2;
void IncPIDInit(void) 
{
    sptr1->LastError=0;            //Error[-1]
    sptr1->PrevError=0;            //Error[-2]
		sptr1->Error=0; 
    sptr1->Proportion=P_DATA;      //�������� Proportional Const
    sptr1->Integral=I_DATA;        //���ֳ���  Integral Const
    sptr1->Derivative=D_DATA;      //΢�ֳ��� Derivative Const
    sptr1->SetPoint=setpoint;           //�趨Ŀ��Desired Value
		sptr1->lastoutput=0;
		sptr1->output=0;
		sptr1->increment=0;
		
    sptr2->LastError=0;            //Error[-1]
    sptr2->PrevError=0;            //Error[-2]
		sptr2->Error=0; 
    sptr2->Proportion=P_DATA;      //�������� Proportional Const
    sptr2->Integral=I_DATA;        //���ֳ���  Integral Const
    sptr2->Derivative=D_DATA;      //΢�ֳ��� Derivative Const
    sptr2->SetPoint=setpoint;           //�趨Ŀ��Desired Value
		sptr2->lastoutput=0;
		sptr2->output=0;
		sptr2->increment=0;

    sptr3->LastError=0;            //Error[-1]
    sptr3->PrevError=0;            //Error[-2]
		sptr3->Error=0; 
    sptr3->Proportion=P_DATA;      //�������� Proportional Const
    sptr3->Integral=I_DATA;        //���ֳ���  Integral Const
    sptr3->Derivative=D_DATA;      //΢�ֳ��� Derivative Const
    sptr3->SetPoint=setpoint;           //�趨Ŀ��Desired Value
		sptr3->lastoutput=0;
		sptr3->output=0;
		sptr3->increment=0;

    sptr4->LastError=0;            //Error[-1]
    sptr4->PrevError=0;            //Error[-2]
		sptr4->Error=0; 
    sptr4->Proportion=P_DATA;      //�������� Proportional Const
    sptr4->Integral=I_DATA;        //���ֳ���  Integral Const
    sptr4->Derivative=D_DATA;      //΢�ֳ��� Derivative Const
    sptr4->SetPoint=setpoint;           //�趨Ŀ��Desired Value
		sptr4->lastoutput=0;
		sptr4->output=0;
		sptr4->increment=0;
}
void pid_calculation(PID *spid,int now,int ch)
{
	//printf("\r\nsss=%d",now);
	spid->Error=spid->SetPoint-now;
	//printf("\r\nerror=%d",spid->Error);
	spid->increment=spid->Proportion*spid->Error-spid->Integral*spid->LastError+spid->Derivative*spid->PrevError;
	
	//printf("\r\nincrement=%d",spid->increment);
	spid->PrevError=spid->LastError;
	//printf("\r\nPrevError=%d",spid->PrevError);
	spid->LastError=spid->Error;
	//printf("\r\nLastError=%d",spid->LastError);
	spid->output=spid->lastoutput+spid->increment;
	
	spid->lastoutput=spid->output;
	if(spid->output>900)spid->output=900;
	if(spid->output<0)spid->output=0;
	//printf("\r\nss=%d",spid->output);
	//printf("11");
	TIM4_SetPWM_Num(spid->output,ch);
	//if(ch==1){printf("\r\n1111");printf("\r\n1s=%d",spid->output);}
	//if(ch==2){printf("\r\n2222");printf("\r\n2s=%d",spid->output);}
	//if(ch==3){printf("\r\n3333");printf("\r\n3s=%d",spid->output);}
	//if(ch==4){printf("\r\n4444");printf("\r\n4s=%d",spid->output);}
		
	
}
int IncPIDCalc1(int NextPoint) 
{
  int iError,iIncpid;                                 //��ǰ���
  iError=sptr1->SetPoint-NextPoint;                    //��������
  iIncpid=(sptr1->Proportion * iError)                 //E[k]��
              -(sptr1->Integral * sptr1->LastError)     //E[k-1]��
              +(sptr1->Derivative * sptr1->PrevError);  //E[k-2]��
              
  sptr1->PrevError=sptr1->LastError;                    //�洢�������´μ���
  sptr1->LastError=iError;
  return(iIncpid);                                    //��������ֵ
	
}
void TIM4_NVIC_Config(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;


 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���


							 
}

void TIM4_Mode_Config(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
//	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5    
 
   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIO
#if 0
	PDout(4)=1;
	PDout(5)=0;
	PDout(6)=1;
	PDout(7)=0;
#endif


TIM_TimeBaseStructure.TIM_Period = 999; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =3; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ 

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

  TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
//	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
    //ʹ��TIM1Ԥװ�ؼĴ���
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1

	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
//	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
    //ʹ��TIM1Ԥװ�ؼĴ���
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1

	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
//	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
    //ʹ��TIM1Ԥװ�ؼĴ���		
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1

	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
//	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
    //ʹ��TIM1Ԥװ�ؼĴ���


  TIM_ARRPreloadConfig(TIM4, ENABLE);  
    //�����ж�Դ��ֻ�����ʱ���ж�
  TIM_UpdateRequestConfig(TIM4,TIM_UpdateSource_Regular);
	// ����жϱ�־λ
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    // ʹ���ж�
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM3
	//100//211 1s
	//500//1190
	TIM_SetCompare1(TIM4,500);
	TIM_SetCompare2(TIM4,500);
	TIM_SetCompare3(TIM4,500);
	TIM_SetCompare4(TIM4,500);
	TIM_SetCompare1(TIM4,0);
	TIM_SetCompare2(TIM4,0);
	TIM_SetCompare3(TIM4,0);
	TIM_SetCompare4(TIM4,0);
}
void TIM4_Init(void)
{
	TIM4_NVIC_Config();
	TIM4_Mode_Config();
}
void TIM4_SetPWM_Num(u16 value,u8 ch)
{
	if (ch==1)
	{
		TIM_SetCompare1(TIM4,value);
	}
	if (ch==2)
	{
		TIM_SetCompare2(TIM4,value);
	}
	if (ch==3)
	{
		TIM_SetCompare3(TIM4,value);
	}
	if (ch==4)
	{
		TIM_SetCompare4(TIM4,value);
	}
}
void Stop_it(void)
{	
TIM4_SetPWM_Num(0,1);
	TIM4_SetPWM_Num(0,2);
	TIM4_SetPWM_Num(0,3);
	TIM4_SetPWM_Num(0,4);
}
void forward(void)
{	
			PDout(4)=1;
			PDout(5)=0;
			PDout(6)=1;
			PDout(7)=0;
			pid_calculation(&sPID1,CaptureNumberA1,1);

			CaptureNumberA1=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID2,CaptureNumberA2,2);
			CaptureNumberA2=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID3,CaptureNumberB1,3);
			CaptureNumberB1=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID4,CaptureNumberB2,4);			
			CaptureNumberB2=0;    // ���㣬���㿪ʼ����
}
void backward(void)
{	
			PDout(4)=0;
			PDout(5)=1;
			PDout(6)=0;
			PDout(7)=1;
			pid_calculation(&sPID1,CaptureNumberA1,1);

			CaptureNumberA1=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID2,CaptureNumberA2,2);
			CaptureNumberA2=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID3,CaptureNumberB1,3);
			CaptureNumberB1=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID4,CaptureNumberB2,4);			
			CaptureNumberB2=0;    // ���㣬���㿪ʼ����
}
void leftward(void)
{	
			PDout(4)=0;
			PDout(5)=0;
			PDout(6)=1;
			PDout(7)=1;
			pid_calculation(&sPID1,CaptureNumberA1,1);

			CaptureNumberA1=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID2,CaptureNumberA2,2);
			CaptureNumberA2=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID3,CaptureNumberB1,3);
			CaptureNumberB1=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID4,CaptureNumberB2,4);			
			CaptureNumberB2=0;    // ���㣬���㿪ʼ����
}
void rightward(void)
{	
			PDout(4)=1;
			PDout(5)=1;
			PDout(6)=0;
			PDout(7)=0;
			pid_calculation(&sPID1,CaptureNumberA1,1);

			CaptureNumberA1=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID2,CaptureNumberA2,2);
			CaptureNumberA2=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID3,CaptureNumberB1,3);
			CaptureNumberB1=0;    // ���㣬���㿪ʼ����
			pid_calculation(&sPID4,CaptureNumberB2,4);			
			CaptureNumberB2=0;    // ���㣬���㿪ʼ����
}
