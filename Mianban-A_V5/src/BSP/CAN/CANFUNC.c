/****************************************Copyright (c)**************************************************
**                               广州周立功单片机发展有限公司
**                                     研    究    所
**                                        产品一部 
**                                 http://www.zlgmcu.com
**-----------------------------------------------------------文件信息--------------------------------------------------------------------------------
**	文   件  	名:	CANFUNC.C
** 	版  		本:	v1.0
** 	日		期:	2004年2月23日
**	描		述:	CAN模块功能操作函数说明文件
********************************************************************************************************/
#define	_CANFUNC_GLOBAL_
#include "IncludeCan.h"
#include 	<Common.h>
/*
***********************************************************************************************************
**函数原型		:  	void	HwRstCAN (eCANNUM CanNum)
**参数说明		:  	CanNum		-->>	CAN控制器，值不能大于CAN_MAX_NUM 规定的值
**返回值		:	无	
**说	明		:	本函数用于硬件复位CAN控制器，主要是CAN模块掉电，用于省电处理
************************************************************************************************************/
void		HwRstCAN(eCANNUM CanNum)
{
	PCONP &= ~((u32)0x01 << (13+CanNum));
}
/*
***********************************************************************************************************
**函数原型		:  	void	HwEnCAN (eCANNUM CanNum)
**参数说明		:  	CanNum		-->>	CAN控制器，值不能大于CAN_MAX_NUM 规定的值
**返回值		:	无	
**说	明		:	本函数用于硬件使能CAN控制器
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
**函数原型	:  	u32	SoftRstCAN (eCANNUM CanNum)
**参数说明	:  	CanNum		-->>	CAN控制器，值不能大于CAN_MAX_NUM 规定的值
**返回值		:	＝0，表示复位成功； ！＝0，复位不成功；	
**说	明		:	本函数用于软件复位CAN控制器
************************************************************************************************************/
u32		SoftRstCAN(eCANNUM CanNum)
{
	CANMOD(CanNum).Bits.RM_BIT =1;
	return(!CANMOD(CanNum).Bits.RM_BIT );
}
/*
***********************************************************************************************************
**函数原型		:  	u32	SoftEnCAN (eCANNUM CanNum)
**参数说明		:  	CanNum		-->>	CAN控制器，值不能大于CAN_MAX_NUM 规定的值
**返回值		:	＝0，表示成功使CAN控制器进入工作模式； ！＝0，不成功；	
**说	明		:	本函数用于软件使能CAN控制器
************************************************************************************************************/
u32		SoftEnCAN(eCANNUM CanNum)
{
	CANMOD(CanNum).Bits.RM_BIT =0;
	return(CANMOD(CanNum).Bits.RM_BIT );
}
/*
***********************************************************************************************************
**函数原型		:  	void	CanSendCmd(eCANNUM CanNum,u32 Cmd,u32 TxBuf)
**参数说明		:  	CanNum		-->>	CAN控制器，值不能大于CAN_MAX_NUM 规定的值
					Cmd		--> 发送命令字
					TxBuf	-->	选择发送缓冲区
**返回值		:		
**说	明		:	本函数用于发送命令处理
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
			Temp.Bits.AT_BIT =1;						//单次发送
			Temp.Bits.TR_BIT =1;
			break;
		case	2:
		case	3:									//自发自收
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
			if(Cmd == 3)							//单次自发自收
			{
				Temp.Bits.AT_BIT =1;
			}
			break;
		case 	0:
		default:									//正常发送
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
**函数原型	:  	u32	WriteCanTxBuf(eCANNUM CanNum,u32 TxBufNum,
										u32 TPM,P_stcTxBUF Buf) ;
**参数说明	:  	CanNum		-->>	CAN控制器，值不能大于CAN_MAX_NUM 规定的值
				TxBufNum	-->	选择发送缓冲区
				TPM			-->	发送优先级模式
				Buf			-->	发送到总线的数据
**返回值		:	=0;成功将数据写入发送缓冲区
					!=0;写发送缓冲区操作失败	
**说	明		:	本函数用于向CAN发送缓冲区TxBUF写入发送数据
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
**函数原型	:  	void	ReadCanRxBuf(eCANNUM CanNum,P_stcRxBUF Buf)
**参数说明	:  	CanNum		-->>	CAN控制器，值不能大于CAN_MAX_NUM 规定的值
				Buf			-->>	接收CAN总线数据
**返回值		:	
**说	明		:	本函数用于从CAN控制器RxBuf中读取数据
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
**函数原型		:  	u32 	CanEntrySM(CanNum)
**参数说明		:  	CanNum	-->>	CAN控制器，值不能大于CAN_MAX_NUM 规定的值
**返回值		:	=0;成功进入睡眠状态;
					!=0;不能进入睡眠状态;	
**说	明		:	本函数用于使CAN控制器进入休眠模式
***********************************************************************************************************
*/
u32	CanEntrySM(eCANNUM	CanNum)
{
	CANMOD(CanNum).Bits.SM_BIT = 1;	
	return(!CANMOD(CanNum).Bits.SM_BIT);
}
/*
***********************************************************************************************************
**函数原型		:  	u32 CanQuitSM(eCANNUM	CanNum)
**参数说明		:  	CanNum		-->>	CAN控制器，值不能大于CAN_MAX_NUM 规定的值
**返回值		:	=0;成功退出睡眠状态;
					!=0;不能退出睡眠状态;	
**说	明		:	本函数用于使CAN控制器退出休眠模式
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
