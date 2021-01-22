#include <rtthread.h>
#include <finsh.h>
#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif
#include "CAN/IncludeCan.h"
#include "CAN/Can.h"
#include "lpc214x.h"
#include <Key/Key.h>
#include "CanAnalyzer.h"
#include "GlobalSetting.h"
#include "Alarm.h"


u8 m_actionCmd = 0;

s_ToiletSensor g_ToiletSensor = {0};
// 床板传感器状态全局变量
volatile s_BedSensor g_bedSensor = {0};

// 接收到CAN回调函数
rt_err_t CanRxInd(rt_device_t dev, rt_size_t size)
{
	stcRxBUF rcvBuf;
	while(CanRead(0, &rcvBuf))
	{
//		DbgPrintf("Rcv CAN.ID=%X\r\n",rcvBuf.RxCANID.FrameFm.stcSTRFRFM.ID_BIT);
		switch(rcvBuf.RxCANID.FrameFm.stcSTRFRFM.ID_BIT)
		{
		case 0x300:
			g_ToiletSensor = *(s_ToiletSensor*)&rcvBuf.CANRDA;
			break;
		case 0x200:
			g_bedSensor = *(s_BedSensor*)&rcvBuf.CANRDA;
			break;
		default:
			break;
		}
	}
	return RT_EOK;
}

typedef struct {
	u32 key;
	u8 action;
}t_KeyActionMap;

static const t_KeyActionMap m_aKeyActionMap[] = {
	{KEY_RELEASE,		0},
	{KEY_BEIBAN_UP,		1},
	{KEY_BEIBAN_DOWN,	2},
	{KEY_TUIBU_UP,		3},
	{KEY_TUIBU_DOWN,	4},
	{KEY_QIZUO,			5},
	{KEY_PINGTANG,		6},
	{KEY_CUITIYANGWO,	7},
	{KEY_XINZANGTANGWEI,8},
	{KEY_ZHENTOU_UP,	9},
	{KEY_ZHENTOU_DOWN,	10},
	{KEY_BIANMEN_KAI,	11},
	{KEY_BIANMEN_GUAN,	12},
	{KEY_FENTUIBAN_KAI,	15},
	{KEY_FENTUIBAN_GUAN,16},
	{KEY_FANSHEN_UP,17},
	{KEY_FANSHEN_DOWN,18},
	{KEY_BIANZHAO_KAI,19},
	{KEY_BIANZHAO_GUAN,20},
	{KEY_CHONGXIBIANPEN,129},
	{KEY_GANZAO,		133},
	{KEY_QINGXITUNBU,	131},
	{KEY_QINGXISHENGZHIQI,135},
	{KEY_TANGBIAN_KAISHI, 136},
	{KEY_TANGBIAN_JIESHU, 137},
	{KEY_ZUOBIAN_KAISHI,  138},
	{KEY_ZUOBIAN_JIESHU,  139},
	{KEY_QIFANSHEN_ZUO,17},  //  气翻身-左 add mayb for 气垫控制2014.8.13.
	{KEY_QIFANSHEN_YOU,18},  //  气翻身-右	add mayb for 气垫控制2014.8.13.
	{KEY_QIFANSHEN_ZUOYOUYAO,154},	 //  气翻身-右	add mayb for 气垫控制2014.8.13.
	{KEY_QIFANSHEN_PING,21}, //  气翻身-平躺 add mayb for 气垫控制2014.8.13.

	{KEY_CANCEL,255},
};

void Alarm(const char * strMsg,const char *caption)
{
	MessageBox(strMsg,caption);
	ALARM_BUZZER(1);
	rt_thread_delay(RT_TICK_PER_SECOND*50/1000);
	ALARM_BUZZER(0);
	rt_thread_delay(RT_TICK_PER_SECOND*100/1000);
	ALARM_BUZZER(1);
	rt_thread_delay(RT_TICK_PER_SECOND*50/1000);
	ALARM_BUZZER(0);
}

// 发送按键动作指令
void CanSetKeyCmd(u32 msg)
{
	int i;
	for(i=0;i<sizeof(m_aKeyActionMap)/sizeof(t_KeyActionMap);i++)
	{
		if(msg == m_aKeyActionMap[i].key)
		{
			switch(msg)
			{
				case KEY_TANGBIAN_KAISHI:
				case KEY_TANGBIAN_JIESHU:
				case KEY_ZUOBIAN_JIESHU:
				case KEY_ZUOBIAN_KAISHI:
				case KEY_CHONGXIBIANPEN:
				case KEY_GANZAO:
				case KEY_QINGXITUNBU:
				case KEY_QINGXISHENGZHIQI:
					if(!g_bedSensor.ShuiPing)
					{
						m_actionCmd = 0;
						Alarm("请首先翻身放平","禁止操作");
						return;
					}
					else if(!(g_ToiletSensor.QianBianMenZuoDaKai
							&&g_ToiletSensor.QianBianMenYouDaKai
							&&g_ToiletSensor.HouBianMenDaKai))
					{
						m_actionCmd = 0;
						Alarm("请先打开便门","禁止操作");
						return;
					}
					break;
				case KEY_BEIBAN_UP:
				case KEY_TUIBU_DOWN:
				case KEY_QIZUO:
				case KEY_XINZANGTANGWEI:
				case KEY_BIANMEN_KAI:
				case KEY_FENTUIBAN_KAI:
					if(!g_bedSensor.ShuiPing)
					{
						m_actionCmd = 0;
						Alarm("请首先翻身放平","禁止操作");
						return;
					}
					break;
				case KEY_FANSHEN_DOWN:
				case KEY_FANSHEN_UP:
					if(!(g_bedSensor.BeiBuFangPing&&g_bedSensor.TuiBuFangPing))
					{					
						m_actionCmd = 0;
						Alarm("请先将床板放平","禁止操作");
						return;
					}
					else if(!(g_ToiletSensor.ZuoBiTuiDaoWei&&g_ToiletSensor.YouBiTuiDaoWei))
					{
						m_actionCmd = 0;
						Alarm("请先关闭分腿板","禁止操作");
						return;
					}
					else if(!(g_ToiletSensor.QianBianMenZuoGuanBi&&
							g_ToiletSensor.QianBianMenYouGuanBi&&
							g_ToiletSensor.HouBianMenGuanBi))
					{
						m_actionCmd = 0;
						Alarm("请先关闭便门","禁止操作");
						return;
					}
					break;
			}
			m_actionCmd = m_aKeyActionMap[i].action;
			return;
		}
	}
	m_actionCmd = 0;
}


// 10ms 周期发送
static void CanSend10(void* p)
{
	static u8 count = 0;
	CanWrite(0, 0x100,1,&m_actionCmd);
	if(count++ >= 5)
	{
		count = 0;
	}
	if(count == 0)
	{
		CanWrite(0, 0x110,8,(u8*)&g_sGlobalSetting.acMsgNumber);
	}
	else if(count == 1)
	{
		CanWrite(0, 0x111,8,(u8*)&g_sGlobalSetting.acMsgNumber+8);
	}
	else if(count == 2)
	{
		CanWrite(0, 0x112,8,(u8*)&g_sGlobalSetting.ToiletCfg);
	}
	else if(count == 3)
	{
		CanWrite(0, 0x113,8,(u8*)&g_sGlobalSetting.ToiletCfg.TunBuQingXiShiJian);
	}
	
}

// 50ms 周期发送
static void CanSend50(void* p)
{
}


static struct rt_timer timer10;
static struct rt_timer timer50;

// CANAnalyzer 初始化
void CanAnalyzerInit(void)
{
	CanOpen(0,CanRxInd);	

	rt_timer_init(&timer10, "act", CanSend10, RT_NULL, RT_TICK_PER_SECOND*10/1000, RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&timer10);
	
	rt_timer_init(&timer50, "act", CanSend50, RT_NULL, RT_TICK_PER_SECOND*50/1000, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
	rt_timer_start(&timer50);
}

