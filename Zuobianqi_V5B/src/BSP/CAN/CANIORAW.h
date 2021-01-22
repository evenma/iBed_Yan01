/****************************************Copyright (c)**************************************************
**                               ������������Ƭ����չ���޹�˾
**                                     ��    ��    ��
**                                        ��Ʒһ�� 
**                                 http://www.zlgmcu.com
**-----------------------------------------------------------�ļ���Ϣ--------------------------------------------------------------------------------
**	��   ��  	��:	CANIORAW.H
** 	��  		��:	v1.0
** 	��		��:	2004��2��23��
**	��		��:	CANģ��Ĵ������ʷ�������ͷ�ļ�
********************************************************************************************************/
#ifndef	_CANIORAW_H_
#define	_CANIORAW_H_

//��������CANģ��ģʽ�Ĵ�������������
#define 	CANMOD(CanNum)		(*((volatile P_uCANMod)(CANMOD_BADR+CanNum* CAN_OFFSET_ADR)))
//��������CANģ������Ĵ�������������
#define 	CANCMR(CanNum)		(*((volatile P_uCANCMR)(CANCMR_BADR+CanNum* CAN_OFFSET_ADR )))    
//��������CANģ��ȫ��״̬�Ĵ�������������
#define 	CANGSR(CanNum)		(*((volatile P_uCANGSR)( CANGSR_BADR+CanNum* CAN_OFFSET_ADR )))    
//��������CANģ���жϺͲ���Ĵ�������������
#define 	CANICR(CanNum)		(*((volatile P_uCANICR)( CANICR_BADR+CanNum* CAN_OFFSET_ADR )))    
//��������CANģ���ж�ʹ�ܼĴ�������������
#define 	CANIER(CanNum)		(*((volatile P_uCANIER)( CANIER_BADR+CanNum* CAN_OFFSET_ADR )))    
//��������CANģ������ʱ��Ĵ�������������
#define 	CANBTR(CanNum)		(*((volatile P_uCANBTR)( CANBTR_BADR+CanNum* CAN_OFFSET_ADR )))    
//��������CANģ���������޼Ĵ�������������
#define 	CANEWL(CanNum)		(*((volatile P_uCANEWL)( CANEWL_BADR+CanNum* CAN_OFFSET_ADR )))    
//��������CANģ��CANSR�Ĵ�������������
#define 	CANSR(CanNum)		(*((volatile P_uCANSR)( CANSR_BADR+CanNum* CAN_OFFSET_ADR )))    
//����CANģ���CANRFS�������Ͷ���
#define 	CANRFS(CanNum)		(*((volatile P_uCANRFS)( CANRFS_BADR+CanNum* CAN_OFFSET_ADR )))    
//����CANģ���CANRID�������Ͷ���
#define 	CANRID(CanNum)		(*((volatile P_uCANID)( CANRID_BADR+CanNum* CAN_OFFSET_ADR )))    
//����CANģ���CANRDA�������Ͷ���
#define 	CANRDA(CanNum)		(*((volatile P_uCANDF)( CANRDA_BADR+CanNum* CAN_OFFSET_ADR )))    
//����CANģ���CANRDB�������Ͷ���
#define 	CANRDB(CanNum)		(*((volatile P_uCANDF)( CANRDB_BADR+CanNum* CAN_OFFSET_ADR )))    
//��������CANģ��ķ��ͻ���������
#define CANTFI1(CanNum)			(*((volatile P_uCANTFI)(CANTFI1_BADR+CanNum* CAN_OFFSET_ADR)))    
#define CANTID1(CanNum)		(*((volatile P_uCANID)(CANTID1_BADR+CanNum*CAN_OFFSET_ADR)))   
#define CANTDA1(CanNum)		(*((volatile P_uCANDF)(CANTDA1_BADR+CanNum*CAN_OFFSET_ADR)))   
#define CANTDB1(CanNum)		(*((volatile P_uCANDF)(CANTDB1_BADR+CanNum*CAN_OFFSET_ADR)))  
#define CANTFI2(CanNum)			(*((volatile P_uCANTFI)(CANTFI2_BADR+CanNum*CAN_OFFSET_ADR)))    
#define CANTID2(CanNum)		(*((volatile P_uCANID)(CANTID2_BADR+CanNum*CAN_OFFSET_ADR))) 
#define CANTDA2(CanNum)		(*((volatile P_uCANDF)(CANTDA2_BADR+CanNum*CAN_OFFSET_ADR)))
#define CANTDB2(CanNum)		(*((volatile P_uCANDF)(CANTDB2_BADR+CanNum*CAN_OFFSET_ADR)))  
#define CANTFI3(CanNum)			(*((volatile P_uCANTFI)(CANTFI3_BADR+CanNum*CAN_OFFSET_ADR)))    
#define CANTID3(CanNum)		(*((volatile P_uCANID)(CANTID3_BADR+CanNum*CAN_OFFSET_ADR)))
#define CANTDA3(CanNum)		(*((volatile P_uCANDF)(CANTDA3_BADR+CanNum*CAN_OFFSET_ADR))) 
#define CANTDB3(CanNum)		(*((volatile P_uCANDF)(CANTDB3_BADR+CanNum*CAN_OFFSET_ADR)))
//��������CANģ���TxBUF
#define	TxBUF1(CanNum)		(*((volatile P_stcTxBUF)( TxBUF1_BADR+CanNum*CAN_OFFSET_ADR)))
#define	TxBUF2(CanNum)		(*((volatile P_stcTxBUF)( TxBUF2_BADR+CanNum*CAN_OFFSET_ADR)))
#define	TxBUF3(CanNum)		(*((volatile P_stcTxBUF)( TxBUF3_BADR+CanNum*CAN_OFFSET_ADR)))
//����״̬�Ĵ���
#define 	CANTxSR				(*((volatile P_uCANTxSR) CANTxSR_GADR))
#define 	CANRxSR				(*((volatile P_uCANRxSR) CANRxSR_GADR))
#define 	CANMSR				(*((volatile P_uCANMSR) CANMSR_GADR))
//�����˲�������
#define 	CANAFMR				(*((volatile P_uCANAFMR) CANAFMR_GADR))
#define 	CANSFF_sa			(*((volatile P_uCANF_sa)(CANSFF_sa_GADR)))    
#define 	CANSFF_GRP_sa		(*((volatile P_uCANGRP_sa)(CANSFF_GRP_sa_GADR)))    
#define 	CANEFF_sa			(*((volatile P_uCANF_sa)(CANEFF_sa_GADR)))    
#define 	CANEFF_GRP_sa		(*((volatile P_uCANGRP_sa)(CANEFF_GRP_sa_GADR))) 
#define 	CANENDofTable		(*((volatile P_uCANGRP_sa)(CANENDofTable_GADR)))
#define 	CANLUTerrAd			(*((volatile P_uCANF_sa)(CANLUTerrAd_GADR)))
//LUT����Ĵ������ʷ�ʽ����
#define 	CANLUTerr			(*((volatile P_uCANLUTerr)( CANLUTerr_GADR)))
//�����˲������
#define		CANSFF_TAB(i)			(*((volatile P_uFullCANSFFLine)( CANAFRAM_GADR + USE_SFF_sa_VAL +i*4)))	
#define 	CANSFF_GRP_TAB(i)		(*((volatile P_uFullCANSFFLine)( CANAFRAM_GADR + USE_SFF_GRP_sa_VAL +i*4)))
#define		CANEFF_TAB(i)			(*((volatile P_uFullCANEFFLine)( CANAFRAM_GADR + USE_EFF_sa_VAL +i*4)))	
#define 	CANEFF_GRP_TAB(i)		(*((volatile P_uFullCANEFFLine)( CANAFRAM_GADR + USE_EFF_GRP_sa_VAL +i*4)))

//��������CANģ���RxBUF
#define	RxBUF(CanNum)	(*((volatile P_stcRxBUF)( RxBUF_BADR+CanNum*CAN_OFFSET_ADR)))
#endif
/*********************************************************************************************************
**                            			End Of File
********************************************************************************************************/
