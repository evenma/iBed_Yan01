/********************************************************************
	信息界面
*********************************************************************/
#include <stdio.h>
#include "FormMain.h"
#include "Lcm/Lcm.h"
#include "GlobalSetting.h"
#include "FormMain.h"
#include "FormInfo.h"
#include "FormId.h"
#include "FormMenu.h"
#include "Key/Key.h"
#include "CanAnalyzer.h"

static void Display(void);
static void Active(bool active);
static bool FormEvent(u32 msg);

#define ROW_HEIGH	13	// 行高

static const S_Form fmInfo = 
{
	INFO_FORM_ID,			// 窗体ID，应当是全局唯一
	GUI_DELAY_COUNTER_S(30),		// 超时退出时间，0表示不超时退出
	NULL,					// 窗体创建时调用
	NULL,					// 窗体退出时调用
	Active,					// 窗体激活事件，参数true表示激活，false表示进入非激活状态
	FormEvent,				// 事件响应，如按键等以及其他用户自定义消息
	Display					// 显示函数
};

const S_FormNode g_fmInfo = 
{
	(PS_Form)&fmInfo,					// 对应窗体
	(PS_FormNode)&g_fmMenu,				// 父菜单
	NULL,			// 子项
	NULL,			// 下一项
	NULL			// 上一项
};

// 打印水箱箱液位
// 参数 forceRefresh：true=不管液位是否变化都要刷新
static void ShowShuiXiangYeWei(bool forceRefresh )
{
	if ((!forceRefresh))
	{
		return;
	}
	// 水箱水位
	LcmClearRectangle(0,0,128,12);
	if (g_ZuobianSensor.ShuiXiangLow)
	{
		LcmPrintString(0,0,"低水位");
	}
	else if(g_ZuobianSensor.ShuiXiangHigh)
	{
		LcmPrintString(0,0,"水箱满");
	}
	else
	{
		LcmPrintString(0,0,"水位正常"); 		
	}
}

static u32 m_dwWuWuShuiWei;
// 打印污物箱液位
// 参数 forceRefresh：true=不管液位是否变化都要刷新
static void ShowWuWuYeWei(bool forceRefresh )
{
	if ((!forceRefresh)&&(m_dwWuWuShuiWei == g_ZuobianSensor.WuWuXiangZhongLiang/100))
	{
		return;
	}
	// 水箱水位
	m_dwWuWuShuiWei = g_ZuobianSensor.WuWuXiangZhongLiang/100;
	LcmClearRectangle(0,ROW_HEIGH,128,12);
	LcmPrintString(0,ROW_HEIGH,"污物箱重量: %d	g",(u32)g_ZuobianSensor.WuWuXiangZhongLiang);
}

static s8 m_cReShuiWen;
static s8 m_cLengShuiWen;
// 显示水温
// 参数 forceRefresh：true=不管液位是否变化都要刷新
static void ShowReShuiWen(bool forceRefresh )
{
	if ((!forceRefresh)&&(m_cReShuiWen == g_ZuobianSensor.ReShuiWenDu))
	{
		return;
	}
	// 水箱温度
	m_cReShuiWen = g_ZuobianSensor.ReShuiWenDu;
	LcmClearRectangle(0,ROW_HEIGH*2,128,12);
	LcmPrintString(0,ROW_HEIGH*2,"当前热水水温：%d℃",(s32)m_cReShuiWen);
}

static void ShowLengShuiWen(bool forceRefresh )
{
	if ((!forceRefresh)&&(m_cLengShuiWen == g_ZuobianSensor.LengShuiWenDu))
	{
		return;
	}
	// 水箱温度
	m_cLengShuiWen = g_ZuobianSensor.LengShuiWenDu;
	LcmClearRectangle(0,ROW_HEIGH*3,128,12);
	LcmPrintString(0,ROW_HEIGH*3,"当前水箱水温：%d℃",(s32)m_cLengShuiWen);
}

static u32 m_cNuanFengWenDu;

static void ShowNuanFengWenDu(bool forceRefresh )
{
	if ((!forceRefresh)&&(m_cNuanFengWenDu == g_ZuobianSensor.ChuFengWenDu))
	{
		return;
	}
	//出风口温度
	m_cNuanFengWenDu = g_ZuobianSensor.ChuFengWenDu;
	LcmClearRectangle(0,ROW_HEIGH*4,128,12);
	LcmPrintString(0,ROW_HEIGH*4,"当前出风口温度：%d℃",(s32)m_cNuanFengWenDu);
}

//-------------------- 第二页 --------------------------

// 显示便门打开状态
static void ShowBianMenDaKai(bool forceRefresh)
{
	static u8 bianmen=0;
	if ((!forceRefresh)&&(bianmen == g_ChuangTiSensor.BianMenKaiDaoDing))
	{
		return;
	}
	bianmen = g_ChuangTiSensor.BianMenKaiDaoDing;
	LcmClearRectangle(0,ROW_HEIGH*0,128,12);
	LcmPrintString(0,ROW_HEIGH*0,g_ChuangTiSensor.BianMenKaiDaoDing?"便门打开到位":"便门没有打开到位");
}

// 显示便门关闭状态
static void ShowBianMenGuanBi(bool forceRefresh)
{
	static u8 bianmen=0;
	if ((!forceRefresh)&&(bianmen == g_ChuangTiSensor.BianMenGuanDaoDi))
	{
		return;
	}
	bianmen = g_ChuangTiSensor.BianMenGuanDaoDi;
	LcmClearRectangle(0,ROW_HEIGH*1,128,12);
	LcmPrintString(0,ROW_HEIGH*1,g_ChuangTiSensor.BianMenGuanDaoDi?"便门关闭到位":"便门没有关闭到位");
}

// 显示电池状况
static void ShowBatt(bool forceRefresh)
{
	if ((!forceRefresh))
	{
		return;
	}
	LcmClearRectangle(0,ROW_HEIGH*2,128,12);	
	if(!g_ChuangTiSensor.DianChiZhuOn)
	{
		LcmPrintString(0,ROW_HEIGH*2,"无电池");
	}
	else
	{
		if(g_ChuangTiSensor.TingDian)
			LcmPrintString(0,ROW_HEIGH*2,"停电");
		else
			if(g_ChuangTiSensor.DianLiangBuZu)
				LcmPrintString(0,ROW_HEIGH*2,"电池电量不足");
	}
}

static u8 m_bPage = 0;
void ShowPage(bool forceRefresh)
{
	if (0==m_bPage)
	{
		ShowWuWuYeWei(forceRefresh);
		ShowShuiXiangYeWei(forceRefresh);
		ShowReShuiWen(forceRefresh);
		ShowLengShuiWen(forceRefresh);
		ShowNuanFengWenDu(forceRefresh);
	}
	else
	{
		ShowBianMenDaKai(forceRefresh);
		ShowBianMenGuanBi(forceRefresh);
		ShowBatt(forceRefresh);
	}
}

static void Display()
{
	ShowPage(false);
}



static void Active(bool active)
{
	if (active)
	{
		ClrScreen();
		ShowPage(true);
	}
}

static bool FormEvent(u32 msg)
{
	switch(msg)
	{
	case KEY_LEFT:
		if (m_bPage!=0)
		{
			m_bPage--;
		}
		else
		{
			m_bPage = 1;
		}
		ClrScreen();
		ShowPage(true);
		return true;
	case KEY_RIGHT:
		if (m_bPage!=1)
		{
			m_bPage++;
		}
		else
		{
			m_bPage = 0;
		}
		ClrScreen();
		ShowPage(true);
		return true;
	}
	return false;
}

