	if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			//printf("������ְ�");
			//printf("%d",len);
			x=y=0;
			len = len - 1;
			for(t=0;t<len;t++)
			{
				if(USART_RX_BUF[t] == ' ')      //�ҵ��綨���Ļ�
				{
					unsigned char i = 0;
					for(i = 0;i<t; i++)
					{
						uint16_t m = 1;
						unsigned char k = 0;
						for(k = 0; k<(t-i-1);k++)
							m = m*10;
						x = x + (USART_RX_BUF[i] - '0')*m;
					}
					for(i = t+1;i<len; i++)
					{
						uint16_t m = 1;
						unsigned char k = 0;
						for(k = 0; k<(len-1-i);k++)
							m = m*10;
						y = y + (USART_RX_BUF[i] - '0')*m;
					}
					last_last_status = last_status;       //���ϴκ��̵�״̬��
					last_status = status;                 //�ϴκ��̵Ƶ�״̬��
					status = USART_RX_BUF[len] - '0';
					LCD_ShowNum(85,160,x,3,16);           //��ʾx����
					LCD_ShowNum(85,180,y,3,16);           //��ʾy����
					LCD_ShowNum(85,200,status,3,16);           //��ʾ��ǰ״̬
				}
			}
			USART_RX_STA=0;
		}