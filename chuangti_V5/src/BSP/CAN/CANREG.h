/****************************************Copyright (c)**************************************************
**                               广州周立功单片机发展有限公司
**                                     研    究    所
**                                        产品一部 
**                                 http://www.zlgmcu.com
**-----------------------------------------------------------文件信息--------------------------------------------------------------------------------
**	文   件  	名:	CANREG.H
** 	版  		本:	v1.0
** 	日		期:	2004年2月23日
**	描		述:	CAN模块寄存器类型定义、地址定义头文件
********************************************************************************************************/
#include	"../Common.h"
#ifndef	_CANREG_H_
#define	_CANREG_H_
//验收滤波器RAM首地址
#define	CANAFRAM_GADR			0xE0038000
#define	USE_SFF_sa_VAL			0
//CAN寄存器基地址定义
#define 	CANMOD_BADR			0xE0044000     	//模式寄存器
#define 	CANCMR_BADR			0xE0044004      	//命令寄存器
#define 	CANGSR_BADR			0xE0044008      	//全局状态寄存器
#define 	CANICR_BADR			0xE004400C     	//中断及捕获寄存器
#define 	CANIER_BADR			0xE0044010      	//中断使能寄存器
#define 	CANBTR_BADR			0xE0044014      	//总线时序寄存器
#define 	CANEWL_BADR			0xE0044018      	//报警限制寄存器
#define 	CANSR_BADR  		0xE004401C      	//状态寄存器
#define 	CANRFS_BADR			0xE0044020      	//接收帧信息寄存器
#define		CANRID_BADR			0xE0044024      	//接收报文ID寄存器

#define 	CANRDA_BADR			0xE0044028      	//接收数据1~4
#define 	CANRDB_BADR 			0xE004402C      	//接收数据5～8
#define 	CANTFI1_BADR			0xE0044030      	//CAN发送缓冲区1帧信息寄存器
#define 	CANTID1_BADR			0xE0044034      	// CAN发送缓冲区1报文ID寄存器
#define 	CANTDA1_BADR			0xE0044038      	// CAN发送缓冲区1发送数据1～4寄存器
#define 	CANTDB1_BADR			0xE004403C      	// CAN发送缓冲区1发送数据5～8寄存器
#define 	CANTFI2_BADR			0xE0044040      	// CAN发送缓冲区2帧信息寄存器
#define 	CANTID2_BADR			0xE0044044      	// CAN发送缓冲区2报文ID寄存器
#define 	CANTDA2_BADR			0xE0044048     	// CAN发送缓冲区2发送数据1～4寄存器
#define 	CANTDB2_BADR			0xE004404C      	// CAN发送缓冲区2发送数据5～8寄存器
#define 	CANTFI3_BADR			0xE0044050      	// CAN发送缓冲区3帧信息寄存器
#define 	CANTID3_BADR			0xE0044054      	// CAN发送缓冲区3报文ID寄存器
#define 	CANTDA3_BADR			0xE0044058      	// CAN发送缓冲区3发送数据1～4寄存器
#define 	CANTDB3_BADR			0xE004405C      	// CAN发送缓冲区3发送数据5～8寄存器
//定义RxBUF的基地址
#define	RxBUF_BADR				0xE0044020		
//定义TxBUF的基地址
#define	TxBUF1_BADR				0xE0044030		// CAN发送缓冲区1首地址
#define	TxBUF2_BADR				0xE0044040		// CAN发送缓冲区2首地址
#define	TxBUF3_BADR				0xE0044050		// CAN发送缓冲区3首地址
//定义全局应用的寄存器
#define 	CANTxSR_GADR   			0xE0040000      	//发送状态寄存器
#define 	CANRxSR_GADR      		0xE0040004      	//接收状态寄存器
#define 	CANMSR_GADR       		0xE0040008      	//错误状态寄存器
#define 	CANAFMR_GADR      		0xE003C000      	//验收滤波控制
#define 	CANSFF_sa_GADR      	0xE003C004      	//标准帧表格
#define 	CANSFF_GRP_sa_GADR 		0xE003C008      	//标准帧组表格
#define 	CANEFF_sa_GADR      	0xE003C00C      	//扩展帧表格
#define 	CANEFF_GRP_sa_GADR 		0xE003C010      	//扩展帧组表格
#define 	CANENDofTable_GADR  	0xE003C014      	//表格结束地址
#define 	CANLUTerrAd_GADR   		0xE003C018      	//LUT错误地址地址寄存器
#define 	CANLUTerr_GADR      	0xE003C01C		//LUT错误寄存器
//数据类型定义
//CAN控制器通道定义
typedef 	enum		_cannum_				
{ 
//#if		CAN_MAX_NUM  == 4		//微处理器中含有4路CAN
	CAN1 = 0, 						//CAN1模块
	CAN2,							//CAN2模块
	CAN3,							//CAN3模块
	CAN4							//CAN4模块
//#elif	CAN_MAX_NUM  == 2			//微处理器中含有2路CAN
//	CAN1 = 0, 						//CAN1模块
//	CAN2							//CAN2模块
//#endif
} eCANNUM;		

//CAN滤波器标准帧表格数据类型定义
typedef	union		_fullcansffline_
{
	u32	Word;
	struct	
	{
		u32	CANSFFID1 	:11;	
		u32	RSV_BIT1		:1;	
		u32	EN_BIT1 		:1;	
		u32	Num_BIT1 	:3;	
		u32	CANSFFID2 	:11;
		u32	RSV_BIT2		:1;	
		u32	EN_BIT2 		:1;	
		u32	Num_BIT2 	:3;	
	}Bits;
}uFullCANSFFLine,*P_uFullCANSFFLine;
//CAN滤波器扩展帧表格数据类型定义
typedef	union		_fullcaneffline_
{
	u32	Word;
	struct	
	{
		u32	CANEFFID1 	:29;
		u32	Num_BIT 		:3;	
	}Bits;
}uFullCANEFFLine,*P_uFullCANEFFLine;
//CAN模式寄存器数据类型定义
typedef	union		_canmod_
{
	u32	Word;
	struct	{
		u32	RM_BIT 		:1;		//定义RM位
		u32	LOM_BIT 	:1;		//定义LOM位
		u32	STM_BIT		:1;		//定义STM位
		u32	TPM_BIT 	:1;		//定义TPM位
		u32	SM_BIT 		:1;		//定义SM位
		u32	RPM_BIT 	:1;		//定义RPM位
		u32	RSV_BIT1 	:1;		//保留位
		u32	TM_BIT 		:1;		//定义TM位
		u32	RSV_BIT24	:24;		//保留位
	}Bits;
}uCANMod,*P_uCANMod;
//命令寄存器的数据类型定义
typedef	union		_cancmr_
{
u32	Word;
	struct	
	{
		u32	TR_BIT 		:1;		//定义TR位
		u32	AT_BIT 		:1;		//定义AT位
		u32	RRB_BIT 	:1;		//定义RRB位
		u32	CDO_BIT 	:1;	
		u32	SRR_BIT 	:1;		//定义SRR位
		u32	STB1_BIT 	:1;		//定义SEND TxBUF1位
		u32	STB2_BIT 	:1;		//定义SEND TxBUF2位
		u32	STB3_BIT 	:1;		//定义SEND TxBUF3位
		u32	RSV_BIT24	:24;		//保留位
	}Bits;
}uCANCMR,*P_uCANCMR;
//全局状态寄存器的数据类型定义
typedef	union		_cangsr_
{
	u32	Word;				//字操作定义
	struct	
	{
		u32	RBS_BIT 		:1;	//接收缓冲区状态位
		u32	DOS_BIT 		:1; 	//数据溢出状态位
		u32	TBS_BIT 		:1;	//发送缓冲区状态锁定位
		u32	TCS_BIT 		:1;	//发送完成状态位
		u32	RS_BIT 		:1;	//正在接收状态
		u32	TS_BIT 		:1;	//正在发送状态
		u32	ES_BIT	 	:1;	//错误状态
		u32	BS_BIT 		:1;	//总线关闭
		u32	RSV_BIT8		:8;	//保留位
		u32	RXERR_BIT 	:8;	//接收错误计数
		u32	TXERR_BIT 	:8;	//发送错误计数
	}Bits;
}uCANGSR,*P_uCANGSR;
//中断捕获寄存器的数据类型定义
typedef	union		_canicr_
{
	u32	Word;				//字操作定义
	struct	
	{
		u32	RI_BIT 		:1;	//接收中断位
		u32	TI1_BIT 		:1;	//TxBUF1发送成功中断位
		u32	EI_BIT 		:1;	//错误报警中断位
		u32	DOI_BIT 		:1;	//数据溢出中断位
		u32	WUI_BIT 		:1;	//睡眠唤醒中断位
		u32	EPI_BIT 		:1;	//错误认可中断位
		u32	ALI_BIT 		:1;	//仲裁错误中断位
		u32	BEI_BIT 		:1;	//总线错误中断
		u32	IDI_BIT 		:1;	//接收到CAN标识符中断位
		u32	TI2_BIT 		:1;	// TxBUF2发送成功中断位
		u32	TI3_BIT 		:1;	// TxBUF3发送成功中断位
		u32	RSV_BIT5		:5;	//保留位
		u32	ERRBIT_BIT 	:5;	//错误代码捕获
		u32	ERRDIR_BIT 	:1;	//错误方向
		u32	ERRC_BIT 	:2;	//错误类型
		u32	ALCBIT_BIT 	:5;	//仲裁错误代码捕获
		u32	RSV_BIT3		:3;	//保留
	}Bits;
}uCANICR,*P_uCANICR;
//中断使能寄存器的数据类型定义
typedef	union		_canier_
{
	u32	Word;			//字操作定义
	struct	
	{
		u32	RIE_BIT 		:1;	//接收中断使能位
		u32	TIE1_BIT 		:1;	//TxBUF1发送完成中断使能位
		u32	EIE_BIT 		:1;	//错误报警中断使能位
		u32	DOIE_BIT 	:1;	//数据溢出中断使能位
		u32	WUIE_BIT 	:1;	//睡眠模式唤醒中断使能位
		u32	EPIE_BIT 	:1;	//错误认可中断使能位
		u32	ALIE_BIT 	:1;	//仲裁丢失中断使能位
		u32	BEIE_BIT 	:1;	//总线错误中断使能位
		u32	IDIE_BIT	 	:1;	//接收到CAN标识符中断使能位
		u32	TIE2_BIT 		:1;	//TxBUF2发送完成中断使能位
		u32	TIE3_BIT	 	:1;	//TxBUF3发送完成中断使能位
		u32	RSV_BIT21	:21;	//保留位
	}Bits;
}uCANIER,*P_uCANIER;
//总线时序寄存器的数据类型定义
typedef	union		_canbtr_
{
	u32	Word;					//字操作定义
	struct	
	{
		u32	BRP_BIT 		:10;		//预分频位组合定义
		u32	RSV_BIT4		:4;		//保留位
		u32	SJW_BIT 		:2;		//同步跳转宽度
		u32	TSEG1_BIT 	:4;		//时间段1
		u32	TSEG2_BIT 	:3;		//时间段2
		u32	SAM_BIT 	:1;		//采样模式位
		u32	RSV_BIT8		:8;		//保留
	}Bits;
}uCANBTR,*P_uCANBTR;
//出错警告界限寄存器的数据类型定义
typedef	union	_canewl_
{
	u32	Word;					//字操作定义
	struct	
	{
		u32	EWL_BIT 	:8;		///出错警告界限值
		u32	RSV_BIT24	:24;		//保留位
	}Bits;
}uCANEWL,*P_uCANEWL;
//状态寄存器的数据类型定义
typedef	union	_cansr_
{
	u32	Word;					//字操作定义
	struct	
	{
		u32	RBS1_BIT 	:1;		//接收缓冲区有效
		u32	DOS1_BIT 	:1;		//数据溢出
		u32	TBS1_BIT 	:1;		//TxBUF1锁定
		u32	TCS1_BIT 	:1;		//TxBUF1的数据发送完成
		u32	RS1_BIT 		:1;		//正在接收
		u32	TS1_BIT 		:1;		//TxBUF1的数据正在发送
		u32	ES1_BIT 		:1;		//错误认可
		u32	BS1_BIT 		:1;		//总线错误
		u32	RBS2_BIT 	:1;		//
		u32	DOS2_BIT 	:1;		//
		u32	TBS2_BIT 	:1;		// TxBUF2锁定
		u32	TCS2_BIT 	:1;		// TxBUF2的数据发送完成
		u32	RS2_BIT 		:1;		//
		u32	TS2_BIT 		:1;		// TxBUF2的数据正在发送
		u32	ES2_BIT 		:1;		//
		u32	BS2_BIT 		:1;		//
		u32	RBS3_BIT 	:1;		//
		u32	DOS3_BIT 	:1;		//
		u32	TBS3_BIT 	:1;		// TxBUF3锁定
		u32	TCS3_BIT 	:1;		// TxBUF3的数据发送完成
		u32	RS3_BIT 		:1;		//
		u32	TS3_BIT 		:1;		// TxBUF3的数据正在发送
		u32	ES3_BIT 		:1;		//
		u32	BS3_BIT 		:1;		//
		u32	RSV_BIT8		:8;		//
	}Bits;
}uCANSR,*P_uCANSR;
//CANRFS寄存器的数据类型定义
typedef	union		_canrfs_
{
	u32		Word;				//字操作定义
	struct	
	{
		u32	IDIN_BIT 	:10;		//ID索引值
		u32	BP_BIT 		:1;		//BP
		u32	RSV_BIT5 	:5;		//保留
		u32	DLC_BIT 		:4;		//数据长度
		u32	RSV_BIT10	:10;		//保留
		u32	RTR_BIT 		:1;		//远程帧、数据帧识别位
		u32	FF_BIT 		:1;		//扩展帧、标准帧识别位
	}Bits;
}uCANRFS,*P_uCANRFS;
//CANID寄存器的数据类型定义
typedef	union		_canid_
{
	u32		Word;					//字操作定义
	union	_frame_format
	{
		struct		_strFrameFm_				//标准帧结构
		{
			u32	ID_BIT 		:11;
			u32	RSV_BIT21	:21;
		}stcSTRFRFM;
		struct		_etxFrameFm_				//扩展帧结构
		{
			u32	ID_BIT 		:29;
			u32	RSV_BIT3 	:3;
		}stcETXFRFM;
	}FrameFm;
}uCANID,*P_uCANID;
//数据寄存器数据类型定义
typedef	union		_candf_
{
	u32		Word;					//字操作定义
	struct	
	{
		u32	Data1_BIT 	:8;			//数据字节
		u32	Data2_BIT 	:8;	
		u32	Data3_BIT 	:8;	
		u32	Data4_BIT 	:8;	
	}Bits;
}uCANDF,*P_uCANDF;
//CANTFI寄存器的数据类型定义
typedef	union		_cantfi_
{
	u32		Word;				//字节操作定义
	struct	
	{
		u32	PRIO_BIT 	:8;		//缓冲区数据发送优先级的值
		u32	RSV_BIT8		:8;						
		u32	DLC_BIT 		:4;		//发送数据长度
		u32	RSV_BIT10	:10;
		u32	RTR_BIT 		:1;		//远程帧、数据帧辨别位
		u32	FF_BIT 		:1;		//扩展帧、标准帧辨别位
	}Bits;
}uCANTFI,*P_uCANTFI;
//定义发送帧结构
typedef	struct		_stcTXBUF_
{
	uCANTFI		TxFrameInfo;	//发送帧信息结构
	uCANID		TxCANID;		//发送帧信息ID
	uCANDF		CANTDA;		//发送数据1～4字节
	uCANDF		CANTDB;		//发送数据5～8字节
}*P_stcTxBUF,stcTxBUF;

//定义集中发送状态寄存器结构
typedef	union		_uCANTxSR_
{
	u32		Word;				//字操作定义
	struct	
	{
		u32	TS1_BIT 		:1;		//CAN1正在发送状态
		u32	TS2_BIT 		:1;		//CAN2正在发送状态
		u32	TS3_BIT 		:1;		//CAN3正在发送状态
		u32	TS4_BIT 		:1;		//CAN4正在发送状态
		u32	RSV_BIT1		:4;		//保留
		u32	TBS1_BIT 		:1;		//CAN1发送缓冲区锁定状态
		u32	TBS2_BIT 		:1;		//CAN2发送缓冲区锁定状态
		u32	TBS3_BIT 	:1;		//CAN3发送缓冲区锁定状态
		u32	TBS4_BIT 	:1;		//CAN4发送缓冲区锁定状态
		u32	RSV_BIT2 	:4;		// 
		u32	TCS1_BIT 	:1;		//CAN1发送完成状态
		u32	TCS2_BIT 	:1;		//CAN2发送完成状态
		u32	TCS3_BIT 	:1;		//CAN3发送完成状态
		u32	TCS4_BIT 	:1;		//CAN4发送完成状态
		u32	RSV_BIT12	:12;		//保留
	}Bits;
}*P_uCANTxSR,uCANTxSR;
//定义集中接收状态寄存器结构
typedef	union		_uCANRxSR_
{
	u32		Word;				//字操作定义
	struct	
	{
		u32	RS1_BIT 		:1;		//CAN1正在接收状态
		u32	RS2_BIT 		:1;		//CAN2正在接收状态
		u32	RS3_BIT 		:1;		//CAN3正在接收状态
		u32	RS4_BIT 		:1;		//CAN4正在接收状态
		u32	RSV_BIT1		:4;		//保留
		u32	RBS1_BIT 	:1;		//CAN1接收缓冲区有效
		u32	RBS2_BIT 	:1;		//CAN2接收缓冲区有效
		u32	RBS3_BIT 	:1;		//CAN3接收缓冲区有效
		u32	RBS4_BIT 	:1;		//CAN4接收缓冲区有效
		u32	RSV_BIT2 	:4;		// 
		u32	DOS1_BIT 	:1;		//CAN1接收缓冲区溢出
		u32	DOS2_BIT 	:1;		//CAN2接收缓冲区溢出
		u32	DOS3_BIT 	:1;		//CAN3接收缓冲区溢出
		u32	DOS4_BIT 	:1;		//CAN4接收缓冲区溢出
		u32	RSV_BIT12	:12;		//保留
	}Bits;
}*P_uCANRxSR, uCANRxSR;

//RxBuf Struct
typedef	struct	_stcRXBUF_
{
	uCANRFS	CANRcvFS;
	uCANID	RxCANID;
	uCANDF	CANRDA;
	uCANDF	CANRDB;
}*P_stcRxBUF,stcRxBUF;

//定义集中其他状态寄存器结构
typedef	union		_uCANMSR_
{
	u32		Word;				//字操作定义
	struct	
	{
		u32	ES1_BIT 		:1;		//CAN1错误报警
		u32	ES2_BIT 		:1;		//CAN2错误报警
		u32	ES3_BIT 		:1;		//CAN3错误报警
		u32	ES4_BIT 		:1;		//CAN4错误报警
		u32	RSV_BIT1		:4;		//保留
		u32	BS1_BIT 		:1;		//CAN1总线脱离
		u32	BS2_BIT 		:1;		//CAN2总线脱离
		u32	BS3_BIT 		:1;		//CAN3总线脱离
		u32	BS4_BIT 		:1;		//CAN4总线脱离
		u32	RSV_BIT20	:20;		//保留
	}Bits;
}*P_uCANMSR, uCANMSR;
//定义验收滤波模式寄存器结构
typedef	union		_canafmr_
{
	u32	Word;
	struct	
	{
		u32	AccOff_BIT 	:1;	
		u32	AccBP_BIT 	:1;									
		u32	eFCAN_BIT 	:1;	
		u32	RSV_BIT29 	:29;
	}Bits;
}uCANAFMR,*P_uCANAFMR;
//验收滤波器表格单个标志符表格起始地址数据结构定义
typedef	union	_canF_sa_
{
	u32	Word;
	struct	
	{
		u32	RSV_BIT2 	:2;
		u32	SADR_BIT 	:9;	
		u32	RSV_BIT21 	:21;
	}Bits;
}uCANF_sa,*P_uCANF_sa;
//验收滤波器表格标志符组表格起始地址数据结构定义
typedef	union	_canGRP_sa_
{
	u32	Word;
	struct	
	{
		u32	RSV_BIT2 	:2;
		u32	SADR_BIT 	:10;
		u32	RSV_BIT20 	:20;
	}Bits;
}uCANGRP_sa,*P_uCANGRP_sa;

//LUT错误寄存器数据结构定义
typedef	union	_canLUTerr
{
	u32	Word;
	struct	
	{
		u32	LERR_BIT 	:1;
		u32	RSV_BIT31 	:31;
	}Bits;
}uCANLUTerr,*P_uCANLUTerr;

#endif
/*********************************************************************************************************
**                            			End Of File
********************************************************************************************************/
