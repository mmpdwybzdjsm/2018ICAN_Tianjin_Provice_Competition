#ifndef __STEERING_H
#define __STEERING_H
#include "stm32f10x.h"
#define DUTY_MAX  2400
#define DUTY_MIN  1100      
#define DUTY_MID  1750
#define Coefficient_Trh    10               //�������ƫ��ϵ����ֵ 

typedef struct PD {
 
     unsigned char setpoint;          //�趨ֵ
     
     float Proportion_1;        //����ϵ��
     float Derivative_1;        //΢��ϵ��
		 int last_output;
		
     
     int error;         //ƫ��
     int last_error;        //��һ��ƫ��
     int Gyro_error;
     int duty;             //ռ�ձȵ�ֵ
}PD;
//��ʼ��PD����
void PD_init(PD *pdvar);
//PD�ļ���
void PD_cal(PD *pdvar,int Mid);


void TIM2_Int_Init(u16 arr,u16 psc);
void TIM2_PWM_Init(u16 arr,u16 psc);
void Steeringtest(void);
#endif

