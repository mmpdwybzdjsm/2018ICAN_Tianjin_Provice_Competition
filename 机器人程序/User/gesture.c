#include "gesture.h"
#include "simulate.h"
#include "key.h"
#include "bsp_usart.h"
#include "paj7620u2_cfg.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"
#include "lcd.h"
#include "simulate.h"
//ѡ��PAJ7620U2 BANK����
void paj7620u2_selectBank(bank_e bank)
{
	switch(bank)
	{
		case BANK0: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK0);break;//BANK0�Ĵ�������
		case BANK1: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK1);break;//BANK1�Ĵ�������
	}
			
}

//PAJ7620U2����
u8 paj7620u2_wakeup(void)
{ 
	u8 data=0x0a;
	GS_WakeUp();//����PAJ7620U2
	Delay_ms(5);//����ʱ��>700us
	GS_WakeUp();//����PAJ7620U2
	Delay_ms(5);//����ʱ��>700us
	paj7620u2_selectBank(BANK0);//����BANK0�Ĵ�������
	data = GS_Read_Byte(0x00);//��ȡ״̬
	if(data!=0x20) return 0; //����ʧ��
	
	return 1;
}

//PAJ7620U2��ʼ��
//����ֵ��0:ʧ�� 1:�ɹ�
u8 paj7620u2_init(void)
{
	u8 i;
	u8 status;
	
	i2c_CfgGpio();
	status = paj7620u2_wakeup();//����PAJ7620U2
	if(!status) return 0;
    paj7620u2_selectBank(BANK0);//����BANK0�Ĵ�������
	for(i=0;i<INIT_SIZE;i++)
	{
		GS_Write_Byte(init_Array[i][0],init_Array[i][1]);//��ʼ��PAJ7620U2
	}
    paj7620u2_selectBank(BANK0);//�л���BANK0�Ĵ�������
	
	return 1;
}

//���˵�
#if 0
void paj7620u2_test_ui(void)
{
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_Fill(30,170,300,300,WHITE);
	LCD_ShowString(30,170,200,16,16,"KEY1:   Gesrure test");//����ʶ�����
	LCD_ShowString(30,190,200,16,16,"KEY0:   Ps test     ");//�ӽ��������
	
}
#endif
//����ʶ�����
void Gesrure_test(void)
{
	u8 i;
    u8 status;
	u8 key;
	u8 data[2]={0x00};
	u16 gesture_data;
	u8 ledflash=0;
	
	paj7620u2_selectBank(BANK0);//����BANK0
	for(i=0;i<GESTURE_SIZE;i++)
	{
		GS_Write_Byte(gesture_arry[i][0],gesture_arry[i][1]);//����ʶ��ģʽ��ʼ��
	}
	paj7620u2_selectBank(BANK0);//�л���BANK0
	i=0;
	//POINT_COLOR=BLUE;//��������Ϊ��ɫ
	//LCD_Fill(30,170,300,300,WHITE);
	//LCD_ShowString(30,180,200,16,16,"KEY_UP: Exit the test");
	//LCD_ShowString(30,210,200,16,16,"Gesrure test");
	//POINT_COLOR=RED;//��������Ϊ��ɫ
	while(1)
	{
        key = KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			GS_Write_Byte(PAJ_SET_INT_FLAG1,0X00);//�ر�����ʶ���ж����
			GS_Write_Byte(PAJ_SET_INT_FLAG2,0X00);
			break;
		}			
        status = GS_Read_nByte(PAJ_GET_INT_FLAG1,2,&data[0]);//��ȡ����״̬			
		if(!status)
		{   
			gesture_data =(u16)data[1]<<8 | data[0];
			if(gesture_data) 
			{
				switch(gesture_data)
				{
					case GES_UP:               //LCD_ShowString(110,250,200,16,24,"UP          ");
					                           printf("Up\r\n");            ledflash=1;      break; //����
					case GES_DOWM:             //LCD_ShowString(100,250,200,16,24,"Dowm        ");      
               						           printf("Dowm\r\n");          ledflash=1;      break; //����
					case GES_LEFT:             //LCD_ShowString(100,250,200,16,24,"Left        ");           
  						                       printf("Left\r\n");          ledflash=1;      break; //����
					case GES_RIGHT:            //LCD_ShowString(100,250,200,16,24,"Right       ");       
                						       printf("Right\r\n");         ledflash=1;      break; //����
					case GES_FORWARD:          //LCD_ShowString(80,250,200,16,24,"Forward     ");        
						                       printf("Forward\r\n");       ledflash=1;      break; //��ǰ
					case GES_BACKWARD:         //LCD_ShowString(80,250,200,16,24,"Backward    ");    
            						           printf("Backward\r\n");      ledflash=1;      break; //���
					case GES_CLOCKWISE:      //  LCD_ShowString(70,250,200,16,24,"Clockwise   ");     
                						       printf("Clockwise\r\n");     ledflash=1;      break; //˳ʱ��
					case GES_COUNT_CLOCKWISE:  //LCD_ShowString(50,250,200,16,24,"AntiClockwise");  
                   						       printf("AntiClockwise\r\n"); ledflash=1;      break; //��ʱ��
					case GES_WAVE:           //  LCD_ShowString(100,250,200,16,24,"Wave         ");    
						                       printf("Wave\r\n");          ledflash=1;      break; //�Ӷ�
					default:  ledflash=0; break;
					
				}	
                if(ledflash)//DS1��˸
				{   
					LED1=0;Delay_ms(80);LED1=1;Delay_ms(80);
					LED1=0;Delay_ms(80);LED1=1;Delay_ms(80);
					Delay_ms(300);
		//			LCD_ShowString(40,250,200,16,24,"                        ");
					ledflash=0;
				}						
			}
			
		}
		Delay_ms(50);
		i++;
		if(i==5)
		{
			LED0=!LED0;//��ʾϵͳ��������	
			i=0;
		}		   
	}
}

//�ӽ�������
void Ps_test(void)
{
	u8 i;
	u8 key;
	u8 data[2]={0x00};
	u8 obj_brightness=0;
	u16 obj_size=0;
	
	paj7620u2_selectBank(BANK0);//����BANK0
	for(i=0;i<PROXIM_SIZE;i++)
	{
		GS_Write_Byte(proximity_arry[i][0],proximity_arry[i][1]);//�ӽ����ģʽ��ʼ��
	}
	paj7620u2_selectBank(BANK0);//�л���BANK0
	i=0;
//	POINT_COLOR=BLUE;//��������Ϊ��ɫ
//	LCD_Fill(30,170,300,300,WHITE);
//	LCD_ShowString(30,180,200,16,16,"KEY_UP: Exit the test");
//	LCD_ShowString(30,210,200,16,16,"Ps test");
//	LCD_ShowString(30,240,200,16,16,"Brightness");
//	LCD_ShowString(160,240,200,16,16,"Size");
//	POINT_COLOR=RED;//��������Ϊ��ɫ	
	
	while(1)
	{	
		key = KEY_Scan(0);
		if(key==WKUP_PRES) break;
		
		obj_brightness = GS_Read_Byte(PAJ_GET_OBJECT_BRIGHTNESS);//��ȡ��������
		data[0] = GS_Read_Byte(PAJ_GET_OBJECT_SIZE_1);//��ȡ�����С
		data[1] = GS_Read_Byte(PAJ_GET_OBJECT_SIZE_2);
		obj_size = ((u16)data[1] & 0x0f)<<8 | data[0];
//		LCD_ShowxNum(50,270,obj_brightness,3,24,0);
//		LCD_ShowxNum(152,270,obj_size,3,24,0);
		printf("obj_brightness: %d\r\n",obj_brightness);
        printf("obj_size: %d\r\n",obj_size);
		
		Delay_ms(100);
		i++;
		if(i==5)
		{
		    LED0=!LED0;//��ʾϵͳ��������	
			i=0;
		}
	}
	
}
//PAJ7620U2����������
void paj7620u2_sensor_test(void)
{   
	 u8 i=0;
	 u8 key;
	
 //  paj7620u2_test_ui();//���˵���ʾ
	 while(1)
	 {
		 key = KEY_Scan(0);//����ɨ��
		 if(key)
		 {
			 switch(key)
			 {
				 case KEY1_PRES:  Gesrure_test();   break;//���Ƽ��ģʽ
				 case KEY0_PRES:  Ps_test();        break;//�ӽ����ģʽ  
			 }
		//	 paj7620u2_test_ui();
		 }
		Delay_ms(50);
		i++;
		if(i==5)
		{
		    LED0=!LED0;//��ʾϵͳ��������	
			i=0;
		}
		 
	 }
}

//PAJ7620U2дһ���ֽ�����
u8 GS_Write_Byte(u8 REG_Address,u8 REG_data)
{
	i2c_Start();
	i2c_SendByte(PAJ7620_ID);
	if(i2c_WaitAck())
	{
		i2c_Stop();//�ͷ�����
		return 1;//ûӦ�����˳�

	}
	i2c_SendByte(REG_Address);
	i2c_WaitAck();	
	i2c_SendByte(REG_data);
	i2c_WaitAck();	
	i2c_Stop();

	return 0;
}

//PAJ7620U2��һ���ֽ�����
u8 GS_Read_Byte(u8 REG_Address)
{
	u8 REG_data;
	
	i2c_Start();
	i2c_SendByte(PAJ7620_ID);//��д����
	if(i2c_WaitAck())
	{
		 i2c_Stop();//�ͷ�����
		 return 0;//ûӦ�����˳�
	}		
	i2c_SendByte(REG_Address);
	i2c_WaitAck();
	i2c_Start(); 
	i2c_SendByte(PAJ7620_ID|0x01);//��������
	i2c_WaitAck();
	REG_data = i2c_ReadByte();
	i2c_NAck();
	i2c_Stop();

	return REG_data;
}
//PAJ7620U2��n���ֽ�����
u8 GS_Read_nByte(u8 REG_Address,u16 len,u8 *buf)
{
	i2c_Start();
	i2c_SendByte(PAJ7620_ID);//��д����
	if(i2c_WaitAck()) 
	{
		i2c_Stop();//�ͷ�����
		return 1;//ûӦ�����˳�
	}
	i2c_SendByte(REG_Address);
	i2c_WaitAck();

	i2c_Start();
	i2c_SendByte(PAJ7620_ID|0x01);//��������
	i2c_WaitAck();
	while(len)
	{
		if(len==1)
		{
			*buf = i2c_ReadByte();
			i2c_NAck();
		}
		else
		{
			*buf = i2c_ReadByte();
			i2c_Ack();
		}
		buf++;
		len--;
	}
	i2c_Stop();//�ͷ�����

	return 0;
	
}
//PAJ7620U2����
void GS_WakeUp(void)
{
	i2c_Start();
	i2c_SendByte(PAJ7620_ID);//��д����
	i2c_Stop();//�ͷ�����
}

/*********************************************END OF FILE**********************/
