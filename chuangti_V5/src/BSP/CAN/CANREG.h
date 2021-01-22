/****************************************Copyright (c)**************************************************
**                               ������������Ƭ����չ���޹�˾
**                                     ��    ��    ��
**                                        ��Ʒһ�� 
**                                 http://www.zlgmcu.com
**-----------------------------------------------------------�ļ���Ϣ--------------------------------------------------------------------------------
**	��   ��  	��:	CANREG.H
** 	��  		��:	v1.0
** 	��		��:	2004��2��23��
**	��		��:	CANģ��Ĵ������Ͷ��塢��ַ����ͷ�ļ�
********************************************************************************************************/
#include	"../Common.h"
#ifndef	_CANREG_H_
#define	_CANREG_H_
//�����˲���RAM�׵�ַ
#define	CANAFRAM_GADR			0xE0038000
#define	USE_SFF_sa_VAL			0
//CAN�Ĵ�������ַ����
#define 	CANMOD_BADR			0xE0044000     	//ģʽ�Ĵ���
#define 	CANCMR_BADR			0xE0044004      	//����Ĵ���
#define 	CANGSR_BADR			0xE0044008      	//ȫ��״̬�Ĵ���
#define 	CANICR_BADR			0xE004400C     	//�жϼ�����Ĵ���
#define 	CANIER_BADR			0xE0044010      	//�ж�ʹ�ܼĴ���
#define 	CANBTR_BADR			0xE0044014      	//����ʱ��Ĵ���
#define 	CANEWL_BADR			0xE0044018      	//�������ƼĴ���
#define 	CANSR_BADR  		0xE004401C      	//״̬�Ĵ���
#define 	CANRFS_BADR			0xE0044020      	//����֡��Ϣ�Ĵ���
#define		CANRID_BADR			0xE0044024      	//���ձ���ID�Ĵ���

#define 	CANRDA_BADR			0xE0044028      	//��������1~4
#define 	CANRDB_BADR 			0xE004402C      	//��������5��8
#define 	CANTFI1_BADR			0xE0044030      	//CAN���ͻ�����1֡��Ϣ�Ĵ���
#define 	CANTID1_BADR			0xE0044034      	// CAN���ͻ�����1����ID�Ĵ���
#define 	CANTDA1_BADR			0xE0044038      	// CAN���ͻ�����1��������1��4�Ĵ���
#define 	CANTDB1_BADR			0xE004403C      	// CAN���ͻ�����1��������5��8�Ĵ���
#define 	CANTFI2_BADR			0xE0044040      	// CAN���ͻ�����2֡��Ϣ�Ĵ���
#define 	CANTID2_BADR			0xE0044044      	// CAN���ͻ�����2����ID�Ĵ���
#define 	CANTDA2_BADR			0xE0044048     	// CAN���ͻ�����2��������1��4�Ĵ���
#define 	CANTDB2_BADR			0xE004404C      	// CAN���ͻ�����2��������5��8�Ĵ���
#define 	CANTFI3_BADR			0xE0044050      	// CAN���ͻ�����3֡��Ϣ�Ĵ���
#define 	CANTID3_BADR			0xE0044054      	// CAN���ͻ�����3����ID�Ĵ���
#define 	CANTDA3_BADR			0xE0044058      	// CAN���ͻ�����3��������1��4�Ĵ���
#define 	CANTDB3_BADR			0xE004405C      	// CAN���ͻ�����3��������5��8�Ĵ���
//����RxBUF�Ļ���ַ
#define	RxBUF_BADR				0xE0044020		
//����TxBUF�Ļ���ַ
#define	TxBUF1_BADR				0xE0044030		// CAN���ͻ�����1�׵�ַ
#define	TxBUF2_BADR				0xE0044040		// CAN���ͻ�����2�׵�ַ
#define	TxBUF3_BADR				0xE0044050		// CAN���ͻ�����3�׵�ַ
//����ȫ��Ӧ�õļĴ���
#define 	CANTxSR_GADR   			0xE0040000      	//����״̬�Ĵ���
#define 	CANRxSR_GADR      		0xE0040004      	//����״̬�Ĵ���
#define 	CANMSR_GADR       		0xE0040008      	//����״̬�Ĵ���
#define 	CANAFMR_GADR      		0xE003C000      	//�����˲�����
#define 	CANSFF_sa_GADR      	0xE003C004      	//��׼֡���
#define 	CANSFF_GRP_sa_GADR 		0xE003C008      	//��׼֡����
#define 	CANEFF_sa_GADR      	0xE003C00C      	//��չ֡���
#define 	CANEFF_GRP_sa_GADR 		0xE003C010      	//��չ֡����
#define 	CANENDofTable_GADR  	0xE003C014      	//��������ַ
#define 	CANLUTerrAd_GADR   		0xE003C018      	//LUT�����ַ��ַ�Ĵ���
#define 	CANLUTerr_GADR      	0xE003C01C		//LUT����Ĵ���
//�������Ͷ���
//CAN������ͨ������
typedef 	enum		_cannum_				
{ 
//#if		CAN_MAX_NUM  == 4		//΢�������к���4·CAN
	CAN1 = 0, 						//CAN1ģ��
	CAN2,							//CAN2ģ��
	CAN3,							//CAN3ģ��
	CAN4							//CAN4ģ��
//#elif	CAN_MAX_NUM  == 2			//΢�������к���2·CAN
//	CAN1 = 0, 						//CAN1ģ��
//	CAN2							//CAN2ģ��
//#endif
} eCANNUM;		

//CAN�˲�����׼֡����������Ͷ���
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
//CAN�˲�����չ֡����������Ͷ���
typedef	union		_fullcaneffline_
{
	u32	Word;
	struct	
	{
		u32	CANEFFID1 	:29;
		u32	Num_BIT 		:3;	
	}Bits;
}uFullCANEFFLine,*P_uFullCANEFFLine;
//CANģʽ�Ĵ����������Ͷ���
typedef	union		_canmod_
{
	u32	Word;
	struct	{
		u32	RM_BIT 		:1;		//����RMλ
		u32	LOM_BIT 	:1;		//����LOMλ
		u32	STM_BIT		:1;		//����STMλ
		u32	TPM_BIT 	:1;		//����TPMλ
		u32	SM_BIT 		:1;		//����SMλ
		u32	RPM_BIT 	:1;		//����RPMλ
		u32	RSV_BIT1 	:1;		//����λ
		u32	TM_BIT 		:1;		//����TMλ
		u32	RSV_BIT24	:24;		//����λ
	}Bits;
}uCANMod,*P_uCANMod;
//����Ĵ������������Ͷ���
typedef	union		_cancmr_
{
u32	Word;
	struct	
	{
		u32	TR_BIT 		:1;		//����TRλ
		u32	AT_BIT 		:1;		//����ATλ
		u32	RRB_BIT 	:1;		//����RRBλ
		u32	CDO_BIT 	:1;	
		u32	SRR_BIT 	:1;		//����SRRλ
		u32	STB1_BIT 	:1;		//����SEND TxBUF1λ
		u32	STB2_BIT 	:1;		//����SEND TxBUF2λ
		u32	STB3_BIT 	:1;		//����SEND TxBUF3λ
		u32	RSV_BIT24	:24;		//����λ
	}Bits;
}uCANCMR,*P_uCANCMR;
//ȫ��״̬�Ĵ������������Ͷ���
typedef	union		_cangsr_
{
	u32	Word;				//�ֲ�������
	struct	
	{
		u32	RBS_BIT 		:1;	//���ջ�����״̬λ
		u32	DOS_BIT 		:1; 	//�������״̬λ
		u32	TBS_BIT 		:1;	//���ͻ�����״̬����λ
		u32	TCS_BIT 		:1;	//�������״̬λ
		u32	RS_BIT 		:1;	//���ڽ���״̬
		u32	TS_BIT 		:1;	//���ڷ���״̬
		u32	ES_BIT	 	:1;	//����״̬
		u32	BS_BIT 		:1;	//���߹ر�
		u32	RSV_BIT8		:8;	//����λ
		u32	RXERR_BIT 	:8;	//���մ������
		u32	TXERR_BIT 	:8;	//���ʹ������
	}Bits;
}uCANGSR,*P_uCANGSR;
//�жϲ���Ĵ������������Ͷ���
typedef	union		_canicr_
{
	u32	Word;				//�ֲ�������
	struct	
	{
		u32	RI_BIT 		:1;	//�����ж�λ
		u32	TI1_BIT 		:1;	//TxBUF1���ͳɹ��ж�λ
		u32	EI_BIT 		:1;	//���󱨾��ж�λ
		u32	DOI_BIT 		:1;	//��������ж�λ
		u32	WUI_BIT 		:1;	//˯�߻����ж�λ
		u32	EPI_BIT 		:1;	//�����Ͽ��ж�λ
		u32	ALI_BIT 		:1;	//�ٲô����ж�λ
		u32	BEI_BIT 		:1;	//���ߴ����ж�
		u32	IDI_BIT 		:1;	//���յ�CAN��ʶ���ж�λ
		u32	TI2_BIT 		:1;	// TxBUF2���ͳɹ��ж�λ
		u32	TI3_BIT 		:1;	// TxBUF3���ͳɹ��ж�λ
		u32	RSV_BIT5		:5;	//����λ
		u32	ERRBIT_BIT 	:5;	//������벶��
		u32	ERRDIR_BIT 	:1;	//������
		u32	ERRC_BIT 	:2;	//��������
		u32	ALCBIT_BIT 	:5;	//�ٲô�����벶��
		u32	RSV_BIT3		:3;	//����
	}Bits;
}uCANICR,*P_uCANICR;
//�ж�ʹ�ܼĴ������������Ͷ���
typedef	union		_canier_
{
	u32	Word;			//�ֲ�������
	struct	
	{
		u32	RIE_BIT 		:1;	//�����ж�ʹ��λ
		u32	TIE1_BIT 		:1;	//TxBUF1��������ж�ʹ��λ
		u32	EIE_BIT 		:1;	//���󱨾��ж�ʹ��λ
		u32	DOIE_BIT 	:1;	//��������ж�ʹ��λ
		u32	WUIE_BIT 	:1;	//˯��ģʽ�����ж�ʹ��λ
		u32	EPIE_BIT 	:1;	//�����Ͽ��ж�ʹ��λ
		u32	ALIE_BIT 	:1;	//�ٲö�ʧ�ж�ʹ��λ
		u32	BEIE_BIT 	:1;	//���ߴ����ж�ʹ��λ
		u32	IDIE_BIT	 	:1;	//���յ�CAN��ʶ���ж�ʹ��λ
		u32	TIE2_BIT 		:1;	//TxBUF2��������ж�ʹ��λ
		u32	TIE3_BIT	 	:1;	//TxBUF3��������ж�ʹ��λ
		u32	RSV_BIT21	:21;	//����λ
	}Bits;
}uCANIER,*P_uCANIER;
//����ʱ��Ĵ������������Ͷ���
typedef	union		_canbtr_
{
	u32	Word;					//�ֲ�������
	struct	
	{
		u32	BRP_BIT 		:10;		//Ԥ��Ƶλ��϶���
		u32	RSV_BIT4		:4;		//����λ
		u32	SJW_BIT 		:2;		//ͬ����ת���
		u32	TSEG1_BIT 	:4;		//ʱ���1
		u32	TSEG2_BIT 	:3;		//ʱ���2
		u32	SAM_BIT 	:1;		//����ģʽλ
		u32	RSV_BIT8		:8;		//����
	}Bits;
}uCANBTR,*P_uCANBTR;
//��������޼Ĵ������������Ͷ���
typedef	union	_canewl_
{
	u32	Word;					//�ֲ�������
	struct	
	{
		u32	EWL_BIT 	:8;		///���������ֵ
		u32	RSV_BIT24	:24;		//����λ
	}Bits;
}uCANEWL,*P_uCANEWL;
//״̬�Ĵ������������Ͷ���
typedef	union	_cansr_
{
	u32	Word;					//�ֲ�������
	struct	
	{
		u32	RBS1_BIT 	:1;		//���ջ�������Ч
		u32	DOS1_BIT 	:1;		//�������
		u32	TBS1_BIT 	:1;		//TxBUF1����
		u32	TCS1_BIT 	:1;		//TxBUF1�����ݷ������
		u32	RS1_BIT 		:1;		//���ڽ���
		u32	TS1_BIT 		:1;		//TxBUF1���������ڷ���
		u32	ES1_BIT 		:1;		//�����Ͽ�
		u32	BS1_BIT 		:1;		//���ߴ���
		u32	RBS2_BIT 	:1;		//
		u32	DOS2_BIT 	:1;		//
		u32	TBS2_BIT 	:1;		// TxBUF2����
		u32	TCS2_BIT 	:1;		// TxBUF2�����ݷ������
		u32	RS2_BIT 		:1;		//
		u32	TS2_BIT 		:1;		// TxBUF2���������ڷ���
		u32	ES2_BIT 		:1;		//
		u32	BS2_BIT 		:1;		//
		u32	RBS3_BIT 	:1;		//
		u32	DOS3_BIT 	:1;		//
		u32	TBS3_BIT 	:1;		// TxBUF3����
		u32	TCS3_BIT 	:1;		// TxBUF3�����ݷ������
		u32	RS3_BIT 		:1;		//
		u32	TS3_BIT 		:1;		// TxBUF3���������ڷ���
		u32	ES3_BIT 		:1;		//
		u32	BS3_BIT 		:1;		//
		u32	RSV_BIT8		:8;		//
	}Bits;
}uCANSR,*P_uCANSR;
//CANRFS�Ĵ������������Ͷ���
typedef	union		_canrfs_
{
	u32		Word;				//�ֲ�������
	struct	
	{
		u32	IDIN_BIT 	:10;		//ID����ֵ
		u32	BP_BIT 		:1;		//BP
		u32	RSV_BIT5 	:5;		//����
		u32	DLC_BIT 		:4;		//���ݳ���
		u32	RSV_BIT10	:10;		//����
		u32	RTR_BIT 		:1;		//Զ��֡������֡ʶ��λ
		u32	FF_BIT 		:1;		//��չ֡����׼֡ʶ��λ
	}Bits;
}uCANRFS,*P_uCANRFS;
//CANID�Ĵ������������Ͷ���
typedef	union		_canid_
{
	u32		Word;					//�ֲ�������
	union	_frame_format
	{
		struct		_strFrameFm_				//��׼֡�ṹ
		{
			u32	ID_BIT 		:11;
			u32	RSV_BIT21	:21;
		}stcSTRFRFM;
		struct		_etxFrameFm_				//��չ֡�ṹ
		{
			u32	ID_BIT 		:29;
			u32	RSV_BIT3 	:3;
		}stcETXFRFM;
	}FrameFm;
}uCANID,*P_uCANID;
//���ݼĴ����������Ͷ���
typedef	union		_candf_
{
	u32		Word;					//�ֲ�������
	struct	
	{
		u32	Data1_BIT 	:8;			//�����ֽ�
		u32	Data2_BIT 	:8;	
		u32	Data3_BIT 	:8;	
		u32	Data4_BIT 	:8;	
	}Bits;
}uCANDF,*P_uCANDF;
//CANTFI�Ĵ������������Ͷ���
typedef	union		_cantfi_
{
	u32		Word;				//�ֽڲ�������
	struct	
	{
		u32	PRIO_BIT 	:8;		//���������ݷ������ȼ���ֵ
		u32	RSV_BIT8		:8;						
		u32	DLC_BIT 		:4;		//�������ݳ���
		u32	RSV_BIT10	:10;
		u32	RTR_BIT 		:1;		//Զ��֡������֡���λ
		u32	FF_BIT 		:1;		//��չ֡����׼֡���λ
	}Bits;
}uCANTFI,*P_uCANTFI;
//���巢��֡�ṹ
typedef	struct		_stcTXBUF_
{
	uCANTFI		TxFrameInfo;	//����֡��Ϣ�ṹ
	uCANID		TxCANID;		//����֡��ϢID
	uCANDF		CANTDA;		//��������1��4�ֽ�
	uCANDF		CANTDB;		//��������5��8�ֽ�
}*P_stcTxBUF,stcTxBUF;

//���弯�з���״̬�Ĵ����ṹ
typedef	union		_uCANTxSR_
{
	u32		Word;				//�ֲ�������
	struct	
	{
		u32	TS1_BIT 		:1;		//CAN1���ڷ���״̬
		u32	TS2_BIT 		:1;		//CAN2���ڷ���״̬
		u32	TS3_BIT 		:1;		//CAN3���ڷ���״̬
		u32	TS4_BIT 		:1;		//CAN4���ڷ���״̬
		u32	RSV_BIT1		:4;		//����
		u32	TBS1_BIT 		:1;		//CAN1���ͻ���������״̬
		u32	TBS2_BIT 		:1;		//CAN2���ͻ���������״̬
		u32	TBS3_BIT 	:1;		//CAN3���ͻ���������״̬
		u32	TBS4_BIT 	:1;		//CAN4���ͻ���������״̬
		u32	RSV_BIT2 	:4;		// 
		u32	TCS1_BIT 	:1;		//CAN1�������״̬
		u32	TCS2_BIT 	:1;		//CAN2�������״̬
		u32	TCS3_BIT 	:1;		//CAN3�������״̬
		u32	TCS4_BIT 	:1;		//CAN4�������״̬
		u32	RSV_BIT12	:12;		//����
	}Bits;
}*P_uCANTxSR,uCANTxSR;
//���弯�н���״̬�Ĵ����ṹ
typedef	union		_uCANRxSR_
{
	u32		Word;				//�ֲ�������
	struct	
	{
		u32	RS1_BIT 		:1;		//CAN1���ڽ���״̬
		u32	RS2_BIT 		:1;		//CAN2���ڽ���״̬
		u32	RS3_BIT 		:1;		//CAN3���ڽ���״̬
		u32	RS4_BIT 		:1;		//CAN4���ڽ���״̬
		u32	RSV_BIT1		:4;		//����
		u32	RBS1_BIT 	:1;		//CAN1���ջ�������Ч
		u32	RBS2_BIT 	:1;		//CAN2���ջ�������Ч
		u32	RBS3_BIT 	:1;		//CAN3���ջ�������Ч
		u32	RBS4_BIT 	:1;		//CAN4���ջ�������Ч
		u32	RSV_BIT2 	:4;		// 
		u32	DOS1_BIT 	:1;		//CAN1���ջ��������
		u32	DOS2_BIT 	:1;		//CAN2���ջ��������
		u32	DOS3_BIT 	:1;		//CAN3���ջ��������
		u32	DOS4_BIT 	:1;		//CAN4���ջ��������
		u32	RSV_BIT12	:12;		//����
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

//���弯������״̬�Ĵ����ṹ
typedef	union		_uCANMSR_
{
	u32		Word;				//�ֲ�������
	struct	
	{
		u32	ES1_BIT 		:1;		//CAN1���󱨾�
		u32	ES2_BIT 		:1;		//CAN2���󱨾�
		u32	ES3_BIT 		:1;		//CAN3���󱨾�
		u32	ES4_BIT 		:1;		//CAN4���󱨾�
		u32	RSV_BIT1		:4;		//����
		u32	BS1_BIT 		:1;		//CAN1��������
		u32	BS2_BIT 		:1;		//CAN2��������
		u32	BS3_BIT 		:1;		//CAN3��������
		u32	BS4_BIT 		:1;		//CAN4��������
		u32	RSV_BIT20	:20;		//����
	}Bits;
}*P_uCANMSR, uCANMSR;
//���������˲�ģʽ�Ĵ����ṹ
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
//�����˲�����񵥸���־�������ʼ��ַ���ݽṹ����
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
//�����˲�������־��������ʼ��ַ���ݽṹ����
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

//LUT����Ĵ������ݽṹ����
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
