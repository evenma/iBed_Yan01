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


static void Display(void);
static void Active(bool active);
static bool Event(u32 msg);
static void Create(u32 msg);

static const S_Form fmTemperature = 
{
	TEMPERATURE_FORM_ID,			// 窗体ID，应当是全局唯一
	GUI_DELAY_COUNTER_S(30),		// 超时退出时间，0表示不超时退出
	Create,					// 窗体创建时调用
	NULL,					// 窗体退出时调用
	Active,					// 窗体激活事件，参数true表示激活，false表示进入非激活状态
	Event,					// 事件响应，如按键等以及其他用户自定义消息
	Display					// 显示函数
};

const S_FormNode g_fmTemperature = 
{
	(PS_Form)&fmTemperature,	// 对应窗体
	(PS_FormNode)&g_fmMenu,		// 父菜单
	NULL,			// 子项
	NULL,			// 下一项
	NULL			// 上一项
};

// 编辑模式
static bool m_flgEdit;
// 编辑的水温
static u8 m_bTemperature;
// 当前编辑的位，0=个位，1=十位……
static u8 m_bIndex;

// 显示当前设置值
static void ShowSetting(bool flgShow)
{
	if (flgShow)
	{
		LcmPrintString(0,13*2,"设定温度：%d℃",m_bTemperature);
	}
	else
	{
		LcmClearRectangle(0,13*2,128,12);
	}
}

// 重绘
static void Display()
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
			if (m_bIndex)
			{
				PutChar(5*12+5,13*2,'0'+(m_bTemperature/10));
			}
			else
			{
				PutChar(5*12+12,13*2,'0'+(m_bTemperature%10));
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
		LcmCenterString(0,"水温设置");
		// 显示当前设置的温度
		LcmPrintString(0,13*1,"当前温度：%d℃",g_sGlobalSetting.ToiletCfg.ShuiWen);
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

#define MAX_TEMPERATURE	40
#define MIN_TEMPERATURE	30
static void SetTemperature(u8 temp)
{
	if (temp > MAX_TEMPERATURE)
	{
		m_bTemperature = MAX_TEMPERATURE;
	}
	else if (temp < MIN_TEMPERATURE)
	{
		m_bTemperature = MIN_TEMPERATURE;
	}
	else
	{
		m_bTemperature = temp;
	}
}

static bool Event(u32 msg)
{
	switch(msg)
	{
	case KEY_UP:
		if (m_flgEdit)
		{
			if (m_bIndex==0)
			{
				SetTemperature(m_bTemperature + 1);
			}
			else
			{
				SetTemperature(m_bTemperature + 10);
			}
			g_dwGuiCounter = 0;
			ShowSetting(true);
		}
		return true;
	case KEY_DOWN:
		if (m_flgEdit)
		{
			if (m_bIndex==0)
			{
				SetTemperature(m_bTemperature - 1);
			}
			else
			{
				SetTemperature(m_bTemperature - 10);
			}
			g_dwGuiCounter = 0;
			ShowSetting(true);
		}
		return true;
	case KEY_LEFT:
	case KEY_RIGHT:
		if (m_flgEdit)
		{
			m_bIndex = (m_bIndex+1)%2;
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
			SetTemperature(g_sGlobalSetting.ToiletCfg.ShuiWen);
			Display();
		}
		else
		{
			// 这里保存设置项到flash中
			if (g_sGlobalSetting.ToiletCfg.ShuiWen != m_bTemperature)
			{
				g_sGlobalSetting.ToiletCfg.ShuiWen = m_bTemperature;
				SaveGlobalSeeting();
			}
			m_flgEdit = false;
			Active(true);
		}
		return true;
#if 0
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		if (m_flgEdit)
		{
			if (m_bIndex)
			{
				SetTemperature((m_bTemperature%10)+10*(msg-KEY_0));
			}
			else
			{
				SetTemperature(10*(m_bTemperature/10)+(msg-KEY_0));
			}
			ShowSetting();
		}
		return true;
#endif
	default:
		return false;
		break;
	}
}

