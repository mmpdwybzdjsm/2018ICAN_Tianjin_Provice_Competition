#ifndef __BEEP_H
#define	__BEEP_H


#include "stm32f10x.h"


/* ����LED���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
// R-��ɫ
#define BEEP_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define BEEP_GPIO_PIN		GPIO_Pin_8			        /* ���ӵ�SCLʱ���ߵ�GPIO */


// B-��ɫ
//#define LED3_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
//#define LED3_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
//#define LED3_GPIO_PIN		GPIO_Pin_1			        /* ���ӵ�SCLʱ���ߵ�GPIO */


/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */



//#define LED3(a)	if (a)	\
					GPIO_SetBits(LED3_GPIO_PORT,LED3_GPIO_PIN);\
					else		\
					GPIO_ResetBits(LED3_GPIO_PORT,LED3_GPIO_PIN)


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		 {p->BRR=i;}	 //����͵�ƽ
#define digitalToggle(p,i) {p->ODR ^=i;} //�����ת״̬


/* �������IO�ĺ� */

#define BEEP_OFF		   digitalHi(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_ON			   digitalLo(BEEP_GPIO_PORT,BEEP_GPIO_PIN)


//#define LED3_TOGGLE		 digitalToggle(LED3_GPIO_PORT,LED3_GPIO_PIN)
//#define LED3_OFF		   digitalHi(LED3_GPIO_PORT,LED3_GPIO_PIN)
//#define LED3_ON			   digitalLo(LED3_GPIO_PORT,LED3_GPIO_PIN)

/* ������ɫ������߼��÷�ʹ��PWM�ɻ��ȫ����ɫ,��Ч������ */


void BEEP_GPIO_Config(void);

#endif /* __LED_H */
