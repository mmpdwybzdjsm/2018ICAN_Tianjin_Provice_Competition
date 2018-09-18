#include "bjdj.h"
void BJDJ_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����LED��ص�GPIO����ʱ��*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin =GPIO_Pin_3;	

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ��GPIO*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
		
		/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;

		/*���ÿ⺯������ʼ��GPIO*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		


		/* �ر�����led��	*/
		GPIO_SetBits(GPIOC,GPIO_Pin_3);
		
		/* �ر�����led��	*/
		GPIO_SetBits(GPIOC,GPIO_Pin_4);	 
    

}
void Clockwise(void)//˳ʱ��
{
	PCout(3) =0;
	PCout(4)=1;
}
void Anticlockwise(void)//��ʱ��
{
	PCout(3) =0;
	PCout(4)=0;
}
void BJDJ_STOP(void)
{
	PCout(3) =1;
	PCout(4)=1;
}
