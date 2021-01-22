#ifndef __GLOBAL_SETTING_H
#define __GLOBAL_SETTING_H

#define  MSG_NUMBER_MAX_LENGTH	15

typedef struct{
	u8 HoldOn;					// ����
	u8 ShuiWenGaoDi ;			// ˮ�¸ߵ�
	u8 NuanFengQiangRuo ;		// ů���¶ȸߵ�
	u8 QiangXiQiangRuo;         // ��ϴǿ��
}S_ZuobianqiSetA;

typedef struct{
	u8 HoldOn;					// ����
	u8 ShuiWen;					// ˮ�����ã�30-41��  *1��  0��Ϊ������
	u8 NuanFeng;				// ů���¶����ã�40-65��     0��Ϊ������
	u8 QiangXiQiangRuo;         // ��ϴǿ��  1,2,3������λ
	u8 ZhiNengModel;			// ����Ĭ��
	u8 ZhiWuRenModel;			// ֲ����ģʽ
}S_ZuobianqiSetB;

typedef struct{
	u16 BianPenZiDongPaiWuYanShi;			// �����Զ�����ʱ�䣬��λ�룬0-60000��Ч
	u16 BianPenZiDongPauWuJianCeYanShi;		// �����Զ����ۼ����ʱ����λ�룬0-60000��Ч
	u16 BianPenShouDongQingJieShiJian;		// �����ֶ����ʱ�䣬��λ�룬0-60000��Ч
	u16 HongGanShiJian;						// ���ʱ�䣬��λ�룬��Χ0-60000��Ч
	u16 TunBuQingXiShiJian;					// �β���ϴʱ�䣬��λ�룬��Χ0-60000��Ч
	u8  WenShuiPenLinJiaoDu;				// ��ˮ���ܽǶȣ���λ��
	u8  ShuiWen;							// ����ˮ�£�60-80��Ч�������¶�=ֵ*0.5��
	u8  AlarmEn : 1;						// ����ʹ��
	u8  AlarmShuiWeiBuZuEn : 1;			// ˮλ���㱨��ʹ��
	u8  AlarmChuBianXiangManEn : 1;		// ������������ʹ��
	u8  AlarmDaBianEn :1;					// ��㱨��ʹ��
	u8  AlarmXiaoBianEn : 1;				// С�㱨��ʹ��
	u8  rev :3;						// ����
	u8 ShangBiYanShi;						// �ϱڳ�ϴ��ʱ
	u8 BianBiYanShi;						// �߱���ʱ
}t_ToiletActionConfig;

typedef struct
{
	t_ToiletActionConfig ToiletCfg;			// ����������
	char acMsgNumber[MSG_NUMBER_MAX_LENGTH+1];			// ���ź��룬��1����Ϊ���һλ��Ҫ��0

}S_GlobalSetting;

// ��С���¼
typedef struct
{
	u16 wYear;			// ��
	u8 bMonth;		// ��
	u8 bDay;			// ��
	u8 bHour;			// ʱ
	u8 bMinute;		// ��
	u8 bSecond;		// ��
	u8 bEvent;		// 1=��㣬0=С��
}S_DaXiaoBianJiLu;

extern S_ZuobianqiSetB g_ZuobianqiSetB;	// ȫ�ֱ���
extern S_ZuobianqiSetA g_ZuobianqiSetA;	// ȫ�ֱ���
extern S_GlobalSetting g_sGlobalSetting;	// ȫ�ֱ���

// �� EEPROM ������ȫ�ֱ���
extern void LoadGlobalSetting(void);
// ��ȫ�ֱ�������� EEPROM ��
extern void SaveGlobalSeeting(void);
// ��С���¼
//	���� bDaXiaoBian 1=��㣬0=С��
extern void DaXiaoBianJiLu(u8 bDaXiaoBian);
// ��ȡ��¼����
extern u32 GetJiLuZongShu(void);
// ��ȡ��һ����¼��ַ
extern S_DaXiaoBianJiLu* GetFirstJiLu(void);
// ��ȡ���һ����¼��ַ
extern S_DaXiaoBianJiLu* GetLastJiLu(void);
extern S_DaXiaoBianJiLu* GetNextJiLu(S_DaXiaoBianJiLu* current);
// ��ȡ��һ����¼
extern S_DaXiaoBianJiLu* GetPreviousJiLu(S_DaXiaoBianJiLu* current);

extern void SetDefault(void);


#endif

