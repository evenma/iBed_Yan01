#ifndef _CAN_ANALYZER_H_
#define _CAN_ANALYZER_H_

typedef struct{
	u8 WuWuXiangGuan		: 1;		// �������·����״̬
	u8 DaBian				: 1;		// ���
	u8 ManXiaoBian			: 1;		// ��ʿС��
	u8 WomenXiaoBian		: 1;		// ŮʿС��
	u8 SuReQiWork           : 1;		// ������������־ 
	u8 ShuiXiangHigh		: 1;		// ˮ������־
	u8 ShuiXiangLow			: 1;		// ˮ��ͱ�־
	u8 rev					: 1;		// Ԥ��
	u16 WuWuXiangZhongLiang		;		// ����������
	u8 ChuFengWenDu				;		// ������¶�
	u8 ReShuiWenDu				;		// ��������ˮ�¶�
	u8 LengShuiWenDu			;       // ˮ����ˮ�¶�
	u8 FunctionWorkNuanFeng : 1	;		// ��������״̬ ů����
	u8 FunctionWorkQianBu	: 1	;		// ��������״̬ ǰ����ϴ
	u8 FunctionWorkHouBu	: 1	;		// ��������״̬ �󲿳�ϴ
	u8 FunctionWorkMaTong 	: 1	;		// ��������״̬ ��Ͱ��ϴ
	u8 FunctionWorkAuto 	: 1	;		// ��������״̬ ����ģʽ
	u8 rv					: 3;		// Ԥ��	
	u8 SuReQiError				;		// �������쳣����
}s_ZuobianSensor;

typedef struct{
	u8 BeiBuFangPing	 	: 1;		// ������ƽ
	u8 TuiBuFangPing		: 1;		// �Ȳ���ƽ
	u8 ZuoFanFangPing	 	: 1;		// �󷭷�ƽ
	u8 YouFanFangPing		: 1;		// �ҷ���ƽ
	u8 FangJia				: 1;		// ���м��
	u8 BianMenKaiDaoDing    : 1;		// ���ſ�����
	u8 BianMenGuanDaoDi    : 1;        // ���Źص���
	u8 BianMenPingYiDaoDing	: 1;		// ����ƽ�Ƶ���

	u8 BianMenPingYiDaoDi	: 1;		// ����ƽ�Ƶ���
	u8 DianLiangBuZu        : 1;		// ��������
	u8 DianJiGuoLiuBao		: 1;		// �����������
	u8 TingDian				: 1;		// ͣ��
	u8 DianChiZhuOn			: 1;		// �Ƿ��е����
	u8 rev                  : 3;		
	
	u8 QiBeiJiaoDu;
	u8 XiaQuTuiJiaoDu;
	u8 ShangQuTuiJiaoDu;
	u8 ZuoFanShenJiaoDu;
	u8 YouFanShenJiaoDu;
}s_ChuangTiSensor;

// ������״̬ȫ�ֱ���
extern volatile s_ChuangTiSensor g_ChuangTiSensor;
extern volatile s_ZuobianSensor g_ZuobianSensor;


#define WU_WU_XIANG_BORDER 8000	// �������ٽ�ֵ g
//#define WU_WU_XIANG_BU_ZAI_WEI 1000     // �����䲻��λ

// ���յ�CAN�ص�����
extern rt_err_t CanRxInd(rt_device_t dev, rt_size_t size);
extern void CanSetKeyCmd(u32 msg);
extern void LEDSetInit(void);
#endif



