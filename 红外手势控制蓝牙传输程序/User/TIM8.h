#ifndef __TIM8_H_
#define __TIM8_H_
#include <stm32f10x.h>
#define DUTY_MAX  4900
#define DUTY_MIN  2900      
#define DUTY_MID  3900
#define Coefficient_Trh    10               //�������ƫ��ϵ����ֵ 

typedef struct PD {
 
   //  unsigned char setpoint;       //�趨ֵ
     int point;
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


void TIM8_Init(void);
void Steeringtest(void);
void TIM8Comtrol(void);
#endif
