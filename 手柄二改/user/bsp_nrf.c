#include "bsp_nrf.h"
//#include"sys.h"
extern u8  TX_ADDRESS[TX_ADR_WIDTH];	//�˵�ַ����ʶ����ն��ĸ�RXͨ�����Խ��շ��ͳ�ȥ�����ݰ�
extern u8  RX_ADDRESS[RX_ADR_WIDTH];
void SPI_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//	NVIC_Configuration();
	/* ��������I��LED��ʹ�õ�GPIO�ܽ�ģʽ*/
	RCC_APB2PeriphClockCmd(NRF_GPIO_CLK, ENABLE); /*ʹ��LED��ʹ�õ�GPIOʱ��*/

	GPIO_InitStructure.GPIO_Pin = NRF_GPIO_CE_PIN;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(NRF_GPIO_CE_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = NRF_GPIO_CSN_PIN;
	GPIO_Init(NRF_GPIO_CSN_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = NRF_GPIO_SCK_PIN;
	GPIO_Init(NRF_GPIO_SCK_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = NRF_GPIO_MOSI_PIN;
	GPIO_Init(NRF_GPIO_MOSI_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = NRF_GPIO_IRQ_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(NRF_GPIO_IRQ_PORT, &GPIO_InitStructure);  /*GPIO�ڳ�ʼ��*/

	GPIO_InitStructure.GPIO_Pin = NRF_GPIO_MISO_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(NRF_GPIO_MISO_PORT, &GPIO_InitStructure);  /*GPIO�ڳ�ʼ��*/
}


static void delay_us(uchar num)
{
	uchar i,j; 
	for(i=0;i>num;i++)
 	for(j=100;j>0;j--);
}
static void delay_150us(void)
{
	uint i;
	for(i=0;i>600;i++);
}

uchar SPI_RW(uchar byte)
{
	uchar bit_ctr;
	for(bit_ctr=0;bit_ctr<8;bit_ctr++)  // ���8λ
	{
		if((uchar)(byte&0x80)==0x80)
		NRF_MOSI=1; 			// MSB TO MOSI
		else 
			NRF_MOSI=0; 
		byte=(byte<<1);					// shift next bit to MSB
		NRF_SCK=1;
		byte|=NRF_MISO;	        		// capture current MISO bit
		NRF_SCK=0;
	}
	return byte;
}
/*********************************************/
/* �������ܣ���24L01�ļĴ���дֵ��һ���ֽڣ� */
/* ��ڲ�����reg   Ҫд�ļĴ�����ַ          */
/*           value ���Ĵ���д��ֵ            */
/* ���ڲ�����status ״ֵ̬                   */
/*********************************************/
uchar NRF24L01_Write_Reg(uchar reg,uchar value)
{
	uchar status;

	NRF_CSN=0;                  //CSN=0;   
  	status = SPI_RW(reg);		//���ͼĴ�����ַ,����ȡ״ֵ̬
	SPI_RW(value);
	NRF_CSN=1;                  //CSN=1;

	return status;
}
/*************************************************/
/* �������ܣ���24L01�ļĴ���ֵ ��һ���ֽڣ�      */
/* ��ڲ�����reg  Ҫ���ļĴ�����ַ               */
/* ���ڲ�����value �����Ĵ�����ֵ                */
/*************************************************/
uchar NRF24L01_Read_Reg(uchar reg)
{
 	uchar value;

	NRF_CSN=0;              //CSN=0;   
  	SPI_RW(reg);			//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	value = SPI_RW(NOP);
	NRF_CSN=1;             	//CSN=1;

	return value;
}
/*********************************************/
/* �������ܣ���24L01�ļĴ���ֵ������ֽڣ�   */
/* ��ڲ�����reg   �Ĵ�����ַ                */
/*           *pBuf �����Ĵ���ֵ�Ĵ������    */
/*           len   �����ֽڳ���              */
/* ���ڲ�����status ״ֵ̬                   */
/*********************************************/
uchar NRF24L01_Read_Buf(uchar reg,uchar *pBuf,uchar len)
{
	uchar status,u8_ctr;
	NRF_CSN=0;                   	//CSN=0       
	status=SPI_RW(reg);				//���ͼĴ�����ַ,����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)
	pBuf[u8_ctr]=SPI_RW(0XFF);		//��������
	NRF_CSN=1;                 		//CSN=1
	return status;        			//���ض�����״ֵ̬
}
/**********************************************/
/* �������ܣ���24L01�ļĴ���дֵ������ֽڣ�  */
/* ��ڲ�����reg  Ҫд�ļĴ�����ַ            */
/*           *pBuf ֵ�Ĵ������               */
/*           len   �����ֽڳ���               */
/**********************************************/
uchar NRF24L01_Write_Buf(uchar reg, uchar *pBuf, uchar len)
{
	uchar status,u8_ctr;
	NRF_CSN=0;
	status = SPI_RW(reg);			//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)
	SPI_RW(*pBuf++); 				//д������
	NRF_CSN=1;
  return status;          		//���ض�����״ֵ̬
}							  					   

/*********************************************/
/* �������ܣ�24L01��������                   */
/* ��ڲ�����rxbuf ������������              */
/* ����ֵ�� 0   �ɹ��յ�����                 */
/*          1   û���յ�����                 */
/*********************************************/
uchar NRF24L01_RxPacket(uchar *rxbuf)
{
	uchar state;
	state=NRF24L01_Read_Reg(STATUS);  			//��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(nRF_WRITE_REG+STATUS,state); //���TX_DS��MAX_RT�жϱ�־
	if(state&RX_OK)								//���յ�����
	{
		NRF_CE = 0;
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);					//���RX FIFO�Ĵ���
		NRF_CE = 1;
		delay_150us(); 
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}
/**********************************************/
/* �������ܣ�����24L01Ϊ����ģʽ              */
/* ��ڲ�����txbuf  ������������              */
/* ����ֵ�� 0x10    �ﵽ����ط�����������ʧ��*/
/*          0x20    �ɹ��������              */
/*          0xff    ����ʧ��                  */
/**********************************************/
uchar NRF24L01_TxPacket(uchar *txbuf)
{
	uchar state;
   
	NRF_CE=0;												//CE���ͣ�ʹ��24L01����
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);	//д���ݵ�TX BUF  8���ֽ�
 	NRF_CE=1;												//CE�øߣ�ʹ�ܷ���	   
	while(NRF_IRQ==1);										//�ȴ��������
	state=NRF24L01_Read_Reg(STATUS);  						//��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(nRF_WRITE_REG+STATUS,state); 			//���TX_DS��MAX_RT�жϱ�־
	if(state&MAX_TX)										//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);					//���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(state&TX_OK)											//�������
	{
		return TX_OK;
	}
	return 0xff;											//����ʧ��
}

/********************************************/
/* �������ܣ����24L01�Ƿ����              */
/* ����ֵ��  0  ����                        */
/*           1  ������                      */
/********************************************/ 	  
uchar NRF24L01_Check(void)
{
	uchar check_in_buf[5]={0x11,0x22,0x33,0x44,0x55};
	uchar check_out_buf[5]={0x00};

	NRF_SCK=0;
	NRF_CSN=1;    
	NRF_CE=0;

	NRF24L01_Write_Buf(nRF_WRITE_REG+TX_ADDR, check_in_buf, 5);

	NRF24L01_Read_Buf(nRF_READ_REG+TX_ADDR, check_out_buf, 5);

	if((check_out_buf[0] == 0x11)&&\
	   (check_out_buf[1] == 0x22)&&\
	   (check_out_buf[2] == 0x33)&&\
	   (check_out_buf[3] == 0x44)&&\
	   (check_out_buf[4] == 0x55))return 0;
	else return 1;
}			


void NRF24L01_WT_Init(void)
{	
	NRF_CE=0;		  
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RX_PW_P0,TX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��
		NRF24L01_Write_Reg(FLUSH_RX,0xff);									//���RX FIFO�Ĵ���    
  	NRF24L01_Write_Buf(nRF_WRITE_REG+TX_ADDR,(uchar*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  	NRF24L01_Write_Buf(nRF_WRITE_REG+RX_ADDR_P0,(uchar*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
  	NRF24L01_Write_Reg(nRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(nRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
  	NRF24L01_Write_Reg(nRF_WRITE_REG+SETUP_RETR,0x21);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RF_CH,55);      //����RFͨ��Ϊ55
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RF_SETUP,0x26);  //����TX�������,0db����,2Mbps,���������濪��    
  	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0F);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX����ģʽ,���������ж�
	NRF_CE=1;									  //CE�øߣ�ʹ�ܷ���
}

uint8_t SEND_BUF(uchar *buf)
{
	uint8_t status;
	NRF_CE=0;
	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0e);//����ģʽ
	NRF_CE=1;
	delay_us(15);
	status = NRF24L01_TxPacket(buf);
	
	NRF_CE=0;
	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG, 0x0f);
	NRF_CE=1;	
	return status;
}
/*void ADC_ConvertedValue_Resolve(u8 *p,float ADC_ConvertedValueLocal)
{
	p[0] = ADC_ConvertedValueLocal/100+'0';
	p[2] = ((u8)(ADC_ConvertedValueLocal/10))%10+'0';
	p[3] = (u8)(((u16)ADC_ConvertedValueLocal)%10)+'0';
}*/



