#ifndef _SENSOR_H_
#define	_SENSOR_H_

#include <Common.h>

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
	u8 FunctionWorkZuoYi    : 1;		// ��������״̬ ����
	u8 FunctionWorkTangYi   : 1;		// ��������״̬ ����	
	u8 ChongDian            : 1;		// ���״̬
	
	u8 QiBeiJiaoDu;
	u8 XiaQuTuiJiaoDu;
	u8 ShangQuTuiJiaoDu;
	u8 ZuoFanShenJiaoDu;
	u8 YouFanShenJiaoDu;
	u8 DianChiDianYa;					// 	��ص�ѹ
}s_Sensor;

typedef struct{
	u32 BeiBuDianLiu;				// ��������ֵ
	u32 TuiBuDianLiu;				// �Ȳ�����ֵ
	u32 ZuoFanDianLiu;				// �������ֵ
	u32 YouFanDianLiu;				// �ҷ������ֵ
	u32 HuaBeiDianLiu;				// ��������ֵ
	u32 BianMenShengJiangDianLiu;	// ������������ֵ
	u32 BianMenPingYiDianLiu;		// ����ƽ�Ƶ���ֵ
	u32 ZuoBianQiShengJiangDianLiu;	// ��������������ֵ
	u32 DianChiDianLiang;			// ��ص���
}ADS_Sensor;

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
	
}s_MT_work;	// ���е��״̬0:���ͣ; 1:�����;2:�������


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
	u8 rv					: 3;		// Ԥ��	
	u8 SuReQiError				;		// �������쳣����
}s_ZuobianqiSensor;

// ������״̬ȫ�ֱ���
extern volatile s_Sensor g_sensor;		
extern volatile ADS_Sensor g_AD_sensor;
extern volatile s_MT_work g_MT_work;
extern volatile s_ZuobianqiSensor g_ZuobianqiSensor;


// ����������Ϣ �Ƹ�������
//#define	BEI_MAPAN_MAX	7850//7350//7445//7500 //7525  // 8000  ʵ������      7.2mm.s
//#define	BEI_MAPAN_MIDDLE	105*BEI_MAPAN_MAX/265   //250   //3160 // 4000 
//#define	TUI_MAPAN_MAX	5950//5700//4450//4500 //3826	//4600    5540   1112  4.2mm/s   40����/mm
//#define	TUI_MAPAN_SHUI_PING	(u32)122*TUI_MAPAN_MAX/150	//3111	3434    5986
//#define	FANSHEN_MAPAN_MAX	2950//3220
#define	BEI_MAPAN_MAX	 3850//3952		//1976*2  130mm 3845 3849 3846 3844 3850 3855 3855
#define	BEI_MAPAN_MIDDLE	BEI_MAPAN_MAX/2   
#define	TUI_MAPAN_MAX	4826//5429	// 2736*2 180mm       5417 5429 5429    // �Ȳ��Ƹ˱�̧��20mm ����������603
#define	TUI_MAPAN_SHUI_PING	3921//4524   //2280*2 150mm     4509   -100
#define	FANSHEN_MAPAN_MAX	2950//3220

#define WUCHAJINGDU 50    // ��Χ100mS��������+ ���ͣ����
// ������ת�Ƕ�
#define BEI_JIAODU_MAX 	72 //
#define TUI_XIA_JIAODU_MAX 66//55
#define TUI_SHANG_JIAODU_MAX 14
#define FANSHEN_JIAODU_MAX 50



// ��������ʼ��
extern void SensorInit(void);
extern void __irq timer1_interrupt(void);

extern u16 m_beibu_max ;
extern u16 m_beibu_middle ;
extern u16 m_tuibu_max ;
extern u16 m_tuibu_middle ;
extern u16 m_fanshen_max ;
extern u16 m_wucha ;
extern u16 beibumapancount;
extern u16 tuibumapancount; 

extern u8 m_TuiBuStatus;
extern u8 c_TuiBuStatus;

extern u8 m_bei_hall;
extern u16 beibumapancount;
extern u8 m_tui_hall;
extern u16 tuibumapancount;
extern u8 m_zuofan_hall;
extern u16 zuofanmapancount;
extern u8 m_youfan_hall;
extern u16 youfanmapancount;
extern u32 g_diancidianliangAd;
#endif

