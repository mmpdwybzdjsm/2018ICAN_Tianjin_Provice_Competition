#include "bsp_nrf.h"
//#include"sys.h"
extern u8  TX_ADDRESS[TX_ADR_WIDTH];	//此地址用来识别接收端哪个RX通道可以接收发送出去的数据包
extern u8  RX_ADDRESS[RX_ADR_WIDTH];
void SPI_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//	NVIC_Configuration();
	/* 配置神舟I号LED灯使用的GPIO管脚模式*/
	RCC_APB2PeriphClockCmd(NRF_GPIO_CLK, ENABLE); /*使能LED灯使用的GPIO时钟*/

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
	GPIO_Init(NRF_GPIO_IRQ_PORT, &GPIO_InitStructure);  /*GPIO口初始化*/

	GPIO_InitStructure.GPIO_Pin = NRF_GPIO_MISO_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(NRF_GPIO_MISO_PORT, &GPIO_InitStructure);  /*GPIO口初始化*/
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
	for(bit_ctr=0;bit_ctr<8;bit_ctr++)  // 输出8位
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
/* 函数功能：给24L01的寄存器写值（一个字节） */
/* 入口参数：reg   要写的寄存器地址          */
/*           value 给寄存器写的值            */
/* 出口参数：status 状态值                   */
/*********************************************/
uchar NRF24L01_Write_Reg(uchar reg,uchar value)
{
	uchar status;

	NRF_CSN=0;                  //CSN=0;   
  	status = SPI_RW(reg);		//发送寄存器地址,并读取状态值
	SPI_RW(value);
	NRF_CSN=1;                  //CSN=1;

	return status;
}
/*************************************************/
/* 函数功能：读24L01的寄存器值 （一个字节）      */
/* 入口参数：reg  要读的寄存器地址               */
/* 出口参数：value 读出寄存器的值                */
/*************************************************/
uchar NRF24L01_Read_Reg(uchar reg)
{
 	uchar value;

	NRF_CSN=0;              //CSN=0;   
  	SPI_RW(reg);			//发送寄存器值(位置),并读取状态值
	value = SPI_RW(NOP);
	NRF_CSN=1;             	//CSN=1;

	return value;
}
/*********************************************/
/* 函数功能：读24L01的寄存器值（多个字节）   */
/* 入口参数：reg   寄存器地址                */
/*           *pBuf 读出寄存器值的存放数组    */
/*           len   数组字节长度              */
/* 出口参数：status 状态值                   */
/*********************************************/
uchar NRF24L01_Read_Buf(uchar reg,uchar *pBuf,uchar len)
{
	uchar status,u8_ctr;
	NRF_CSN=0;                   	//CSN=0       
	status=SPI_RW(reg);				//发送寄存器地址,并读取状态值   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)
	pBuf[u8_ctr]=SPI_RW(0XFF);		//读出数据
	NRF_CSN=1;                 		//CSN=1
	return status;        			//返回读到的状态值
}
/**********************************************/
/* 函数功能：给24L01的寄存器写值（多个字节）  */
/* 入口参数：reg  要写的寄存器地址            */
/*           *pBuf 值的存放数组               */
/*           len   数组字节长度               */
/**********************************************/
uchar NRF24L01_Write_Buf(uchar reg, uchar *pBuf, uchar len)
{
	uchar status,u8_ctr;
	NRF_CSN=0;
	status = SPI_RW(reg);			//发送寄存器值(位置),并读取状态值
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)
	SPI_RW(*pBuf++); 				//写入数据
	NRF_CSN=1;
  return status;          		//返回读到的状态值
}							  					   

/*********************************************/
/* 函数功能：24L01接收数据                   */
/* 入口参数：rxbuf 接收数据数组              */
/* 返回值： 0   成功收到数据                 */
/*          1   没有收到数据                 */
/*********************************************/
uchar NRF24L01_RxPacket(uchar *rxbuf)
{
	uchar state;
	state=NRF24L01_Read_Reg(STATUS);  			//读取状态寄存器的值    	 
	NRF24L01_Write_Reg(nRF_WRITE_REG+STATUS,state); //清除TX_DS或MAX_RT中断标志
	if(state&RX_OK)								//接收到数据
	{
		NRF_CE = 0;
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);					//清除RX FIFO寄存器
		NRF_CE = 1;
		delay_150us(); 
		return 0; 
	}	   
	return 1;//没收到任何数据
}
/**********************************************/
/* 函数功能：设置24L01为发送模式              */
/* 入口参数：txbuf  发送数据数组              */
/* 返回值； 0x10    达到最大重发次数，发送失败*/
/*          0x20    成功发送完成              */
/*          0xff    发送失败                  */
/**********************************************/
uchar NRF24L01_TxPacket(uchar *txbuf)
{
	uchar state;
   
	NRF_CE=0;												//CE拉低，使能24L01配置
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);	//写数据到TX BUF  8个字节
 	NRF_CE=1;												//CE置高，使能发送	   
	while(NRF_IRQ==1);										//等待发送完成
	state=NRF24L01_Read_Reg(STATUS);  						//读取状态寄存器的值	   
	NRF24L01_Write_Reg(nRF_WRITE_REG+STATUS,state); 			//清除TX_DS或MAX_RT中断标志
	if(state&MAX_TX)										//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);					//清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(state&TX_OK)											//发送完成
	{
		return TX_OK;
	}
	return 0xff;											//发送失败
}

/********************************************/
/* 函数功能：检测24L01是否存在              */
/* 返回值；  0  存在                        */
/*           1  不存在                      */
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
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RX_PW_P0,TX_PLOAD_WIDTH);//选择通道0的有效数据宽度
		NRF24L01_Write_Reg(FLUSH_RX,0xff);									//清除RX FIFO寄存器    
  	NRF24L01_Write_Buf(nRF_WRITE_REG+TX_ADDR,(uchar*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  	NRF24L01_Write_Buf(nRF_WRITE_REG+RX_ADDR_P0,(uchar*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  
  	NRF24L01_Write_Reg(nRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  	NRF24L01_Write_Reg(nRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  	NRF24L01_Write_Reg(nRF_WRITE_REG+SETUP_RETR,0x21);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RF_CH,55);      //设置RF通道为55
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RF_SETUP,0x26);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启    
  	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0F);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX发送模式,开启所有中断
	NRF_CE=1;									  //CE置高，使能发送
}

uint8_t SEND_BUF(uchar *buf)
{
	uint8_t status;
	NRF_CE=0;
	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0e);//发送模式
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



