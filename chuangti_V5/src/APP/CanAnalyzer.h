#ifndef _CAN_ANALYZER_H_
#define _CAN_ANALYZER_H_

// CANAnalyzer ��ʼ��
extern void CanAnalyzerInit(void);

typedef struct{
	u8 PingTang : 1;		// ƽ��
	u8 ZuoYi 	: 1;		// ����
	u8 TangYi   : 1;        // ����
	u8 ZuoBian  : 1;        // ����
	u8 FuYuan   : 1;		// ��ԭ
	u8 BianMenKai:1;		// ���ſ�
	u8 BianMenGuan:1;		// ���Ź�
	u8 Anything		:1;		// �������ͣ
}s_FlagCmd;

extern s_FlagCmd g_FlagCmd;	
//extern u16 YanShiCount;		// ����������ʱ������

#endif



