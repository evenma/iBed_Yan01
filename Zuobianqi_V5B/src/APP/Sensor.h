#ifndef _SENSOR_H_
#define	_SENSOR_H_

#include <Common.h>
#include "Pwm.h"
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

	//	u8 ChuFengWenDu				;		// ������¶�  �ṹ��������
	// 	u8 ChuanDianWenDu			;		//  �����¶� �������ͻ�����治�ܼ���
	u8 rotateDiningCnt;				// ������ת���
	
	u8 ReShuiWenDu				;		// ��������ˮ�¶�
	
	u8 LengShuiWenDu			;       // ˮ����ˮ�¶�
	
	u8 FunctionWorkNuanFeng : 1	;		// ��������״̬ ů����
	u8 FunctionWorkQianBu	: 1	;		// ��������״̬ ǰ����ϴ
	u8 FunctionWorkHouBu	: 1	;		// ��������״̬ �󲿳�ϴ
	u8 FunctionWorkMaTong 	: 1	;		// ��������״̬ ��Ͱ��ϴ
	u8 FunctionWorkAuto 	: 1	;		// ��������״̬ ����ģʽ
	u8 MaTongFull			: 1 ;		// ��Ͱ��Һλ�� 
	u8 rv					: 2 ;		// Ԥ��	

	u8 SuReQiError				;		// �������쳣����
}s_Sensor;

typedef struct{
	u8 BeiBuFangPing	 	: 1;		// ������ƽ
	u8 TuiBuFangPing		: 1;		// �Ȳ���ƽ
	u8 ZuoFanFangPing	 	: 1;		// �󷭷�ƽ
	u8 YouFanFangPing		: 1;		// �ҷ���ƽ
	u8 FangJia				: 1;		// ���м��
	u8 BianMenKaiDaoDing    : 1;		// ���ſ�����
	u8 BianMenGuanDaoDi     : 1;        // ���Źص���
	u8 BianMenPingYiDaoDing	: 1;		// ����ƽ�Ƶ���

	u8 BianMenPingYiDaoDi	: 1;		// ����ƽ�Ƶ���
	u8 DianLiangBuZu        : 1;		// ��������
	u8 DianJiGuoLiuBao		: 1;		// �����������
	u8 TingDian				: 1;		// ͣ��
	u8 DianChiZhuOn			: 1;		// �Ƿ��е����
	u8 FunctionWorkZuoYi    : 1;		// ��������״̬ ����
	u8 FunctionWorkTangYi   : 1;		// ��������״̬ ����	
	u8 ChongDian            : 1;		// ���״̬
	
	u8 QiBeiJiaoDu;
	u8 XiaQuTuiJiaoDu;
	u8 ShangQuTuiJiaoDu;
	u8 ZuoFanShenJiaoDu;
	u8 YouFanShenJiaoDu;
	u8 DianChiDianYa;					// 	��ص�ѹ
}s_BedSensor;

typedef struct{
	u8 beibu 				: 2;		// �������״̬
	u8 tuibu			 	: 2;		// �Ȳ����״̬
	u8 zuofanshen			: 2;		// ������״̬
	u8 youfanshen			: 2;		// �ҷ�����״̬
	
	u8 huabei				: 2;		// �������״̬
	u8 bianmenshengjiang 	: 2;		// �����������״̬
	u8 bianmenpingyi	 	: 2;		// ����ƽ�Ƶ��״̬
	u8 zuobianqishengjiang	: 2;		// �������������״̬

	u16 DianChiDianLiang;			// ��ص���
	
	u8 leftG 				: 2;		// �������״̬
	u8 rightG			 	: 2;		// �һ������״̬	
	u8 dining				: 2;		// �������״̬
	u8 rv					: 2;		// Ԥ��		
	
	u8 diningCnt;         // �����Ƹ��г�
}s_BedSensor_MT_work;	// ���е��״̬0:���ͣ; 1:�����;2:�������

// ������״̬ȫ�ֱ���
extern volatile s_Sensor g_sensor;
// ������״̬ȫ�ֱ���
extern volatile s_BedSensor g_bedSensor;		
extern volatile s_BedSensor_MT_work g_bedsensor_MT_work;

// ��������ʼ��
extern void SensorInit(void);

typedef struct{
	u8 HoldOn;								// ����
	u8  ShuiWen;							// ����ˮ�£�30-41��Ч�������¶�=ֵ*1��
	u8  NuanFen;							// ����ů���¶ȣ�40-65��Ч������0��Ч�������¶�=ֵ*1��
	u8  ShuiLiu;							// ����ˮ��ǿ��
}s_SetAConfig;

typedef struct{
	u8 HoldOn;								// ����
	u8  ShuiWen;							// ����ˮ�£�30-41��Ч�������¶�=ֵ*1��
	u8  NuanFen;							// ����ů���¶ȣ�40-65��Ч������0��Ч�������¶�=ֵ*1��
	u8  ShuiLiu;							// ����ˮ��ǿ��
//	u8  ZhiNengmodel;						// ����ģʽ
//	u8 ZhiWuRenModel;						// ֲ����ģʽ	
//	u8 ChuangDianPower;						// ������ȿ���
	u8 ChuangDianJiaRe;						// �������   25-50��Ч�������¶�=ֵ*1��
	u8 ZhiNengModel			: 1;			// ����ģʽ
	u8 ZhiWuRenModel		: 1;			// ֲ����ģʽ
	u8 ChuangDianPower	: 1;			// ������ȿ���
	u8 NightLampSwitch  : 1;			// Сҹ�ƿ���
	u8 ToiletLampSwitch : 1;			// ��Ͱ�ڿ��ӵƿ���
	u8 rev					: 3;		// Ԥ��
}s_SetBConfig;


typedef struct{	
	u16 BianPenZiDongPaiWuYanShi;			// �����Զ�����ʱ�䣬��λ�룬0-60000��Ч
	u16 BianPenZiDongPauWuJianCeYanShi;		// �����Զ����ۼ����ʱ����λ�룬0-60000��Ч
	u16 BianPenShouDongQingJieShiJian;		// �����ֶ����ʱ�䣬��λ�룬0-60000��Ч
	u16 HongGanShiJian;						// ���ʱ�䣬��λ�룬��Χ0-60000��Ч

	u16 TunBuQingXiShiJian;					// �β���ϴʱ�䣬��λ�룬��Χ0-60000��Ч
	u16 QianBuYanShi;						// ǰ����ϴ��ʱ
	u16 MaTongQuanYanShi;					// ��ͰȦ��ϴ��ʱ
}t_ActionConfig;


extern volatile s_SetAConfig  Set_A_Config;
extern volatile s_SetBConfig  Set_B_Config;

extern volatile t_ActionConfig m_ActionCfg ;

extern u16 m_XSBDuty;
// ��ˮ��ˮˮ��
#define RE_SHUI_FANG_SHUI_FA(on)	if(on) {IO0SET = BIT15;} else {IO0CLR = BIT15;}
// ��ˮСˮ�� ȫ�� 1800cc/min =1.8L/MIN =30mL/S
#define XIAO_SHUI_BENG(on)	if(on) {	ZENG_YA_BENG_ZHU_SHUI_FA(1);PwmChange(2, m_XSBDuty);} else {PwmChange(2,0);	ZENG_YA_BENG_ZHU_SHUI_FA(0);}


// sensor.c �� actionthread.c����
// ��ϴ����Ȧˮ��
//#define CHONG_XI_SHUI_FA_ZUOBIANQUAN(on)	if(on) {IO0SET = BIT13;} else {IO0CLR = BIT13;}
#define CHONG_XI_SHUI_FA_ZUOBIANQUAN(on)	if(on) {IO0CLR = BIT13;} else {IO0CLR = BIT13;}
// ��ˮ��ϴ��ѹ��  20L/MIN	=0.33L/S  ʵ�� 0.138 L/S
#define QING_SHUI_CHONG_XI_ZENG_YA_BENG(on)	if(on) {IO0SET = BIT6;} else {IO0CLR = BIT6;}
// ���۱ã�����ã���ˮ�� ˮ��45L/MIN  = 0.75L/S
#define PAI_WU_SUI_WU_BENG(on)  	if(on) {IO0SET = BIT21;} else {IO0CLR = BIT21;}					
#define WU_WU_XIANG_MAN		8000	// �������������� g
// עˮˮ��
#define ZHU_SHUI_SHUI_BENG(on)	if(on) {IO0SET = BIT11;} else {IO0CLR = BIT11;}
// ��ѹ��עˮ��
#define ZENG_YA_BENG_ZHU_SHUI_FA(on)	if(on) {IO1SET = BIT30;} else {IO1CLR = BIT30;}
// ��Ͱ�ڿ��ӵƿ���
#define TOILET_LAMP_WORK(on) if(on) {IO0SET = BIT22;} else {IO0CLR = BIT22;}

#define TIME_ROTATE_DINING	6			// 6��
#endif

