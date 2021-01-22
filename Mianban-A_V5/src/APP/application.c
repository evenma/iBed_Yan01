											
#include <rtthread.h>
#include <finsh.h>

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif

#include "CAN/IncludeCan.h"
#include "CAN/Can.h"
#include "lpc214x.h"
#include <CanAnalyzer.h>
#include <Key/Key.h>
#include <Lcm/Lcm.h>
#include <Gui/GuiCore.h>

#include "Form/FormMain.h"
#include "GlobalSetting.h"
#include "ds1302.h"

extern void CanAnalyzerInit(void);
extern void AlarmLEDInit(void);

struct rt_timer gui_timer;

struct rt_timer test_timer;

// 窗口默认事件回调
bool FormEventDefaultCallBack(u32 msg)
{
	CanSetKeyCmd(msg);
	return true;
}


// 获取寄存器数据
void getreg(u32 addr,u32 len)
{
	while(len--)
	{
		rt_kprintf("%8X : %8X\r\n",addr,*(u32*)addr);
		addr += 4;
	}
}
FINSH_FUNCTION_EXPORT(getreg, "获取[0] 地址的寄存器值");


static struct rt_thread  m_guiThread;
static s8 m_guiStack[512];

// GUI 相关的线程
static void GuiThread(void *p)
{
	KeyInit();
	LCM_Init();
	ClrScreen();	// 清屏
	SetBackLight(64);
	GuiInit();
	GuiSetRootForm((PS_FormNode)&g_fmMain);
	GuiSetDefaultEvent(FormEventDefaultCallBack);
	while(1)
	{
		KeyTick();
		GuiTick();
		rt_thread_delay(RT_TICK_PER_SECOND*GUI_TICK_CYCLE/1000);
	}
}

#include "Alarm.h"
int rt_application_init()
{
	// 增加软件版本号控制
	rt_kprintf("Software Version: iBed-MianBan-5.1.0 20210116 Yan Beta \n");
#if 1
	Ds1302Init();
	// GUI 在一个单独的线程中运行
	rt_thread_init(&m_guiThread, "GUI", GuiThread, RT_NULL, m_guiStack, sizeof(m_guiStack), 20, 1);
	rt_thread_startup(&m_guiThread);

#endif
	// 获取存储的信息
	LoadGlobalSetting();
	// 打开 CAN1 设备
	CanAnalyzerInit();
	AlarmInit();
	AlarmLEDInit();  // 用于报警灯闪烁 
	WDInit();
	SetDefault();	// 全局变量设置
	LEDSetInit();		// 设置LED灯
    return 0;
}

