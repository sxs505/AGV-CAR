#include "can.h"
#include "system.h"
/**************************************************************************
*  �������ܣ�CAN1��ʼ��
*
*  ��ڲ�����tsjw������ͬ����Ծʱ�䵥Ԫ.��Χ:1~3;
*			tbs2��ʱ���2��ʱ�䵥Ԫ.��Χ:1~8;
*			tbs1��ʱ���1��ʱ�䵥Ԫ.��Χ:1~16;
*			brp �������ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ��1,Ҳ����1~1024) tq=(brp)*tpclk1
*			mode��0,��ͨģʽ;1,�ػ�ģʽ;
*
*  �� �� ֵ��0����ʼ��OK����������ʼ��ʧ��;
*
*  ����˵������ڲ���(����mode)������Ϊ0��
*			������=Fpclk1/((tbs1+tbs2+1)*brp)��Fpclk1Ϊ36M
            =36M/((3+2+1)*6)
						=1M
						CAN1_Mode_Init(tsjw:1, tbs2:2, tbs1:3, brp:6, mode:0);
**************************************************************************/
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	u16 i=0;
 	if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
	tsjw-=1;//�ȼ�ȥ1.����������
	tbs2-=1;
	tbs1-=1;
	brp-=1;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);// ��Ҫʹ��AFIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); //ʹ��GPIO�˿�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
	GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);		//��ʼ��IO
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��IO

	CAN1->MCR=0x0000;	//�˳�˯��ģʽ(ͬʱ��������λΪ0)
	CAN1->MCR|=1<<0;		//����CAN1�����ʼ��ģʽ
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100)return 2;//�����ʼ��ģʽʧ��
	}
	CAN1->MCR|=0<<7;		//��ʱ�䴥��ͨ��ģʽ
	CAN1->MCR|=0<<6;		//�����Զ����߹���
	CAN1->MCR|=0<<5;		//˯��ģʽͨ����������(���CAN1->MCR��SLEEPλ)
	CAN1->MCR|=1<<4;		//��ֹ�����Զ�����
	CAN1->MCR|=0<<3;		//���Ĳ�����,�µĸ��Ǿɵ�
	CAN1->MCR|=0<<2;		//���ȼ��ɱ��ı�ʶ������
	CAN1->BTR=0x00000000;//���ԭ��������.
	CAN1->BTR|=mode<<30;	//ģʽ���� 0,��ͨģʽ;1,�ػ�ģʽ;
	CAN1->BTR|=tsjw<<24; //����ͬ����Ծ����(Tsjw)Ϊtsjw+1��ʱ�䵥λ
	CAN1->BTR|=tbs2<<20; //Tbs2=tbs2+1��ʱ�䵥λ
	CAN1->BTR|=tbs1<<16;	//Tbs1=tbs1+1��ʱ�䵥λ
	CAN1->BTR|=brp<<0;  	//��Ƶϵ��(Fdiv)Ϊbrp+1
						//������:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)
	CAN1->MCR&=~(1<<0);	//����CAN1�˳���ʼ��ģʽ
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0XFFF0)return 3;//�˳���ʼ��ģʽʧ��
	}
	//��������ʼ��
	CAN1->FMR|=1<<0;			//�������鹤���ڳ�ʼ��ģʽ
	CAN1->FA1R&=~(1<<0);		//������0������
	CAN1->FS1R|=1<<0; 		//������λ��Ϊ32λ.
	CAN1->FM1R|=0<<0;		//������0�����ڱ�ʶ������λģʽ
	CAN1->FFA1R|=0<<0;		//������0������FIFO0
	CAN1->sFilterRegister[0].FR1=0X00000000;//32λID
	CAN1->sFilterRegister[0].FR2=0X00000000;//32λMASK
	CAN1->FA1R|=1<<0;		//���������0
	CAN1->FMR&=0<<0;			//���������������ģʽ

#if CAN1_RX0_INT_ENABLE
 	//ʹ���жϽ���
	
	CAN1->IER|=1<<1;			//FIFO0��Ϣ�Һ��ж�����.
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   //USART ��ʼ������

  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
	
#endif
	return 0;
}   

/**************************************************************************
*  �������ܣ�CAN��������
*
*  ��ڲ�����id:��׼ID(11λ)/��չID(11λ+18λ)	    
*			ide:0,��׼֡;1,��չ֡
*			rtr:0,����֡;1,Զ��֡
*			len:Ҫ���͵����ݳ���(�̶�Ϊ8���ֽ�,��ʱ�䴥��ģʽ��,��Ч����Ϊ6���ֽ�)
*			*dat:����ָ��.
*
*  �� �� ֵ��0~3,������.0XFF,����Ч����
**************************************************************************/
u8 CAN1_Tx_Msg(u32 id,u8 ide,u8 rtr,u8 len,u8 *dat)
{	   
	u8 mbox;	  
	if(CAN1->TSR&(1<<26))mbox=0;		//����0Ϊ��
	else if(CAN1->TSR&(1<<27))mbox=1;	//����1Ϊ��
	else if(CAN1->TSR&(1<<28))mbox=2;	//����2Ϊ��
	else return 0XFF;					//�޿�����,�޷����� 
	CAN1->sTxMailBox[mbox].TIR=0;		//���֮ǰ������
	if(ide==0)	//��׼֡
	{
		id&=0x7ff;//ȡ��11λstdid
		id<<=21;		  
	}else		//��չ֡
	{
		id&=0X1FFFFFFF;//ȡ��32λextid
		id<<=3;									   
	}
	CAN1->sTxMailBox[mbox].TIR|=id;		 
	CAN1->sTxMailBox[mbox].TIR|=ide<<2;	  
	CAN1->sTxMailBox[mbox].TIR|=rtr<<1;
	len&=0X0F;//�õ�����λ
	CAN1->sTxMailBox[mbox].TDTR&=~(0X0000000F);
	CAN1->sTxMailBox[mbox].TDTR|=len;		   //����DLC.
	//���������ݴ�������.
	CAN1->sTxMailBox[mbox].TDHR=(((u32)dat[7]<<24)|
								((u32)dat[6]<<16)|
 								((u32)dat[5]<<8)|
								((u32)dat[4]));
	CAN1->sTxMailBox[mbox].TDLR=(((u32)dat[3]<<24)|
								((u32)dat[2]<<16)|
 								((u32)dat[1]<<8)|
								((u32)dat[0]));
	CAN1->sTxMailBox[mbox].TIR|=1<<0; //��������������
	return mbox;
}

/**************************************************************************
*  �������ܣ���÷���״̬
*
*  ��ڲ�����mbox��������
*
*  �� �� ֵ��0,����;0X05,����ʧ��;0X07,���ͳɹ�.
**************************************************************************/
u8 CAN1_Tx_Staus(u8 mbox)
{	
	u8 sta=0;					    
	switch (mbox)
	{
		case 0: 
			sta |= CAN1->TSR&(1<<0);			//RQCP0
			sta |= CAN1->TSR&(1<<1);			//TXOK0
			sta |=((CAN1->TSR&(1<<26))>>24);	//TME0
			break;
		case 1: 
			sta |= CAN1->TSR&(1<<8)>>8;		//RQCP1
			sta |= CAN1->TSR&(1<<9)>>8;		//TXOK1
			sta |=((CAN1->TSR&(1<<27))>>25);	//TME1	   
			break;
		case 2: 
			sta |= CAN1->TSR&(1<<16)>>16;	//RQCP2
			sta |= CAN1->TSR&(1<<17)>>16;	//TXOK2
			sta |=((CAN1->TSR&(1<<28))>>26);	//TME2
			break;
		default:
			sta=0X05;//����Ų���,�϶�ʧ��.
		break;
	}
	return sta;
} 

/**************************************************************************
*  �������ܣ��õ���FIFO0/FIFO1�н��յ��ı��ĸ���
*
*  ��ڲ�����fifox��FIFO��ţ�0��1��
*
*  �� �� ֵ��FIFO0/FIFO1�еı��ĸ���.
**************************************************************************/
u8 CAN1_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN1->RF0R&0x03; 
	else if(fifox==1)return CAN1->RF1R&0x03; 
	else return 0;
}

/**************************************************************************
*  �������ܣ���������
*
*  ��ڲ�����fifox�������
*			id:��׼ID(11λ)/��չID(11λ+18λ)
*			ide:0,��׼֡;1,��չ֡
*			rtr:0,����֡;1,Զ��֡
*			len:���յ������ݳ���(�̶�Ϊ8���ֽ�,��ʱ�䴥��ģʽ��,��Ч����Ϊ6���ֽ�)
*			dat:���ݻ�����
*
*  �� �� ֵ����
**************************************************************************/
void CAN1_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat)
{	   
	*ide=CAN1->sFIFOMailBox[fifox].RIR&0x04;//�õ���ʶ��ѡ��λ��ֵ  
 	if(*ide==0)//��׼��ʶ��
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>21;
	}else	   //��չ��ʶ��
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>3;
	}
	*rtr=CAN1->sFIFOMailBox[fifox].RIR&0x02;	//�õ�Զ�̷�������ֵ.
	*len=CAN1->sFIFOMailBox[fifox].RDTR&0x0F;//�õ�DLC
 	//*fmi=(CAN1->sFIFOMailBox[FIFONumber].RDTR>>8)&0xFF;//�õ�FMI
	//��������
	dat[0]=CAN1->sFIFOMailBox[fifox].RDLR&0XFF;
	dat[1]=(CAN1->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	dat[2]=(CAN1->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	dat[3]=(CAN1->sFIFOMailBox[fifox].RDLR>>24)&0XFF;    
	dat[4]=CAN1->sFIFOMailBox[fifox].RDHR&0XFF;
	dat[5]=(CAN1->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	dat[6]=(CAN1->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	dat[7]=(CAN1->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN1->RF0R|=0X20;//�ͷ�FIFO0����
	else if(fifox==1)CAN1->RF1R|=0X20;//�ͷ�FIFO1����	 
}

/**************************************************************************
*  �������ܣ�CAN�����жϷ���������������
*
*  ��ڲ�������
*
*  �� �� ֵ����
**************************************************************************/	
#if CAN1_RX0_INT_ENABLE	//ʹ��RX0�ж�	    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  u8 i;
	u32 id;
	u8 ide,rtr,len;     
	u8 ON_rxbuf[8]={10,12,15,19,24,30,37,0};

	u8 temp_rxbuf[8];

 	CAN1_Rx_Msg(0,&id,&ide,&rtr,&len,temp_rxbuf);
	if(CAN_ON_Flag==0)
	{
		for(i=0;i<=7;i++) //���ڽ���CAN���Ƶ���ʼ���
		{
			if(i==7)
			{
				CAN_ON_Flag=temp_rxbuf[i];
				if(CAN_ON_Flag==1) 
				 {
					 PS2_ON_Flag=0,APP_ON_Flag=0,Remote_ON_Flag=0,Usart_ON_Flag=0;
					 for(i=0;i<7;i++)
					 {
						 temp_rxbuf[i]=0;
					 }
				 }
		  }
			 if(temp_rxbuf[i]!=ON_rxbuf[i]) break;
		}
  }
  if(CAN_ON_Flag==1)
	{
		Move_X=(short)((temp_rxbuf[0]<<8)+(temp_rxbuf[1])); //��X���ٶ� �ָ�8λ�͵�8λ ��λmm/s
		Move_Y=(short)((temp_rxbuf[2]<<8)+(temp_rxbuf[3])); //��Y���ٶ� �ָ�8λ�͵�8λ ��λmm/s
		Move_Z=(short)((temp_rxbuf[4]<<8)+(temp_rxbuf[5])); //��Z���ٶ� �ָ�8λ�͵�8λ ��λmm/s
		
		Move_X=Move_X/1000; //��λm/s
		Move_Y=Move_Y/1000; //��λm/s
		Move_Z=Move_Z/1000; //��λm/s
	}
}
#endif


/**************************************************************************
*  �������ܣ�CAN1����һ������(�̶���ʽ:IDΪ0X601,��׼֡,����֡)
*
*  ��ڲ�����msg:����ָ��
*			len:���ݳ���(���Ϊ8)
*
*  �� �� ֵ��0,�ɹ�������,ʧ��;
**************************************************************************/	
u8 CAN1_Send_Msg(u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;	  	 						       
  mbox=CAN1_Tx_Msg(0X601,0,0,len,msg);   
	while((CAN1_Tx_Staus(mbox)!=0X07)&&(i<0XFFF))i++;//�ȴ����ͽ���
	if(i>=0XFFF)return 1;							//����ʧ��?
	return 0;										//���ͳɹ�;
}

/**************************************************************************
*  �������ܣ�CAN1�ڽ������ݲ�ѯ
*
*  ��ڲ�����buf:���ݻ�����;
*
*  �� �� ֵ��0,�����ݱ��յ�������,���յ����ݳ���;
**************************************************************************/	
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
	u32 id;
	u8 ide,rtr,len; 
	if(CAN1_Msg_Pend(0)==0)return 0;			//û�н��յ�����,ֱ���˳� 	 
  	CAN1_Rx_Msg(0,&id,&ide,&rtr,&len,buf); 	//��ȡ����
    if(id!=0x12||ide!=0||rtr!=0)len=0;		//���մ���	   
	return len;	
}

/**************************************************************************
*  �������ܣ�CAN1����һ�����ݲ���
*
*  ��ڲ�����msg:����ָ��
*			len:���ݳ���(���Ϊ8)
*
*  �� �� ֵ��0,�ɹ���1,ʧ��;
**************************************************************************/	
u8 CAN1_Send_MsgTEST(u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;	  	 						       
    mbox=CAN1_Tx_Msg(0X701,0,0,len,msg);   
	while((CAN1_Tx_Staus(mbox)!=0X07)&&(i<0XFFF))i++;//�ȴ����ͽ���
	if(i>=0XFFF)return 1;							//����ʧ��?
	return 0;										//���ͳɹ�;
}

/**************************************************************************
*  �������ܣ���������id����һ�����������
*
*  ��ڲ�����id��ID��
*			msg������������ָ��
*
*  �� �� ֵ��0,�ɹ���1,ʧ��;
**************************************************************************/	
u8 CAN1_Send_Num(u32 id,u8* msg)
{
	u8 mbox;
	u16 i=0;	  	 						       
  mbox=CAN1_Tx_Msg(id,0,0,8,msg);   
	while((CAN1_Tx_Staus(mbox)!=0X07)&&(i<0XFFF))i++;//�ȴ����ͽ���
	if(i>=0XFFF)return 1;							//����ʧ��?
	return 0;
}