/****************************************Copyright (c)**************************************************
**                               广州周立功单片机发展有限公司
**                                     研    究    所
**                                        产品一部 
**                                 http://www.zlgmcu.com
**-----------------------------------------------------------文件信息--------------------------------------------------------------------------------
**	文   件  	名:	CUSTOM.H
** 	版  		本:	v1.0
** 	日		期:	2004年3月2日
**	描		述:	用户特殊选项配置头文件
********************************************************************************************************/
#ifndef	_CUSTOM_H_
#define	_CUSTOM_H_
//当VPB时钟为11059200Hz时，常用波特率与总线时序器对照表,如果VPB时钟不等，请自己计算出总线时序器的值
#define	BPS_250K				0x0017C003				//CAN的波特率250Kbps时总线时序器的值
//全局应用定义
#define	CAN_MAX_NUM				1					//微处理器所含有的最大CAN模块的数目
#define	CAN_OFFSET_ADR			0x4000				//CAN各模块寄存器之间的线性差异
#define	USE_CAN_RCV_BUF_SIZE	0x20				//驱动程序使用的接收环形缓冲区的大小
//各路CAN独立应用定义
#define	USE_MODE_CAN1			0x00				//CAN模块1的工作模式定义
#define	USE_MODE_CAN2			0x00				// CAN模块2的工作模式定义
#define	USE_MODE_CAN3			0x00				// CAN模块3的工作模式定义
#define	USE_MODE_CAN4			0x00				// CAN模块4的工作模式定义


#define	USE_EWL_CAN1			0x60				// CAN模块1的错误报警界限值
#define	USE_EWL_CAN2			0x60				// CAN模块2的错误报警界限值
#define	USE_EWL_CAN3			0x60				// CAN模块3的错误报警界限值
#define	USE_EWL_CAN4			0x60				// CAN模块4的错误报警界限值


#define	USE_INT_CAN1			0x7f	//BIT0|BIT7		// CAN模块1的中断使能方式
#define	USE_INT_CAN2			0x7f	//BIT0|BIT7		// CAN模块2的中断使能方式
#define	USE_INT_CAN3			0x7f	//BIT0|BIT7		// CAN模块3的中断使能方式
#define	USE_INT_CAN4			0x7f	//BIT0|BIT7		// CAN模块4的中断使能方式


#define	USE_TPM_CAN1			0x01				// CAN模块1的发送优先级模式
#define	USE_TPM_CAN2			0x01				// CAN模块2的发送优先级模式
#define	USE_TPM_CAN3			0x01				// CAN模块3的发送优先级模式
#define	USE_TPM_CAN4			0x01				// CAN模块4的发送优先级模式


#define	USE_BTR_CAN1			BPS_250K			// CAN模块1的总线波特率
#define	USE_BTR_CAN2			BPS_250K			// CAN模块2的总线波特率
#define	USE_BTR_CAN3			BPS_250K			// CAN模块3的总线波特率
#define	USE_BTR_CAN4			BPS_250K			// CAN模块4的总线波特率
#endif
/*********************************************************************************************************
**                            				End Of File
********************************************************************************************************/
