#ifndef _ACTION_THREAD_H_
#define _ACTION_THREAD_H_

#include <Common.h>

// ��������
typedef enum {
	ActionCmdNone = 0,		// �޶�����ֹͣ�������͸�λʱ
	ChongXiBianPeng,		// 1 ��ϴ����
	QingXiTunBu,			// 2 ��ϴ�β�
	QingXiQianBu,			// 3 ��ϴǰ��
	GanZao,					// 4 ����
	ZuoBianKaiShi,			// 5 ���㿪ʼ
	ZuoBianJieShu,			// 6 �������
	STOP,					// 7 ֹͣ��	
	ChuangDianJiaReWork,	// 8 ������ȿ�ʼ
	ChuangDianJiaReEnd,		// 9 ������Ƚ���
	RotatePADUp,		// 10 ��תҺ������
	RotatePADDown,	// 11 ��תҺ������
}ActionCmd;

extern void ActionSetBianPenZiDongPaiWuYanShi(u16 val);
extern void ActionSetBianPenZiDongPauWuJianCeYanShi(u16 val);
extern void ActionSetBianPenShouDongQingJieShiJian(u16 val);
extern void ActionSetHongGanShiJian(u16 val);
extern void ActionSetTunBuQingXiShiJian(u16 val);
extern void ActionSetQianBuQingXiShiJian(u16 val);
extern void ActionSetZuoBianQuanQingXiShiJian(u16 val);

extern void ActionMianBanASetShuiWen(u8 val);
extern void ActionMianBanBSetShuiWen(u8 val);
extern void ActionMianBanASetNuanFengWenDu(u8 val);
extern void ActionMianBanBSetNuanFengWenDu(u8 val);
extern void ActionMianBanASetShuiLiuQiangDu(u8 val);
extern void ActionMianBanBSetShuiLiuQiangDu(u8 val);
extern void ActionMianBanBSetZhiNengModel(u8 val);
extern void ActionMianBanBSetChuangDianWenDu(u8 val);
extern void ActionMianBanBSetSwitchControl(void);
// �����������
extern void ActionThreadEntry(void* parameter);
// ��ʼ��������
extern void ActionStartCmd(ActionCmd cmd);
// ֹͣ��������
extern void ActionStopCmd(ActionCmd cmd);

extern void DoStop(void);
extern void WDInit(void);
extern u32 WDtimers(void);
extern void feedWD(void);



#endif


