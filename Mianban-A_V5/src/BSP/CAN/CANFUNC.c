/****************************************Copyright (c)**************************************************
**                               ������������Ƭ����չ���޹�˾
**                                     ��    ��    ��
**                                        ��Ʒһ�� 
**                                 http://www.zlgmcu.com
**-----------------------------------------------------------�ļ���Ϣ--------------------------------------------------------------------------------
**	��   ��  	��:	CANFUNC.C
** 	��  		��:	v1.0
** 	��		��:	2004��2��23��
**	��		��:	CANģ�鹦�ܲ�������˵���ļ�
********************************************************************************************************/
#define	_CANFUNC_GLOBAL_
#include "IncludeCan.h"
#include 	<Common.h>
/*
***********************************************************************************************************
**����ԭ��		:  	void	HwRstCAN (eCANNUM CanNum)
**����˵��		:  	CanNum		-->>	CAN��������ֵ���ܴ���CAN_MAX_NUM �涨��ֵ
**����ֵ		:	��	
**˵	��		:	����������Ӳ����λCAN����������Ҫ��CANģ����磬����ʡ�紦��
************************************************************************************************************/
void		HwRstCAN(eCANNUM CanNum)
{
	PCONP &= ~((u32)0x01 << (13+CanNum));
}
/*
***********************************************************************************************************
**����ԭ��		:  	void	HwEnCAN (eCANNUM CanNum)
**����˵��		:  	CanNum		-->>	CAN��������ֵ���ܴ���CAN_MAX_NUM �涨��ֵ
**����ֵ		:	��	
**˵	��		:	����������Ӳ��ʹ��CAN������
************************************************************************************************************/
void		HwEnCAN(eCANNUM CanNum)
{
	CANAFMR.Bits.AccBP_BIT =1;
	PCONP |= ((u32)0x01 << (13+CanNum));
	switch(CanNum)
	{
		case	CAN1:
			PINSEL1 &=  ~((u32)0x03 << 18);
			PINSEL1 |=  ((u32)0x01 << 18);
			break;
		case	CAN2:
			PINSEL1 &=  ~((u32)0x0F << 14);
			PINSEL1 |=  ((u32)0x05 << 14);
			break;
		case	CAN3:
			PINSEL1 &=  ~((u32)0x0F << 10);
			PINSEL1 |=  ((u32)0x06 << 10);
			break;
		case	CAN4:
			PINSEL1 |=  ((u32)0x0F << 24);
			break;
		default:
			break;	
	}
}
/*
***********************************************************************************************************
**����ԭ��	:  	u32	SoftRstCAN (eCANNUM CanNum)
**����˵��	:  	CanNum		-->>	CAN��������ֵ���ܴ���CAN_MAX_NUM �涨��ֵ
**����ֵ		:	��0����ʾ��λ�ɹ��� ����0����λ���ɹ���	
**˵	��		:	���������������λCAN������
************************************************************************************************************/
u32		SoftRstCAN(eCANNUM CanNum)
{
	CANMOD(CanNum).Bits.RM_BIT =1;
	return(!CANMOD(CanNum).Bits.RM_BIT );
}
/*
***********************************************************************************************************
**����ԭ��		:  	u32	SoftEnCAN (eCANNUM CanNum)
**����˵��		:  	CanNum		-->>	CAN��������ֵ���ܴ���CAN_MAX_NUM �涨��ֵ
**����ֵ		:	��0����ʾ�ɹ�ʹCAN���������빤��ģʽ�� ����0�����ɹ���	
**˵	��		:	�������������ʹ��CAN������
************************************************************************************************************/
u32		SoftEnCAN(eCANNUM CanNum)
{
	CANMOD(CanNum).Bits.RM_BIT =0;
	return(CANMOD(CanNum).Bits.RM_BIT );
}
/*
***********************************************************************************************************
**����ԭ��		:  	void	CanSendCmd(eCANNUM CanNum,u32 Cmd,u32 TxBuf)
**����˵��		:  	CanNum		-->>	CAN��������ֵ���ܴ���CAN_MAX_NUM �涨��ֵ
					Cmd		--> ����������
					TxBuf	-->	ѡ���ͻ�����
**����ֵ		:		
**˵	��		:	���������ڷ��������
************************************************************************************************************/
void	CanSendCmd(eCANNUM CanNum,u32 Cmd,u32 TxBuf)
{
		uCANCMR Temp;
	Temp.Word =0;
	Temp.Bits.STB1_BIT = TxBuf & 0x00000001;
	Temp.Bits.STB2_BIT = (TxBuf & 0x00000002)>>1;
	Temp.Bits.STB3_BIT = (TxBuf & 0x00000004)>>2;
	switch(Cmd)
	{
		case	1:
			if (CANMOD(CanNum).Bits.STM_BIT != 0)
			{
				while (!CANMOD(CanNum).Bits.RM_BIT)
					CANMOD(CanNum).Bits.RM_BIT = 1;
				/*****************************/	
				while (CANMOD(CanNum).Bits.STM_BIT)
					CANMOD(CanNum).Bits.STM_BIT = 0;
				/******************************/
				while (CANMOD(CanNum).Bits.RM_BIT)
					CANMOD(CanNum).Bits.RM_BIT = 0;
			}
			Temp.Bits.AT_BIT =1;						//���η���
			Temp.Bits.TR_BIT =1;
			break;
		case	2:
		case	3:									//�Է�����
			if (CANMOD(CanNum).Bits.STM_BIT != 1)
			{
				while (!CANMOD(CanNum).Bits.RM_BIT)
					CANMOD(CanNum).Bits.RM_BIT = 1;
				/******************************/	
				while (!CANMOD(CanNum).Bits.STM_BIT)
					CANMOD(CanNum).Bits.STM_BIT = 1;
				/*****************************/
				while (CANMOD(CanNum).Bits.RM_BIT)
					CANMOD(CanNum).Bits.RM_BIT = 0;
			}
			Temp.Bits.SRR_BIT =1;
			if(Cmd == 3)							//�����Է�����
			{
				Temp.Bits.AT_BIT =1;
			}
			break;
		case 	0:
		default:									//��������
			if (CANMOD(CanNum).Bits.STM_BIT != 0)
			{
				while (!CANMOD(CanNum).Bits.RM_BIT)
					CANMOD(CanNum).Bits.RM_BIT = 1;
				/*****************************/	
				while (CANMOD(CanNum).Bits.STM_BIT)
					CANMOD(CanNum).Bits.STM_BIT = 0;
				/******************************/
				while (CANMOD(CanNum).Bits.RM_BIT)
					CANMOD(CanNum).Bits.RM_BIT = 0;
			}
			Temp.Bits.TR_BIT =1;
			break;
	}
	CANCMR(CanNum) = Temp;
}
/*
***********************************************************************************************************
**����ԭ��	:  	u32	WriteCanTxBuf(eCANNUM CanNum,u32 TxBufNum,
										u32 TPM,P_stcTxBUF Buf) ;
**����˵��	:  	CanNum		-->>	CAN��������ֵ���ܴ���CAN_MAX_NUM �涨��ֵ
				TxBufNum	-->	ѡ���ͻ�����
				TPM			-->	�������ȼ�ģʽ
				Buf			-->	���͵����ߵ�����
**����ֵ		:	=0;�ɹ�������д�뷢�ͻ�����
					!=0;д���ͻ���������ʧ��	
**˵	��		:	������������CAN���ͻ�����TxBUFд�뷢������
***********************************************************************************************************
*/
u32	WriteCanTxBuf(eCANNUM CanNum,u32 TxBufNum,u32 TPM,P_stcTxBUF Buf)
{
	u32	status=0;
	switch(TxBufNum)
	{

		case	SEND_TX_BUF1:
			if((0== CANSR(CanNum).Bits.TS1_BIT)&&(0 != CANSR(CanNum).Bits.TBS1_BIT))
			{
				CANTFI1(CanNum)=Buf->TxFrameInfo;
				CANTID1(CanNum)=Buf->TxCANID;
				CANTDA1(CanNum)=Buf->CANTDA;
				CANTDB1(CanNum)=Buf->CANTDB;
			}
			else
			{
				status = 0x01;
			}
			break;
		case	SEND_TX_BUF2:
			if((0== CANSR(CanNum).Bits.TS2_BIT)&&(0 !=CANSR(CanNum).Bits.TBS2_BIT))
			{
				CANTFI2(CanNum)=Buf->TxFrameInfo;
				CANTID2(CanNum)=Buf->TxCANID;
				CANTDA2(CanNum)=Buf->CANTDA;
				CANTDB2(CanNum)=Buf->CANTDB;
			}
			else
			{
				status = 0x01;
			}
			break;
		case	SEND_TX_BUF3:
			if((0== CANSR(CanNum).Bits.TS3_BIT)&&(0 != CANSR(CanNum).Bits.TBS3_BIT))
			{
				CANTFI3(CanNum)=Buf->TxFrameInfo;
				CANTID3(CanNum)=Buf->TxCANID;
				CANTDA3(CanNum)=Buf->CANTDA;
				CANTDB3(CanNum)=Buf->CANTDB;
			}
			else
			{
				status = 0x01;
			}
			break;
		default:
			status = 0x01;
			break;
	}
	if(TPM == 1)
	{
		CANMOD(CanNum).Bits.TPM_BIT =1;
	}
	return(status);
}
/*
***********************************************************************************************************
**����ԭ��	:  	void	ReadCanRxBuf(eCANNUM CanNum,P_stcRxBUF Buf)
**����˵��	:  	CanNum		-->>	CAN��������ֵ���ܴ���CAN_MAX_NUM �涨��ֵ
				Buf			-->>	����CAN��������
**����ֵ		:	
**˵	��		:	���������ڴ�CAN������RxBuf�ж�ȡ����
***********************************************************************************************************
*/
void	ReadCanRxBuf(eCANNUM CanNum,P_stcRxBUF Buf)
{
	Buf->CANRcvFS 	=CANRFS(CanNum);
	Buf->RxCANID 	=CANRID(CanNum);
	Buf->CANRDA	=CANRDA(CanNum);
	Buf->CANRDB	=CANRDB(CanNum);
}
/*
***********************************************************************************************************
**����ԭ��		:  	u32 	CanEntrySM(CanNum)
**����˵��		:  	CanNum	-->>	CAN��������ֵ���ܴ���CAN_MAX_NUM �涨��ֵ
**����ֵ		:	=0;�ɹ�����˯��״̬;
					!=0;���ܽ���˯��״̬;	
**˵	��		:	����������ʹCAN��������������ģʽ
***********************************************************************************************************
*/
u32	CanEntrySM(eCANNUM	CanNum)
{
	CANMOD(CanNum).Bits.SM_BIT = 1;	
	return(!CANMOD(CanNum).Bits.SM_BIT);
}
/*
***********************************************************************************************************
**����ԭ��		:  	u32 CanQuitSM(eCANNUM	CanNum)
**����˵��		:  	CanNum		-->>	CAN��������ֵ���ܴ���CAN_MAX_NUM �涨��ֵ
**����ֵ		:	=0;�ɹ��˳�˯��״̬;
					!=0;�����˳�˯��״̬;	
**˵	��		:	����������ʹCAN�������˳�����ģʽ
***********************************************************************************************************
*/
u32 CanQuitSM(eCANNUM	CanNum)	
{
	uCANMod		i;
	i=CANMOD(CanNum);
	CANMOD(CanNum).Bits.RM_BIT=1; 
	CANMOD(CanNum).Bits.SM_BIT = 0;	
	CANMOD(CanNum).Bits.RM_BIT=i.Bits.RM_BIT; 
	return(CANMOD(CanNum).Bits.SM_BIT);
}
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
