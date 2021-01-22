/********************************************************************
	主界面
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include "FormMain.h"
#include "Lcm/Lcm.h"
//#include "FormInfo.h"
#include "FormId.h"
#include "Key/Key.h"
#include "FormMenu.h"
#include "CanAnalyzer.h"

static void Display(void);
static void Active(bool active);

static const S_Form fmMain = 
{
	MAIN_FORM_ID,			// 窗体ID，应当是全局唯一
	0,						// 超时退出时间，0表示不超时退出
	NULL,					// 窗体创建时调用
	NULL,					// 窗体退出时调用
	Active,					// 窗体激活事件，参数true表示激活，false表示进入非激活状态
	NULL	/*MainFormEvent*/,					// 事件响应，如按键等以及其他用户自定义消息
	Display					// 显示函数
};

const S_FormNode g_fmMain = 
{
	(PS_Form)&fmMain,					// 对应窗体
	NULL,			// 父菜单
	(PS_FormNode)&g_fmMenu,			// 子项
	NULL,			// 下一项
	NULL			// 上一项
};

//static u8 m_flgShuiWei;
// 打印水箱箱液位
// 参数 forceRefresh：true=不管液位是否变化都要刷新
static void ShowShuiXiangYeWei(bool forceRefresh)
{
	if (forceRefresh)
	{
		if (g_ZuobianSensor.ShuiXiangLow)
		{
			LcmClearRectangle(0,0,64,12);
			LcmPrintString(0,0,"低水位");
		}
		else if(g_ZuobianSensor.ShuiXiangHigh)
		{
			LcmClearRectangle(0,0,64,12);
			LcmPrintString(0,0,"水箱满");
		}
		else
		{
			LcmClearRectangle(0,0,64,12);
			LcmPrintString(0,0,"水位正常");			
		}
	}
}

static u16 m_flgWuWu;

// 打印污物箱液位
// 参数 forceRefresh：true=不管液位是否变化都要刷新
static void ShowWuWuYeWei(bool forceRefresh )
{
	if (forceRefresh || ((g_ZuobianSensor.WuWuXiangZhongLiang/100)!=m_flgWuWu))
	{
		if (g_ZuobianSensor.WuWuXiangZhongLiang>=WU_WU_XIANG_BORDER)
		{
			LcmClearRectangle(64,0,64,12);
			LcmPrintString(64+16,0,"污物满");
		}
		else
		{
			LcmClearRectangle(64,0,64,12);
			LcmPrintString(64+16,0,"污物箱重量: %d  g",(u32)g_ZuobianSensor.WuWuXiangZhongLiang);
		}
	}
	m_flgWuWu = g_ZuobianSensor.WuWuXiangZhongLiang/100;
}


// 显示当前动作
// 参数 forceRefresh：true=不管液位是否变化都要刷新
static void ShowAction(bool forceRefresh )
{
#if 1
	if(forceRefresh)
	{
		LcmClearRectangle(0,20,128,12);
		LcmCenterString(20,"强鹰科技");
	}
#else
	extern u8 m_actionCmd;
	char str[16];
	sprintf(str,"当前命令: %d",(u32)m_actionCmd);
	LcmClearRectangle(0,20,128,12);
	LcmCenterString(20,str);
#endif
}

// 显示大小便状态
// 参数 forceRefresh：true=不管液位是否变化都要刷新
static void ShowDaXiaoBian(bool forceRefresh )
{
	static u8 last = 0;
#if 1
	if(!forceRefresh)
	{
		if(g_ZuobianSensor.DaBian)
		{
			if(last == 2)
			{
				return;
			}
			else
			{
				last = 2;
			}
		}
		else if(g_ZuobianSensor.ManXiaoBian||g_ZuobianSensor.WomenXiaoBian)
		{
			if(last==1)
			{
				return;
			}
			else
			{
				last = 1;
			}
		}
		else
		{
			if(last ==0)
			{
				return;
			}
			else
			{
				last = 0;
			}
		}
	}
	LcmClearRectangle(0,48,128,12);
	if(g_ZuobianSensor.DaBian)
	{
		LcmCenterString(48,"检测到大便");
	}
	else if(g_ZuobianSensor.ManXiaoBian||g_ZuobianSensor.WomenXiaoBian)
	{
		LcmCenterString(48,"检测到小便");
	}

#else
	LcmClearRectangle(0,48,128,12);
	LcmPrintString(16,48,"传感器:%X %X",(u32)*(u8*)&g_ZuobianSensor,(u32)*(((u8*)&g_ZuobianSensor)+1));
#endif
}



static void Display()
{
	ShowWuWuYeWei(false);
	ShowShuiXiangYeWei(false);
	ShowAction(false);
	ShowDaXiaoBian(false);
}

static void Active(bool active)
{
	if (active)
	{
		ClrScreen();
		ShowWuWuYeWei(true);
		ShowShuiXiangYeWei(true);
		ShowAction(true);
		ShowDaXiaoBian(true);
	}
}

// 弹出提示对话框，每个文字占用 16*12 空间
void MessageBox(const char * strMsg,const char *caption)
{
	ClrScreen();
	LcmPrintString(0,0,caption);
	LcmCenterString(20,strMsg);
	GuiMessageBoxShow(GUI_DELAY_COUNTER_S(3));
};


bool MainFormEvent(u32 msg)
{
	return false;
}

