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
#include "FormTemperature.h"
#include "GlobalSetting.h"
#include "FormSmsNo.h"

static void Display(void);
static void Active(bool active);
static bool Event(u32 msg);
static void Create(u32 msg);

static const S_Form fmSmsNo = 
{
	SMS_NO_FORM_ID,			// 窗体ID，应当是全局唯一
	GUI_DELAY_COUNTER_S(30),		// 超时退出时间，0表示不超时退出
	Create,					// 窗体创建时调用
	NULL,					// 窗体退出时调用
	Active,					// 窗体激活事件，参数true表示激活，false表示进入非激活状态
	Event,					// 事件响应，如按键等以及其他用户自定义消息
	Display					// 显示函数
};

const S_FormNode g_fmSmsNo = 
{
	(PS_Form)&fmSmsNo,	// 对应窗体
	(PS_FormNode)&g_fmMenu,		// 父菜单
	NULL,			// 子项
	NULL,			// 下一项
	NULL			// 上一项
};

// 编辑模式
static bool m_flgEdit;
// 编辑的号码
static char m_acNo[MSG_NUMBER_MAX_LENGTH+1];
// 当前编辑的位，0=第一位，1=第二位，……
static u8 m_bIndex;

// 显示当前设置值
static void ShowSetting(bool flgShow)
{
	if (flgShow)
	{
		LcmPrintString(0,13*3,"设定号码：");
		LcmClearRectangle(12,13*4,128-12,10);
		LcmPrintString(12,13*4,m_acNo);
		if (strlen(m_acNo)==m_bIndex)
		{
			PutChar(12+7*m_bIndex,13*4,'_');
		}
	}
	else
	{
		LcmClearRectangle(0,13*3,128,25);
	}
}

// 重绘
static void Display(void)
{
	if (m_flgEdit)
	{
		// 编辑模式需要闪烁
		u32 dwFlash = g_dwGuiCounter%GUI_DELAY_COUNTER_S(1);
		if (dwFlash==0)
		{
			ShowSetting(true);
		}
		else if (dwFlash == (500/GUI_TICK_CYCLE))
		{
			ShowSetting(true);
			// 反色显示
			FontMode(1,1);		// 覆盖模式，背景为黑色
			FontSet(0,0);		// 前景色为白色
			if (m_acNo[m_bIndex])
			{
				PutChar(12+7*m_bIndex,13*4,m_acNo[m_bIndex]);
			}
			else
			{
				PutChar(12+7*m_bIndex,13*4,'_');
			}
			FontMode(1,0);		// 覆盖模式，背景色为白色
			FontSet(0,1);
		}
	}
}

static void Active(bool active)
{
	if (active)
	{
		ClrScreen();
		g_dwGuiCounter = 0;
		LcmCenterString(0,"短信号码设置");
		// 显示当前设置的温度
		LcmPrintString(0,13*1,"当前号码：");
		LcmPrintString(12,13*2,g_sGlobalSetting.acMsgNumber);
		if (m_flgEdit)
		{
			ShowSetting(true);
		}
	}
}

static void Create(u32 msg)
{
	m_flgEdit = false;
	m_bIndex = 0;
}

static bool Event(u32 msg)
{
	switch(msg)
	{
	case KEY_UP:
		if (m_flgEdit)
		{
			if(m_acNo[m_bIndex]=='9')
			{
				m_acNo[m_bIndex]=0;
			}
			else if(m_acNo[m_bIndex]==0)
			{
				m_acNo[m_bIndex]='0';
			}
			else 
			{
				m_acNo[m_bIndex]++;
			}
			g_dwGuiCounter = 0;
			Display();
		}
		return true;
	case KEY_DOWN:
		if (m_flgEdit)
		{
			if(m_acNo[m_bIndex]=='0')
			{
				m_acNo[m_bIndex]=0;
			}
			else if(m_acNo[m_bIndex]==0)
			{
				m_acNo[m_bIndex]='9';
			}
			else 
			{
				m_acNo[m_bIndex]--;
			}
			g_dwGuiCounter = 0;
			Display();
		}
		return true;
	case KEY_LEFT:
		if (m_flgEdit&&(m_bIndex!=0))
		{
			m_bIndex--;
			g_dwGuiCounter = 0;
			Display();
		}
		return true;
	case KEY_RIGHT:
		if (m_flgEdit)
		{
			u32 len = strlen(m_acNo);
			len = min(len,MSG_NUMBER_MAX_LENGTH-1);		// len从1开始，index从0开始，所以需要减1
			if (m_bIndex<len)
			{
				m_bIndex++;
				g_dwGuiCounter = 0;
				Display();
			}
		}
		return true;
	case KEY_RETURN:
		if (m_flgEdit)
		{
			m_flgEdit = false;
			Active(true);
			return true;
		}
		else
		{
			return false;
		}
	case KEY_ENTER:
		if (!m_flgEdit)
		{
			m_flgEdit = true;
			g_dwGuiCounter = 0;
			m_bIndex = 0;
//			memcpy(m_acNo,g_sGlobalSetting.acMsgNumber,sizeof(g_sGlobalSetting.acMsgNumber));
			memset(m_acNo,0,sizeof(m_acNo));
			Display();
		}
		else
		{
			// 这里保存设置项到flash中
			if (0!=strcmp(m_acNo,g_sGlobalSetting.acMsgNumber))
			{
				memcpy(g_sGlobalSetting.acMsgNumber,m_acNo,sizeof(g_sGlobalSetting.acMsgNumber));
				SaveGlobalSeeting();
			}
			m_flgEdit = false;
			Active(true);
		}
		return true;
	default:
		return false;
//		break;
	}
}

