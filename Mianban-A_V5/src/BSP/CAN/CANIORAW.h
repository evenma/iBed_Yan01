/****************************************Copyright (c)**************************************************
**                               广州周立功单片机发展有限公司
**                                     研    究    所
**                                        产品一部 
**                                 http://www.zlgmcu.com
**-----------------------------------------------------------文件信息--------------------------------------------------------------------------------
**	文   件  	名:	CANIORAW.H
** 	版  		本:	v1.0
** 	日		期:	2004年2月23日
**	描		述:	CAN模块寄存器访问方法定义头文件
********************************************************************************************************/
#ifndef	_CANIORAW_H_
#define	_CANIORAW_H_

//定义所有CAN模块模式寄存器的数据类型
#define 	CANMOD(CanNum)		(*((volatile P_uCANMod)(CANMOD_BADR+CanNum* CAN_OFFSET_ADR)))
//定义所有CAN模块命令寄存器的数据类型
#define 	CANCMR(CanNum)		(*((volatile P_uCANCMR)(CANCMR_BADR+CanNum* CAN_OFFSET_ADR )))    
//定义所有CAN模块全局状态寄存器的数据类型
#define 	CANGSR(CanNum)		(*((volatile P_uCANGSR)( CANGSR_BADR+CanNum* CAN_OFFSET_ADR )))    
//定义所有CAN模块中断和捕获寄存器的数据类型
#define 	CANICR(CanNum)		(*((volatile P_uCANICR)( CANICR_BADR+CanNum* CAN_OFFSET_ADR )))    
//定义所有CAN模块中断使能寄存器的数据类型
#define 	CANIER(CanNum)		(*((volatile P_uCANIER)( CANIER_BADR+CanNum* CAN_OFFSET_ADR )))    
//定义所有CAN模块总线时序寄存器的数据类型
#define 	CANBTR(CanNum)		(*((volatile P_uCANBTR)( CANBTR_BADR+CanNum* CAN_OFFSET_ADR )))    
//定义所有CAN模块出错警告界限寄存器的数据类型
#define 	CANEWL(CanNum)		(*((volatile P_uCANEWL)( CANEWL_BADR+CanNum* CAN_OFFSET_ADR )))    
//定义所有CAN模块CANSR寄存器的数据类型
#define 	CANSR(CanNum)		(*((volatile P_uCANSR)( CANSR_BADR+CanNum* CAN_OFFSET_ADR )))    
//所有CAN模块的CANRFS数据类型定义
#define 	CANRFS(CanNum)		(*((volatile P_uCANRFS)( CANRFS_BADR+CanNum* CAN_OFFSET_ADR )))    
//所有CAN模块的CANRID数据类型定义
#define 	CANRID(CanNum)		(*((volatile P_uCANID)( CANRID_BADR+CanNum* CAN_OFFSET_ADR )))    
//所有CAN模块的CANRDA数据类型定义
#define 	CANRDA(CanNum)		(*((volatile P_uCANDF)( CANRDA_BADR+CanNum* CAN_OFFSET_ADR )))    
//所有CAN模块的CANRDB数据类型定义
#define 	CANRDB(CanNum)		(*((volatile P_uCANDF)( CANRDB_BADR+CanNum* CAN_OFFSET_ADR )))    
//定义所有CAN模块的发送缓冲区分量
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
//定义所有CAN模块的TxBUF
#define	TxBUF1(CanNum)		(*((volatile P_stcTxBUF)( TxBUF1_BADR+CanNum*CAN_OFFSET_ADR)))
#define	TxBUF2(CanNum)		(*((volatile P_stcTxBUF)( TxBUF2_BADR+CanNum*CAN_OFFSET_ADR)))
#define	TxBUF3(CanNum)		(*((volatile P_stcTxBUF)( TxBUF3_BADR+CanNum*CAN_OFFSET_ADR)))
//集中状态寄存器
#define 	CANTxSR				(*((volatile P_uCANTxSR) CANTxSR_GADR))
#define 	CANRxSR				(*((volatile P_uCANRxSR) CANRxSR_GADR))
#define 	CANMSR				(*((volatile P_uCANMSR) CANMSR_GADR))
//验收滤波器部分
#define 	CANAFMR				(*((volatile P_uCANAFMR) CANAFMR_GADR))
#define 	CANSFF_sa			(*((volatile P_uCANF_sa)(CANSFF_sa_GADR)))    
#define 	CANSFF_GRP_sa		(*((volatile P_uCANGRP_sa)(CANSFF_GRP_sa_GADR)))    
#define 	CANEFF_sa			(*((volatile P_uCANF_sa)(CANEFF_sa_GADR)))    
#define 	CANEFF_GRP_sa		(*((volatile P_uCANGRP_sa)(CANEFF_GRP_sa_GADR))) 
#define 	CANENDofTable		(*((volatile P_uCANGRP_sa)(CANENDofTable_GADR)))
#define 	CANLUTerrAd			(*((volatile P_uCANF_sa)(CANLUTerrAd_GADR)))
//LUT错误寄存器访问方式定义
#define 	CANLUTerr			(*((volatile P_uCANLUTerr)( CANLUTerr_GADR)))
//验收滤波器表格
#define		CANSFF_TAB(i)			(*((volatile P_uFullCANSFFLine)( CANAFRAM_GADR + USE_SFF_sa_VAL +i*4)))	
#define 	CANSFF_GRP_TAB(i)		(*((volatile P_uFullCANSFFLine)( CANAFRAM_GADR + USE_SFF_GRP_sa_VAL +i*4)))
#define		CANEFF_TAB(i)			(*((volatile P_uFullCANEFFLine)( CANAFRAM_GADR + USE_EFF_sa_VAL +i*4)))	
#define 	CANEFF_GRP_TAB(i)		(*((volatile P_uFullCANEFFLine)( CANAFRAM_GADR + USE_EFF_GRP_sa_VAL +i*4)))

//定义所有CAN模块的RxBUF
#define	RxBUF(CanNum)	(*((volatile P_stcRxBUF)( RxBUF_BADR+CanNum*CAN_OFFSET_ADR)))
#endif
/*********************************************************************************************************
**                            			End Of File
********************************************************************************************************/
