#include "nrf24l01.h"
#include "stm32f10x.h"
#define CE_LOW					GPIOB->BRR |= GPIO_Pin_0
#define CE_HIGH					GPIOB->BSRR |= GPIO_Pin_0
#define TX_ADR_WIDTH    		5   		//�����ַ�ǽ��ն˵Ľ���ͨ���ĵ�ַ�Ŀ��
#define RX_ADR_WIDTH    		5   		//����Ǳ�������ͨ��0�ĵ�ַ���
const u8 TX_ADDRESS_MB[TX_ADDR_WIDTH]={0x34,0x43,0x10,0x10,0xff};	 //���͵�ַ
const u8 RX_ADDRESS_MB[RX_ADDR_WIDTH]={0x34,0x43,0x10,0x10,0xff};	//���յ�ַ		
extern u8  TX_ADDRESS[TX_ADR_WIDTH];	//�˵�ַ����ʶ����ն��ĸ�RXͨ�����Խ��շ��ͳ�ȥ�����ݰ�
extern u8  RX_ADDRESS[RX_ADR_WIDTH];	//�˵�ַ�������ñ���NRF��RX0ͨ���ĵ�ַ��ͬʱΪ���������յ�Ӧ���źţ��˵�ַһ�㶼������ĵ�ַ������ͬ
#define WRITE_REG_CMD       	0x20 		// д�Ĵ���ָ��
/***************************************************************************
 * ��  �� : SPI��д���ݺ�������дһ���ֽ�
 * ��  �� : Dat��д�������
 * ����ֵ : ��ȡ������
 **************************************************************************/
u8 NRF24L01_MB_Write_Reg(u8 regaddr,u8 dat);
u8 NRF24L01_Write_Buf(u8 regaddr, u8 *pBuf, u8 datalen);
u8 SPI1_ReadWriteByte(u8 Dat)                                       
{		
	u8 retry=0;				 	
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)      //���ͻ����־λΪ��
	{
		retry++;
		if(retry>200)return 0;
	}			  
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, Dat);                                      //ͨ������SPI1����һ������
	retry=0;
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)   //���ջ����־λ��Ϊ��
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);                                 //ͨ��SPI1���ؽ�������				    
}

/***************************************************************************
 * ��  �� : �������ں�NRF24L01ģ��ͨѶ��SPI����
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************/
void NRF24L01_MB_SPI_Init(void)
{	 
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
    
  // Enable SPI2 and GPIO clocks
  RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1 , ENABLE); //SPI2ʱ��ʹ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);


  //SPI2�ڳ�ʼ��������PB13��PB15Ϊ��push-pull - SCK,MOSI
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


	
	
	GPIO_SetBits(GPIOA,GPIO_Pin_5| GPIO_Pin_6 | GPIO_Pin_7);          //PB13/14/15����
	SPI_Cmd(SPI1, DISABLE); 
	/* SPI2 configuration */                                              //��ʼ��SPI�ṹ��
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //SPI����Ϊ˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master ;		                      //����SPIΪ��ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                  //SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                        //SPIʱ�ӿ���ʱΪ�͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	                        //��һ��ʱ���ؿ�ʼ��������
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                          //NSS�ź��������ʹ��SSIλ������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;   //SPI2������Ԥ��ƵֵΪ32
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                            //CRCֵ����Ķ���ʽ


	SPI_Init(SPI1, &SPI_InitStructure);      //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPI2�Ĵ���
	SPI_Cmd(SPI1, ENABLE);                   //ʹ��SPI2����
//	SPI1_ReadWriteByte(0xff);  
} 

/***************************************************************************
 * ��  �� : NRF24L01��ʼ����������ʼ������NRF24L01ģ��Ĺܽţ�����SPI��ʼ������
 *          ��ɺ�NRF24L01ģ��ͨѶ��SPI���ߵĳ�ʼ��
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************/
void Init_NRF24L01_MB(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_initStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	/* ����SPI2Ƭѡ */
	RCC_APB2PeriphClockCmd(RCC_NRF24L01_MB_CS, ENABLE);   //ʹ��GPIO��ʱ��
  GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_CS;      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_CS, &GPIO_InitStructure);
	
	/* NRF24L01ʹ�ܿ��ƹܽ� */
	RCC_APB2PeriphClockCmd(RCC_NRF24L01_MB_CE, ENABLE);    //ʹ��GPIO��ʱ��
  GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_CE;          //NRF24L01 ģ��Ƭѡ�ź�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_CE, &GPIO_InitStructure);
	
	/* ����NRF24L01ģ��ָʾ�ƿ��ƹܽ� */
  RCC_APB2PeriphClockCmd( RCC_NRF24L01_MB_LED , ENABLE); 						 			
	GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_LED;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_LED, &GPIO_InitStructure);
	
	CE_MB_HIGH;                                    //��ʼ��ʱ������
  CS_MB_HIGH;                                    //��ʼ��ʱ������

	/* ���û�ȡNRF24L01ģ��IRQ�Ĺܽ� */
	GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_IRQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_IRQ, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_NRF24L01_MB_IRQ,NRF24L01_MB_IRQ);

	NRF24L01_MB_SPI_Init();                      //��ʼ��SPI
	CE_MB_LOW; 	                                 //ʹ��NRF24L01
	CS_MB_HIGH;                                  //SPIƬѡȡ��
	EXTI_initStructure.EXTI_Line = EXTI_Line1;
	EXTI_initStructure.EXTI_LineCmd = ENABLE;
	EXTI_initStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_initStructure.EXTI_Trigger = EXTI_Trigger_Falling;					//�½��ش���
	EXTI_Init(&EXTI_initStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);			//����GPIO�ܽŵ��ж���·
	NVIC_EnableIRQ(EXTI1_IRQn);	
	
	CE_LOW;			//����CE��ע�⣺��/дnRF�Ĵ�������Ҫ��CE���ͣ�ʹ�����������ߵ���ģʽ�ſ���
	
	//��ʼ��NRF
	NRF24L01_MB_Write_Reg(WRITE_REG_CMD + SETUP_AW, 0x03);							//����ͨ�ŵ�ַ�ĳ��ȣ�Ĭ��ֵʱ0x03,����ַ����Ϊ5�ֽ�
	NRF24L01_Write_Buf(WRITE_REG_CMD + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    	
	NRF24L01_Write_Buf(WRITE_REG_CMD + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); 	
	NRF24L01_MB_Write_Reg(WRITE_REG_CMD + SETUP_RETR, 0x1a); 						//�Զ��ط��ӳ�Ϊ500+86us���ط�����10��
	NRF24L01_MB_Write_Reg(WRITE_REG_CMD + EN_AA, 0x01);      						//�������ݺ�ֻ����Ƶ��0�Զ�Ӧ��
	NRF24L01_MB_Write_Reg(WRITE_REG_CMD + EN_RXADDR, 0x01);  						//ֻ����Ƶ��0��������
	
	NRF24L01_MB_Write_Reg(WRITE_REG_CMD + RF_SETUP, 0x27);   						//���÷�������Ϊ2MHZ�����书��Ϊ���ֵ0dB
	NRF24L01_MB_Write_Reg(WRITE_REG_CMD + RF_CH, 30);        						//����ͨ��ͨ��Ƶ�ʣ�����ͨ��Ƶ�ʿ������¹�ʽ����ó���Fo=��2400+RF-CH��MHz.������Ƶ�շ���������Ƶ�ʷ�Χ��2.400-2.525GHz
	NRF24L01_MB_Write_Reg(WRITE_REG_CMD + RX_PW_P0, RX_PLOAD_WIDTH); 				//���ý������ݳ��ȣ���������Ϊ5�ֽڣ�ֻ�н��յ����ݴﵽ�˳���ʱ���Żᴥ��RX_DS�ж�
	
	NRF24L01_MB_Write_Reg(WRITE_REG_CMD + CONFIG, 0x0f);   						//Ĭ�ϴ��ڽ���ģʽ

	//����������Ϣ����ֹ���ó������Դ����ʱ�����ʹ�ã������������д���ʱ��ע�͵���Щ��ӡ���룩
//	printf("SETUP_AW:%x\r\n",SPI_Read_Byte(READ_REG_CMD+SETUP_AW));
//	printf("SETUP_RETR:%x\r\n",SPI_Read_Byte(READ_REG_CMD+SETUP_RETR));
//	printf("EN_RXADDR:%x\r\n",SPI_Read_Byte(READ_REG_CMD+EN_RXADDR));
//	printf("EN_AA:%x\r\n",SPI_Read_Byte(READ_REG_CMD+EN_AA));
//	printf("RF_SETUP:%x\r\n",SPI_Read_Byte(READ_REG_CMD+RF_SETUP));
//	printf("RF_CH:%x\r\n",SPI_Read_Byte(READ_REG_CMD+RF_CH));
//	printf("RX_PW_P0:%x\r\n",SPI_Read_Byte(READ_REG_CMD+RX_PW_P0));
//	printf("CONFIG:%x\r\n",SPI_Read_Byte(READ_REG_CMD+CONFIG));

	CE_HIGH;	
}

/***************************************************************************
 * ��  �� : NRF24L01�Ĵ���д����
 * ��  �� : regaddr��Ҫд�ļĴ�����ַ��data��д�뵽�Ĵ���������
 * ����ֵ : ��ȡ��״ֵ̬
 **************************************************************************/
u8 NRF24L01_MB_Write_Reg(u8 regaddr,u8 dat)
{
	u8 status;	
	
  CS_MB_LOW;                             //ʹ��SPI����
  status =SPI1_ReadWriteByte(regaddr); //���ͼĴ�����ַ
  SPI1_ReadWriteByte(dat);             //д��Ĵ�����ֵ
  CS_MB_HIGH;                            //��ֹSPI����	   
  return(status);       		          //����״ֵ̬
}

/***************************************************************************
 * ��  �� : NRF24L01�Ĵ���������
 * ��  �� : regaddr:Ҫ��ȡ�ļĴ�����ַ
 * ����ֵ : ��ȡ�ļĴ�����ֵ
 **************************************************************************/
u8 NRF24L01_MB_Read_Reg(u8 regaddr)
{
	u8 reg_val;	  
  
 	CS_MB_LOW;                          //ʹ��SPI����		
  SPI1_ReadWriteByte(regaddr);      //���ͼĴ�����
  reg_val=SPI1_ReadWriteByte(0); //��ȡ�Ĵ�������
  CS_MB_HIGH;                         //��ֹSPI����		    
  return(reg_val);                 //���ض�ȡ��ֵ
}	

/***************************************************************************
 * ��  �� : ��ָ����ַ����ָ�����ȵ�����
 * ��  �� : pBuf:�������ݵĴ�ŵ�ַ��datlen�������������ֽ���
 * ����ֵ : ��ȡ�ļĴ�����ֵ
 **************************************************************************/
u8 NRF24L01_ReadBuf_MB(u8 regaddr,u8 *pBuf,u8 datlen)
{
	u8 status,u8_ctr;	       
  CS_MB_LOW;                               //ʹ��SPI����
  status=SPI1_ReadWriteByte(regaddr);    //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<datlen;u8_ctr++)pBuf[u8_ctr]=SPI1_ReadWriteByte(0);//��������
  CS_MB_HIGH;                              //�ر�SPI����
  return status;                        //���ض�����״ֵ̬
}

/***************************************************************************
 * ��  �� : ��ָ����ַд��ָ�����ȵ�����
 * ��  �� : pBuf:�������ݵĴ�ŵ�ַ��datlen�������������ֽ���
 * ����ֵ : ��ȡ��״̬�Ĵ���ֵ
 **************************************************************************/
u8 NRF24L01_Write_Buf(u8 regaddr, u8 *pBuf, u8 datalen)
{
	u8 status,u8_ctr;	    
 	CS_MB_LOW;                                              //ʹ��SPI����
  status = SPI1_ReadWriteByte(regaddr);                 //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  for(u8_ctr=0; u8_ctr<datalen; u8_ctr++)SPI1_ReadWriteByte(*pBuf++); //д������	 
  CS_MB_HIGH;                                             //�ر�SPI����
  return status;                                       //���ض�����״ֵ̬
}	

/***************************************************************************
 * ��  �� : ����NRF24L01����һ������
 * ��  �� : buf:�����������׵�ַ
 * ����ֵ : �������״��
 **************************************************************************/
u8 NRF24L01_TxPacket_MB(u8 *buf)
{
	u8 state;   
	
	CE_MB_LOW;
  NRF24L01_Write_Buf(WR_TX_PLOAD,buf,TX_PLOAD_WIDTH);  //д���ݵ�TX BUF 
 	CE_MB_HIGH;                                             //��������	   
	while(READ_NRF24L01_MB_IRQ!=0);                         //�ȴ��������
	state=NRF24L01_MB_Read_Reg(STATUS);                     //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_MB_Write_Reg(SPI_WRITE_REG+STATUS,state);      //���TX_DS��MAX_RT�жϱ�־
	if(state&MAX_TX)                                     //�ﵽ����ط�����
	{
		NRF24L01_MB_Write_Reg(FLUSH_TX,0xff);                 //���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(state&TX_OK)                                      //�������
	{
		return TX_OK;
	}
	return 0xff;                                         //����ԭ����ʧ��
}

/***************************************************************************
 * ��  �� : ����NRF24L01��ȡһ������
 * ��  �� : buf:�����������׵�ַ
 * ����ֵ : 0�����յ����ݣ�1��δ���յ�����
 **************************************************************************/
u8 NRF24L01_RxPacket_MB(u8 *buf)
{
	u8 state;		    
	
	state=NRF24L01_MB_Read_Reg(STATUS);                //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_MB_Write_Reg(SPI_WRITE_REG+STATUS,state); //���TX_DS��MAX_RT�жϱ�־
	if(state&RX_OK)                                 //���յ�����
	{
		NRF24L01_ReadBuf_MB(RD_RX_PLOAD,buf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_MB_Write_Reg(FLUSH_RX,0xff);          //���RX FIFO�Ĵ��� 
		return RX_OK; 
	}	   
	return 0;                                      //û�յ��κ�����
}

/***************************************************************************
 * ��  �� : ��ʼ��NRF24L01��RXģʽ��������ز�����CE��ߺ�,������RXģʽ
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************/
void Set_RxMode_MB(void)
{
	CE_MB_LOW;	  
  //дRX�ڵ��ַ
  NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS_MB,RX_ADDR_WIDTH);

  //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);    
  //ʹ��ͨ��0�Ľ��յ�ַ  	 
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01);
  //����RFͨ��Ƶ��		  
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_CH,55);	     
  //ѡ��ͨ��0����Ч���ݿ�� 	    
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
  //����TX�������,0db����,2Mbps,���������濪��   
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x26);
  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX����ģʽ 
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+CONFIG, 0x0f); 
  //CEΪ��,�������ģʽ 
  CE_MB_HIGH;                                
}			

/***************************************************************************
 * ��  �� : ��ʼ��NRF24L01��TXģʽ��������ز���
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************/
void Set_TxMode_MB(void)
{														 
	CE_MB_LOW;	    
  //дTX�ڵ��ַ 
  NRF24L01_Write_Buf(SPI_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS_MB,TX_ADDR_WIDTH);    
  //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
  NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS_MB,RX_ADDR_WIDTH); 

  //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);     
  //ʹ��ͨ��0�Ľ��յ�ַ  
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01); 
  //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+SETUP_RETR,0x21);
  //����RFͨ��Ϊ55
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_CH,55);       
  //����TX�������,0db����,2Mbps,���������濪��   
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x26);  
  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX����ģʽ,���������ж�
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+CONFIG,0x0e);                                  
}		  
/*********************************END FILE************************************/







