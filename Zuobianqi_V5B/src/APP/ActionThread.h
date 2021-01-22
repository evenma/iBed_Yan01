#ifndef _ACTION_THREAD_H_
#define _ACTION_THREAD_H_

#include <Common.h>

// 动作命令
typedef enum {
	ActionCmdNone = 0,		// 无动作，停止动作发送该位时
	ChongXiBianPeng,		// 1 冲洗便盆
	QingXiTunBu,			// 2 清洗臀部
	QingXiQianBu,			// 3 清洗前部
	GanZao,					// 4 干燥
	ZuoBianKaiShi,			// 5 座便开始
	ZuoBianJieShu,			// 6 座便结束
	STOP,					// 7 停止键	
	ChuangDianJiaReWork,	// 8 床垫加热开始
	ChuangDianJiaReEnd,		// 9 床垫加热结束
	RotatePADUp,		// 10 旋转液晶屏升
	RotatePADDown,	// 11 旋转液晶屏降
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
// 动作进程入口
extern void ActionThreadEntry(void* parameter);
// 开始动作命令
extern void ActionStartCmd(ActionCmd cmd);
// 停止动作命令
extern void ActionStopCmd(ActionCmd cmd);

extern void DoStop(void);
extern void WDInit(void);
extern u32 WDtimers(void);
extern void feedWD(void);



#endif


