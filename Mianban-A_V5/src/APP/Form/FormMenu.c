/********************************************************************
	主界面
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include "FormMain.h"
#include "Lcm/Lcm.h"
#include "GlobalSetting.h"
#include "FormInfo.h"
#include "FormId.h"
#include "Key/Key.h"
#include "FormMenu.h"
#include "FormMain.h"
#include "FormTemperature.h"
#include "FormSmsNo.h"
#include "FormTime.h"
#include "FormSmsAlarm.h"
#include "FormLog.h"

static void Display(void);
static void Active(bool active);
static bool Event(u32 msg);
static void Create(u32 msg);

static const S_Form fmMenu = 
{
	MENU_FORM_ID,			// 窗体ID，应当是全局唯一
	GUI_DELAY_COUNTER_S(30),		// 超时退出时间，0表示不超时退出
	Create,					// 窗体创建时调用
	NULL,					// 窗体退出时调用
	Active,					// 窗体激活事件，参数true表示激活，false表示进入非激活状态
	Event,					// 事件响应，如按键等以及其他用户自定义消息
	NULL	/*Display*/					// 显示函数
};

const S_FormNode g_fmMenu = 
{
	(PS_Form)&fmMenu,					// 对应窗体
	(PS_FormNode)&g_fmMain,		// 父菜单
	NULL,			// 子项
	NULL,			// 下一项
	NULL			// 上一项
};

// 菜单项以及对应的页面
typedef struct
{
	char* pIco;				// 图标
	char* pMsg;				// 提示文字
	PS_FormNode pForm;		// 对应的页面
}S_MenuItem;

const S_MenuItem m_asMenu[] = 
{
	{NULL,"系统状态",(PS_FormNode)&g_fmInfo},
//	{NULL,"记录查询",(PS_FormNode)&g_fmLog},
	{NULL,"短信号码设置",(PS_FormNode)&g_fmSmsNo},
	{NULL,"短信报警项设置",(PS_FormNode)&g_fmSmsAlarm},
	{NULL,"时间设置",(PS_FormNode)&g_fmTime},
	{NULL,"水温设置",(PS_FormNode)&g_fmTemperature},
};

static u8 m_bIndex;		// 菜单索引
#define MENU_LENGTH		sizeof(m_asMenu)/sizeof(S_MenuItem)
// 重绘
static void Display(void)
{
	u8 top;		// 顶部
	u8 bottom;	// 底部
	int i;
	if (MENU_LENGTH<=5)
	{
		bottom = MENU_LENGTH-1;
	}
	else
	{
		bottom = min(m_bIndex+2,MENU_LENGTH-1);
		bottom = max(bottom,4);
	}
	top = bottom>=5 ? bottom-4:0;
	DbgPrintf("index=%d,\ttop=%d\tbottom=%d\r\n",m_bIndex,top,bottom);
	ClrScreen();
	for (i=top;i<=bottom;i++)
	{
		if (i==m_bIndex)
		{
			// 反色显示
			Rectangle(0,13*(i-top),127,13*(i-top)+11,1);
			FontMode(0,0);		// 不覆盖模式
			FontSet(0,0);
			FontSet_cn(0,0);
			LcmPrintString(0,13*(i-top),m_asMenu[i].pMsg);
			FontMode(1,0);		// 覆盖模式，背景色为白色
			FontSet(0,1);
			FontSet_cn(0,1);
		}
		else
		{
			// 正常显示
			LcmPrintString(0,13*(i-top),m_asMenu[i].pMsg);
		}
	}
}

static void Active(bool active)
{
	if (active)
	{
		Display();
	}
}

static void Create(u32 msg)
{
//	m_bIndex = 0;
}

static bool Event(u32 msg)
{
	switch(msg)
	{
	case KEY_UP:
		if (m_bIndex!=0)
		{
			m_bIndex--;
			Display();
		}
		return true;
	case KEY_DOWN:
		if (m_bIndex<MENU_LENGTH-1)
		{
			m_bIndex++;
			Display();
		}
		return true;
	case KEY_ENTER:
		GuiNav(m_asMenu[m_bIndex].pForm);
		return true;
	default:
		return false;
		break;
	}
}

