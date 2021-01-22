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
#include "FormSmsAlarm.h"

static void Display(void);
static void Active(bool active);
static bool Event(u32 msg);
static void Create(u32 msg);

static const S_Form fmSmsAlarm = 
{
	SMS_ALARM_FORM_ID,			// 窗体ID，应当是全局唯一
	GUI_DELAY_COUNTER_S(30),		// 超时退出时间，0表示不超时退出
	Create,					// 窗体创建时调用
	NULL,					// 窗体退出时调用
	Active,					// 窗体激活事件，参数true表示激活，false表示进入非激活状态
	Event,					// 事件响应，如按键等以及其他用户自定义消息
	NULL/*Display*/					// 显示函数
};

const S_FormNode g_fmSmsAlarm = 
{
	(PS_Form)&fmSmsAlarm,	// 对应窗体
	(PS_FormNode)&g_fmMenu,		// 父菜单
	NULL,			// 子项
	NULL,			// 下一项
	NULL			// 上一项
};

// 编辑模式
static bool m_flgEdit;
// 配置信息
static bool m_aflgSetting[4];
static const char *C_aTitle[4] = {
	"水位不足",
	"污物箱满",
	"大便事件",
	"小便事件"
};
// 当前编辑的项
static u8 m_bIndex;


// 重绘
static void Display(void)
{
	char acBuf[20];
	int i;
	ClrScreen();
	LcmCenterString(0,"短信报警项设置");
	// 显示所有条目
	for (i =0;i<4;i++)
	{
		if (m_aflgSetting[i])
		{
			strcpy(acBuf,"√");
		}
		else
		{
			strcpy(acBuf,"□");
		}
		strcpy(acBuf+2,C_aTitle[i]);

		if (m_flgEdit&&(i==m_bIndex))
		{
			// 反色显示
			Rectangle(0,13*(i+1),127,13*(i+1)+11,1);
			FontMode(0,0);		// 不覆盖模式
			FontSet(0,0);
			FontSet_cn(0,0);
			LcmPrintString(0,13*(i+1),acBuf);
			FontMode(1,0);		// 覆盖模式，背景色为白色
			FontSet(0,1);
			FontSet_cn(0,1);
		}
		else
		{
			// 正常显示
			LcmPrintString(0,13*(i+1),acBuf);
		}
	}
}

static void Active(bool active)
{
	if (active)
	{
		m_aflgSetting[0] = g_sGlobalSetting.ToiletCfg.AlarmShuiWeiBuZuEn;
		m_aflgSetting[1] = g_sGlobalSetting.ToiletCfg.AlarmChuBianXiangManEn;
		m_aflgSetting[2] = g_sGlobalSetting.ToiletCfg.AlarmXiaoBianEn;
		m_aflgSetting[3] = g_sGlobalSetting.ToiletCfg.AlarmDaBianEn;
		Display();
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
		if (m_flgEdit&&(m_bIndex!=0))
		{
			m_bIndex--;
			Display();
		}
		return true;
	case KEY_DOWN:
		if (m_flgEdit&&(m_bIndex<3))
		{
			m_bIndex++;
			Display();
		}
		return true;
	case KEY_LEFT:
	case KEY_RIGHT:
		// 更改相应的设置项
		if (m_flgEdit)
		{
			m_aflgSetting[m_bIndex] = !m_aflgSetting[m_bIndex];
			Display();
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
			m_bIndex = 0;
			Display();
		}
		else
		{
			g_sGlobalSetting.ToiletCfg.AlarmShuiWeiBuZuEn = m_aflgSetting[0];
			g_sGlobalSetting.ToiletCfg.AlarmChuBianXiangManEn = m_aflgSetting[1];
			g_sGlobalSetting.ToiletCfg.AlarmXiaoBianEn = m_aflgSetting[2];
			g_sGlobalSetting.ToiletCfg.AlarmDaBianEn = m_aflgSetting[3];
			SaveGlobalSeeting();
			m_flgEdit = false;
			Active(true);
		}
		return true;
	default:
		return false;
//		break;
	}
}

