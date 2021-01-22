											
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

// ����Ĭ���¼��ص�
bool FormEventDefaultCallBack(u32 msg)
{
	CanSetKeyCmd(msg);
	return true;
}


// ��ȡ�Ĵ�������
void getreg(u32 addr,u32 len)
{
	while(len--)
	{
		rt_kprintf("%8X : %8X\r\n",addr,*(u32*)addr);
		addr += 4;
	}
}
FINSH_FUNCTION_EXPORT(getreg, "��ȡ[0] ��ַ�ļĴ���ֵ");


static struct rt_thread  m_guiThread;
static s8 m_guiStack[512];

// GUI ��ص��߳�
static void GuiThread(void *p)
{
	KeyInit();
	LCM_Init();
	ClrScreen();	// ����
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
	// ��������汾�ſ���
	rt_kprintf("Software Version: iBed-MianBan-5.1.0 20210116 Yan Beta \n");
#if 1
	Ds1302Init();
	// GUI ��һ���������߳�������
	rt_thread_init(&m_guiThread, "GUI", GuiThread, RT_NULL, m_guiStack, sizeof(m_guiStack), 20, 1);
	rt_thread_startup(&m_guiThread);

#endif
	// ��ȡ�洢����Ϣ
	LoadGlobalSetting();
	// �� CAN1 �豸
	CanAnalyzerInit();
	AlarmInit();
	AlarmLEDInit();  // ���ڱ�������˸ 
	WDInit();
	SetDefault();	// ȫ�ֱ�������
	LEDSetInit();		// ����LED��
    return 0;
}

