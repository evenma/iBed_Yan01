#ifndef __KEY_H
#define __KEY_H

#include "../Common.h"
#include "../Gui/GuiCore.h"


//按键定义
typedef enum
{
	// 数字键
	KEY_UP = MSG_KEY,	 // 取消 设置键按下循环往下走		
	KEY_LEFT,	  // = + 键
	KEY_ENTER,    // = 设置键
	KEY_RIGHT,	  // = - 键
	KEY_DOWN,	  // 取消 设置键按下循环往下走
	KEY_SET,	  // = 确定键
	KEY_RETURN,   // 自动返回
	
	KEY_BEIBAN_UP,
	KEY_BEIBAN_DOWN,
	KEY_TUIBU_UP,
	KEY_TUIBU_DOWN,
	KEY_FANSHEN_ZUO_UP,   
	KEY_FANSHEN_ZUO_DOWN,
	KEY_FANSHEN_YOU_UP,   
	KEY_FANSHEN_YOU_DOWN,	
	KEY_BIANMEN_KAI,
	KEY_BIANMEN_GUAN,
	KEY_QIZUO,
	KEY_PINGTANG,
	KEY_XINZANGTANGWEI,
	
	KEY_CHONGXIBIANPEN,
	KEY_GANZAO,
	KEY_QINGXITUNBU,
	KEY_QINGXISHENGZHIQI,
	KEY_ZUOBIAN_KAISHI,
	KEY_ZUOBIAN_JIESHU,
	
	KEY_PAD_UP,
	KEY_PAD_DOWN,
		
	KEY_SET_SHUIWEN,
	KEY_SET_NUANFEN_SUB,
	KEY_SET_NUANFEN_ADD,

	KEY_LOCK,	// 键盘锁
	KEY_CANCEL,	// 停止键或者取消动作
	KEY_RELEASE,		// 按键松开
}E_KeyMessage;

extern s32 g_KeyMsgDown;	// 当前被按下的按键消息

extern void KeyInit(void);	// 按键初始化
extern void KeyTick(void);	// 按键扫描定时显示


#endif
