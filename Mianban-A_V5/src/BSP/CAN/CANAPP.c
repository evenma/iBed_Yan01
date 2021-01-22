/****************************************Copyright (c)**************************************************
**                               ������������Ƭ����չ���޹�˾
**                                     ��    ��    ��
**                                        ��Ʒһ�� 
**                                 http://www.zlgmcu.com
**-----------------------------------------------------------�ļ���Ϣ--------------------------------------------------------------------------------
**	��   ��  	��:	CANAPP.C
** 	��  		��:	v1.0
** 	��		��:	2004��2��23��
**	��		��:	CANģ��Ӧ�ýӿں���˵���ļ������ļ��еĺ���������û����ر����Ҫ�����Խ����޸ġ�
********************************************************************************************************/
#define		_CANAPP_GLOBAL_
#include	<Common.h>
#include 	"IncludeCan.h"


/*
***********************************************************************************************************
**����ԭ��		:  	u32	CANSendData(eCANNUM CanNum,u32 Cmd,P_stcTxBUF Buf)
**����˵��		:  	CanNum	-->>	CAN��������ֵ���ܴ���CAN_MAX_NUM �涨��ֵ
					Cmd	-->	����������
					Buf	-->	Ҫ���͵�����
**����ֵ		:	0��ʾ�ɹ�����0��ʾʧ��
**˵	��		:	���������ڽ����ݷ��͵�CAN����
************************************************************************************************************/
u32		CANSendData(eCANNUM CanNum,u32 Cmd,P_stcTxBUF Buf)
{
	u32 i,status=0;
#if 0
	rt_kprintf("����CAN���ģ�id=%d,len=%d,da=%x,db=%x\r\n",Buf->TxCANID.Word,
				Buf->TxFrameInfo.Bits.DLC_BIT,
				Buf->CANTDA.Word,
				Buf->CANTDB.Word);
#endif
	while(CANMOD(CanNum).Bits.RM_BIT )
	{
		CANMOD(CanNum).Bits.RM_BIT =0;	// // ������ڸ�λ״̬��������Ҫʹ��һ��
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


//Ӧ�ó�������
//ʹ�ù���ģʽ
const 	u32	USE_MOD_CAN[4] = 
{
	USE_MODE_CAN1,
	USE_MODE_CAN2,
	USE_MODE_CAN3,
	USE_MODE_CAN4
};

//ʹ�ò�����
const 	u32	USE_BTR_CAN[4] = 
{
	USE_BTR_CAN1,
	USE_BTR_CAN2,
	USE_BTR_CAN3,
	USE_BTR_CAN4
};
//ʹ�÷��ͻ������������ȼ�ģʽѡ��
const 	u32	USE_TPM_CAN[4] = 
{
	USE_TPM_CAN1,
	USE_TPM_CAN2,
	USE_TPM_CAN3,
	USE_TPM_CAN4
};
//Ӧ���ж�
const 	u32	USE_INT_CAN[4] = 
{
	USE_INT_CAN1,
	USE_INT_CAN2,
	USE_INT_CAN3,
	USE_INT_CAN4
};
//Ӧ�ñ�������
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
