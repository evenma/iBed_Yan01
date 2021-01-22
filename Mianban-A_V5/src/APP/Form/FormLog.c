/********************************************************************
	主界面
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include "FormMain.h"
#include "../Lcm/Lcm.h"
#include "../GlobalSetting.h"
#include "../Sensor/Sensor.h"
#include "FormInfo.h"
#include "FormId.h"
#include "../Key/Key.h"
#include "FormMenu.h"
#include "FormMain.h"
#include "FormTemperature.h"
#include "FormSmsNo.h"
#include "FormTime.h"
#include "FormLog.h"
#include "../GlobalSetting.h"

static void Display();
static void Active(bool active);
static bool Event(DWORD msg);

static const S_Form fmLog = 
{
	LOG_FORM_ID,			// 窗体ID，应当是全局唯一
	GUI_DELAY_COUNTER_S(30),	// 超时退出时间，0表示不超时退出
	NULL,					// 窗体创建时调用
	NULL,					// 窗体退出时调用
	Active,					// 窗体激活事件，参数true表示激活，false表示进入非激活状态
	Event,					// 事件响应，如按键等以及其他用户自定义消息
	NULL	/*Display*/					// 显示函数
};

const S_FormNode g_fmLog = 
{
	(PS_Form)&fmLog,					// 对应窗体
	(PS_FormNode)&g_fmMenu,		// 父菜单
	NULL,			// 子项
	NULL,			// 下一项
	NULL			// 上一项
};

static S_DaXiaoBianJiLu* m_psCurrentJiLu;		// 当前记录指针

// 重绘
static void Display()
{
	DbgPrintf("记录地址:%x\r\n",m_psCurrentJiLu);

	ClrScreen();
	LcmCenterString(0,"记录查询");
	// 显示4条记录
	S_DaXiaoBianJiLu *jilu = m_psCurrentJiLu;
	for (int i=0;i<4;i++)
	{
		if (NULL == jilu)
		{
			return;
		}
		char buf[20];
		sprintf(buf,"%04d-%02d-%02d %02d:%02d%s",
			jilu->wYear,jilu->bMonth,jilu->bDay,jilu->bHour,jilu->bMinute,jilu->bEvent?"大":"小");
		LcmPrintString(0,13*i+13,buf);
		jilu = GetPreviousJiLu(jilu);
	}
}

static void Active(bool active)
{
	if (active)
	{
		DbgPrintf("记录总数:%d\r\n",GetJiLuZongShu());
		m_psCurrentJiLu = GetLastJiLu();
		Display();
	}
}

static bool Event(DWORD msg)
{
	switch(msg)
	{
	case KEY_UP:	// 跳到第一页
		m_psCurrentJiLu = GetFirstJiLu();
		for (int i=0;i<3;i++)	// 往前后3条
		{
			S_DaXiaoBianJiLu* jilu = GetNextJiLu(m_psCurrentJiLu);
			if (NULL == jilu)
			{
				break;
			}
			else
			{
				m_psCurrentJiLu = jilu;
			}
		}
		Display();
		return true;
	case KEY_DOWN:	// 跳到最后一页
		Active(true);
		return true;
	case KEY_ENTER:	// 跳出删除界面（暂不提供删除功能）
		return true;
	case KEY_LEFT:	// 上一页
		if (m_psCurrentJiLu==NULL)
		{
			Active(true);
		}
		else
		{
			S_DaXiaoBianJiLu* jilu = m_psCurrentJiLu;
			for (int i=0;i<4;i++)	// 往前4条
			{
				jilu = GetPreviousJiLu(jilu);
				if (NULL == jilu)	// 已经到顶了则不继续翻页
				{
					return true;
				}
			}
			m_psCurrentJiLu = jilu;
			Display();
		}
		return true;
	case KEY_RIGHT:	// 下一页
		if (m_psCurrentJiLu==NULL)
		{
			Active(true);
		}
		else
		{
			for (int i=0;i<4;i++)	// 往后4条
			{
				S_DaXiaoBianJiLu* jilu = GetNextJiLu(m_psCurrentJiLu);
				if (NULL == jilu)
				{
					break;
				}
				else
				{
					m_psCurrentJiLu = jilu;
				}
			}
			Display();
		}
		return true;
	default:
		return false;
		break;
	}
}

