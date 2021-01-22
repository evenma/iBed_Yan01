#ifndef _ACTION_THREAD_H_
#define _ACTION_THREAD_H_

#include <Common.h>

// 动作命令
typedef enum {
	ActionCmdNone = 0,		// 无动作，停止动作发送该位时
	BeiSheng,		// 背升
	BeiJiang,		// 背降
	TuiSheng,		// 腿升
	TuiJiang,		// 腿降
	QiZuo,			// 座椅
	PingTang,		// 平躺
	XinZangTangWei,	// 心脏躺位
	BianMenQuanKai, // 便门开
	BianMenQuanGuan,// 便门关
	ZuoFanSheng,			// 左翻升
	ZuoFanJiang,			// 左翻降
	YouFanSheng,			// 右翻升
	YouFanJiang,            // 右翻降
	ZuoBian, 				// 座便
	FuYuan,					// 复原
	STOP,					// 停止键
}ActionCmd;

// 通过CAN发送的动作
extern u8 g_actionSend[8] ;

// 动作进程入口
extern void ActionThreadEntry(void* parameter);
// 开始动作命令
extern void ActionStartCmd(ActionCmd cmd);
// 停止动作命令
extern void ActionStopCmd(ActionCmd cmd);

extern void BeiBu(u8 work,u8 dir);
extern void TuiBei(u8 work,u8 dir);

extern void DoStop(void);

extern void DoAllStop(void);

extern void WDInit(void);
extern u32 WDtimers(void);
void feedWD(void);
static int WaitTimeout(rt_int32_t timeout,rt_uint32_t set);
static void RestartTimer(void);

#endif


