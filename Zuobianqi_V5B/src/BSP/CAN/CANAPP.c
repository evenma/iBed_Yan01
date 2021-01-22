/****************************************Copyright (c)**************************************************
**                               广州周立功单片机发展有限公司
**                                     研    究    所
**                                        产品一部 
**                                 http://www.zlgmcu.com
**-----------------------------------------------------------文件信息--------------------------------------------------------------------------------
**	文   件  	名:	CANAPP.C
** 	版  		本:	v1.0
** 	日		期:	2004年2月23日
**	描		述:	CAN模块应用接口函数说明文件。该文件中的函数，如果用户有特别的需要，可以进行修改。
********************************************************************************************************/
#define		_CANAPP_GLOBAL_
#include	<Common.h>
#include 	"IncludeCan.h"


/*
***********************************************************************************************************
**函数原型		:  	u32	CANSendData(eCANNUM CanNum,u32 Cmd,P_stcTxBUF Buf)
**参数说明		:  	CanNum	-->>	CAN控制器，值不能大于CAN_MAX_NUM 规定的值
					Cmd	-->	发送命令字
					Buf	-->	要发送的数据
**返回值		:	0表示成功，非0表示失败
**说	明		:	本函数用于将数据发送到CAN总线
************************************************************************************************************/
u32		CANSendData(eCANNUM CanNum,u32 Cmd,P_stcTxBUF Buf)
{
	u32 i,status=0;
#if 0
	rt_kprintf("发送CAN报文，id=%d,len=%d,da=%x,db=%x\r\n",Buf->TxCANID.Word,
				Buf->TxFrameInfo.Bits.DLC_BIT,
				Buf->CANTDA.Word,
				Buf->CANTDB.Word);
#endif
	while(CANMOD(CanNum).Bits.RM_BIT )
	{
		CANMOD(CanNum).Bits.RM_BIT =0;	// // 如果处于复位状态，首先需要使能一下
	}

	if(0 != CANSR(CanNum).Bits.TBS1_BIT)
	{
		i=SEND_TX_BUF1;
	}
	else if(0 != CANSR(CanNum).Bits.TBS2_BIT)
	{
		i=SEND_TX_BUF2;
	}
	else if(0 != CANSR(CanNum).Bits.TBS3_BIT)
	{
		i=SEND_TX_BUF3;
	}
	else
	{
		i=0xFF;
	}
	status=WriteCanTxBuf(CanNum,i,  USE_TPM_CAN[CanNum],  Buf);
	if(status == 0)
	{
		#if 1
		if(CANMOD(CanNum).Bits.SM_BIT != 0)												
		{
			CanQuitSM(CanNum);
		}
		#endif
		CanSendCmd(CanNum,Cmd,i);
	}
	return (status);
}


//应用常量定义
//使用工作模式
const 	u32	USE_MOD_CAN[4] = 
{
	USE_MODE_CAN1,
	USE_MODE_CAN2,
	USE_MODE_CAN3,
	USE_MODE_CAN4
};

//使用波特率
const 	u32	USE_BTR_CAN[4] = 
{
	USE_BTR_CAN1,
	USE_BTR_CAN2,
	USE_BTR_CAN3,
	USE_BTR_CAN4
};
//使用发送缓冲区发送优先级模式选择
const 	u32	USE_TPM_CAN[4] = 
{
	USE_TPM_CAN1,
	USE_TPM_CAN2,
	USE_TPM_CAN3,
	USE_TPM_CAN4
};
//应用中断
const 	u32	USE_INT_CAN[4] = 
{
	USE_INT_CAN1,
	USE_INT_CAN2,
	USE_INT_CAN3,
	USE_INT_CAN4
};
//应用报警限制
const	u32	USE_EWL_CAN[4] = 
{
	USE_EWL_CAN1,
	USE_EWL_CAN2,
	USE_EWL_CAN3,
	USE_EWL_CAN4
};
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
