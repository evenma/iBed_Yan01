/*******************************
	日志:
		2021.1.12.  
				1. 左右翻身修改为液晶屏升降，

********************************/
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

u8 m_alarmledState=0;   // 报警灯闪烁状态标识
u8 m_alarmledTimer=0;   // 报警灯闪烁周期定义为3 秒
u8 m_alarmledCount=0;   // 报警灯闪烁的次数
u8 m_alarmledYanshi=0;  // 速热器判断工作状态的延时计数 

u8 m_actionCmd = 0;
// 传感器状态全局变量
volatile s_ChuangTiSensor g_ChuangTiSensor={0};
volatile s_ZuobianSensor g_ZuobianSensor={0};
//volatile S_ZuobianqiSetB g_ZuobianqiSetB={0};
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
			g_ZuobianSensor = *(s_ZuobianSensor*)&rcvBuf.CANRDA;
			break;
		case 0x200:
			g_ChuangTiSensor = *(s_ChuangTiSensor*)&rcvBuf.CANRDA;
			break;	
		case 0x111:
			g_ZuobianqiSetB = *(S_ZuobianqiSetB*)&rcvBuf.CANRDA;
			if(g_ZuobianqiSetB.HoldOn)		//面板B参数被更改 同步参数
			{
	//			rt_kprintf("B面板参数被更改\r\n");
				g_ZuobianqiSetA.QiangXiQiangRuo = g_ZuobianqiSetB.QiangXiQiangRuo;
				g_ZuobianqiSetA.ShuiWenGaoDi = g_ZuobianqiSetB.ShuiWen;
				g_ZuobianqiSetA.NuanFengQiangRuo=g_ZuobianqiSetB.NuanFeng;
//				if((g_ZuobianqiSetB.ShuiWen<30))
//					g_ZuobianqiSetA.ShuiWenGaoDi = 0;
//				if((g_ZuobianqiSetB.ShuiWen<=34)&&(g_ZuobianqiSetB.ShuiWen>=30))
//					g_ZuobianqiSetA.ShuiWenGaoDi = 1;
//				if((g_ZuobianqiSetB.ShuiWen<=37)&&(g_ZuobianqiSetB.ShuiWen>34))
//					g_ZuobianqiSetA.ShuiWenGaoDi = 2;
//				if((g_ZuobianqiSetB.ShuiWen<=41)&&(g_ZuobianqiSetB.ShuiWen>37))
//					g_ZuobianqiSetA.ShuiWenGaoDi = 3;	
//				if((g_ZuobianqiSetB.NuanFeng<40))
//					g_ZuobianqiSetA.NuanFengQiangRuo= 0;
//				if((g_ZuobianqiSetB.NuanFeng<=45)&&(g_ZuobianqiSetB.NuanFeng>=40))
//					g_ZuobianqiSetA.NuanFengQiangRuo = 1;
//				if((g_ZuobianqiSetB.NuanFeng<=55)&&(g_ZuobianqiSetB.NuanFeng>45))
//					g_ZuobianqiSetA.NuanFengQiangRuo = 2;
//				if((g_ZuobianqiSetB.NuanFeng<=65)&&(g_ZuobianqiSetB.NuanFeng>55))
//					g_ZuobianqiSetA.NuanFengQiangRuo = 3;					
			}
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
	{KEY_FANSHEN_ZUO_UP,15},
	{KEY_FANSHEN_ZUO_DOWN,16},	
	{KEY_FANSHEN_YOU_UP,17},
	{KEY_FANSHEN_YOU_DOWN,18},

	{KEY_CANCEL,100},

	{KEY_PAD_UP,118},
	{KEY_PAD_DOWN,119},
	
	{KEY_QIZUO, 		121},
	{KEY_PINGTANG,		122},
	{KEY_XINZANGTANGWEI,123},   
	{KEY_BIANMEN_KAI,	124},
	{KEY_BIANMEN_GUAN,	125},
	
	{KEY_CHONGXIBIANPEN,128},
	{KEY_QINGXITUNBU,	129},
	{KEY_GANZAO,		130},
	{KEY_QINGXISHENGZHIQI,131},//
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
	if(msg == KEY_SET_SHUIWEN) 		// 水温四档设置
	{			
		g_ZuobianqiSetA.HoldOn=1;
		g_ZuobianqiSetA.ShuiWenGaoDi += 1;
		if(g_ZuobianqiSetA.ShuiWenGaoDi>3)
		{	g_ZuobianqiSetA.ShuiWenGaoDi=0;}
	}
	if(msg == KEY_SET_NUANFEN_ADD)  
	{	
		g_ZuobianqiSetA.HoldOn=1;			
		if(g_ZuobianSensor.FunctionWorkNuanFeng)
		{
			g_ZuobianqiSetA.NuanFengQiangRuo += 1;
			if(g_ZuobianqiSetA.NuanFengQiangRuo>3)
				g_ZuobianqiSetA.NuanFengQiangRuo=3;		
		}
		if(g_ZuobianSensor.FunctionWorkHouBu
		||g_ZuobianSensor.FunctionWorkQianBu)
		{
			g_ZuobianqiSetA.QiangXiQiangRuo+=1;
			if(g_ZuobianqiSetA.QiangXiQiangRuo>3)
				g_ZuobianqiSetA.QiangXiQiangRuo=3;
		}
	}	 
	if(msg == KEY_SET_NUANFEN_SUB) 
	{	  
		  g_ZuobianqiSetA.HoldOn=1;
		  if(g_ZuobianSensor.FunctionWorkNuanFeng)
		  {
			if(g_ZuobianqiSetA.NuanFengQiangRuo<1)
				g_ZuobianqiSetA.NuanFengQiangRuo=0;
			else
				g_ZuobianqiSetA.NuanFengQiangRuo -= 1;
		  }
		  if(g_ZuobianSensor.FunctionWorkHouBu
			||g_ZuobianSensor.FunctionWorkQianBu)
		  {
			if(g_ZuobianqiSetA.QiangXiQiangRuo<=1)
				g_ZuobianqiSetA.QiangXiQiangRuo = 1;
			else
				g_ZuobianqiSetA.QiangXiQiangRuo -= 1;
		  } 				
	}		
	for(i=0;i<sizeof(m_aKeyActionMap)/sizeof(t_KeyActionMap);i++)
	{
		if(msg == m_aKeyActionMap[i].key)
		{
			switch(msg)  // 异常判断
			{
				case KEY_ZUOBIAN_JIESHU:
				case KEY_ZUOBIAN_KAISHI:
				case KEY_CHONGXIBIANPEN:
				case KEY_QINGXITUNBU:
				case KEY_QINGXISHENGZHIQI:
					if(!(g_ChuangTiSensor.ZuoFanFangPing&&g_ChuangTiSensor.YouFanFangPing))
					{
						m_actionCmd = 0;
						m_alarmledState=1;
						m_alarmledCount=7;	// 每4秒闪烁7次						
						Alarm("请首先翻身放平","禁止操作");
						return;
					}
					else if(!(g_ChuangTiSensor.BianMenKaiDaoDing))
					{
						m_actionCmd = 0;
						rt_kprintf("请先打开便门\r\n");
						Alarm("请先打开便门","禁止操作");
						m_alarmledState=1;
						m_alarmledCount=1;	// 每3秒闪烁1次
						return;
					}
					else if(!(g_ZuobianSensor.WuWuXiangGuan)) 
					{
						m_actionCmd = 0;
						rt_kprintf("污物箱管路未连接\r\n");
						Alarm("污物箱管路未连接","禁止操作");
						m_alarmledState=1;
						m_alarmledCount=2;	// 每3秒闪烁2次
						return;						
					}
				    else if(g_ZuobianSensor.WuWuXiangZhongLiang>=WU_WU_XIANG_BORDER)  
					{
						m_actionCmd = 0;
						rt_kprintf("污物箱液位满\r\n");
						Alarm("污物箱液位满","禁止操作");
						m_alarmledState=1;
						m_alarmledCount=3;	// 每3秒闪烁3次
						return;						
					}
				    else if(g_ZuobianSensor.ShuiXiangLow)  
					{
						m_actionCmd = 0;
						rt_kprintf("水箱水位过低\r\n");
						Alarm("水箱水位过低","禁止操作");
						m_alarmledState=1;
						m_alarmledCount=4;	// 每3秒闪烁4次
						return;						
					}	
					break;
				case KEY_GANZAO:
					if(!(g_ChuangTiSensor.ZuoFanFangPing&&g_ChuangTiSensor.YouFanFangPing))
					{
						m_actionCmd = 0;
						m_alarmledState=1;
						m_alarmledCount=7;	// 每4秒闪烁7次						
						Alarm("请首先翻身放平","禁止操作");
						return;
					}
					else if(!(g_ChuangTiSensor.BianMenKaiDaoDing))
					{
						m_actionCmd = 0;
						rt_kprintf("请先打开便门\r\n");
						Alarm("请先打开便门","禁止操作");
						m_alarmledState=1;
						m_alarmledCount=1;	// 每3秒闪烁1次
						return;
					}					
					break;
				case KEY_BEIBAN_UP:
				case KEY_TUIBU_DOWN:
				case KEY_TUIBU_UP:
				case KEY_QIZUO:
				case KEY_XINZANGTANGWEI:
				case KEY_BIANMEN_KAI:
					if(!(g_ChuangTiSensor.ZuoFanFangPing&&g_ChuangTiSensor.YouFanFangPing))
					{
						m_actionCmd = 0;
						m_alarmledState=1;
						m_alarmledCount=7;	// 每4秒闪烁7次
						rt_kprintf("请首先翻身放平\r\n");
						Alarm("请首先翻身放平","禁止操作");
						return;
					}
					break;
				case KEY_FANSHEN_ZUO_UP:  
				case KEY_FANSHEN_YOU_UP: 					
					if((!(g_ChuangTiSensor.BeiBuFangPing&&g_ChuangTiSensor.TuiBuFangPing)))
					{
						m_actionCmd = 0;
						m_alarmledState=1;
						m_alarmledCount=8;	// 每4秒闪烁8次
						rt_kprintf("请先将床板放平\r\n");						
						Alarm("请先将床板放平","禁止操作");
						return;
					}
					else if(!(g_ChuangTiSensor.BianMenGuanDaoDi))
					{
						m_actionCmd = 0;
						m_alarmledState=1;
						m_alarmledCount=8;	// 每4秒闪烁8次
						rt_kprintf("请先关闭便门\r\n");
						Alarm("请先关闭便门","禁止操作");
						return;
					}
					break;
	
					default:
						break;					
			}
			if(msg== KEY_RELEASE)	
			{
				m_alarmledState=0;
				m_alarmledCount=0;
				m_alarmledYanshi=0;
				ALARM_RUN_LED(0);
				ALARM_ERROR_LED(0);
			}
			m_actionCmd = m_aKeyActionMap[i].action;
			CanWrite(0, 0x100,1,&m_actionCmd);   // 发送命令信息
			if(m_actionCmd>0) // 表示有命令执行
			{
				ALARM_RUN_LED(1);  // 工作指示灯亮
			}
			return;
		}	
	}
	m_actionCmd = 0;
}

static u8 cansend_count=0;
// 10ms 周期发送
static void CanSend10(void* p)
{
	static u8 count = 0;
	//	{KEY_QINGXITUNBU,	131},  
   if((m_actionCmd==131)&&(m_alarmledYanshi>15)&&(m_alarmledYanshi<140)&&!(g_ZuobianSensor.SuReQiWork))  
	{
		m_actionCmd = 100;	// 发送停止命令
//		Alarm("加热器未工作","禁止操作");
		m_alarmledState=1;
		m_alarmledCount=6;	// 每3秒闪烁6次	
	}
	CanWrite(0, 0x100,1,&m_actionCmd);   // 发送命令信息

	if(count == 0)
	{
		if(!g_ZuobianqiSetB.HoldOn)   // 面板B未处于设置
		{
			CanWrite(0, 0x110,8,(u8*)&g_ZuobianqiSetA.HoldOn);  // 发送设置信息
		}
		if(g_ZuobianqiSetA.HoldOn)
		{	
			cansend_count+=1;
			if(cansend_count>5)
				{g_ZuobianqiSetA.HoldOn=0;cansend_count=0;}
		}
	}
	if(count++ >= 5)
	{
		count = 0;
	}	
}


static struct rt_timer timer10;
//static struct rt_timer timer50;
static struct rt_timer LED_timer;

// CANAnalyzer 初始化
void CanAnalyzerInit(void)
{
	CanOpen(0,CanRxInd);	

	rt_timer_init(&timer10, "act", CanSend10, RT_NULL, RT_TICK_PER_SECOND*100/1000, RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&timer10);
	
//	rt_timer_init(&timer50, "act", CanSend50, RT_NULL, RT_TICK_PER_SECOND*50/1000, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
//	rt_timer_start(&timer50);
}

// 显示传感器信息
static void SensorList(void * parameter)
{
	rt_kprintf("当前GPIO配置:\r\n");
	rt_kprintf("\tPINSEL0=%04X    PINSEL1=%04X    PINSEL2=%04X    IO0DIR=%04X    IO1DIR=%04X    IO0PIN=%04X    IO1PIN=%04X    \r\n",
		PINSEL0,PINSEL1,PINSEL2,IO0DIR,IO1DIR,IO0PIN,IO1PIN);

	rt_kprintf("座便器传感器状态:\r\n");
	rt_kprintf("\t大便: %s\r\n",g_ZuobianSensor.DaBian?"Y":"");
	rt_kprintf("\t男士小便: %s\r\n",g_ZuobianSensor.ManXiaoBian?"Y":"");
	rt_kprintf("\t女士小便: %s\r\n",g_ZuobianSensor.WomenXiaoBian?"Y":"");
	rt_kprintf("\t污物箱管路已连接: %s\r\n",g_ZuobianSensor.WuWuXiangGuan?"Y":"");	
	rt_kprintf("\t水箱满: %s\r\n",g_ZuobianSensor.ShuiXiangHigh?"Y":"");	
	rt_kprintf("\t水箱低: %s\r\n",g_ZuobianSensor.ShuiXiangLow?"Y":"");	
	rt_kprintf("\t速热器工作: %s\r\n",g_ZuobianSensor.SuReQiWork?"Y":"");
	rt_kprintf("\t速热器报警错误: %d\r\n",(u32)g_ZuobianSensor.SuReQiError);
	rt_kprintf("\t污物箱重量: %d\r\n",(u32)g_ZuobianSensor.WuWuXiangZhongLiang);
	rt_kprintf("\t出风口温度: %d\r\n",(u32)g_ZuobianSensor.ChuFengWenDu);
	rt_kprintf("\t水温调节: %d档\r\n",(u32)g_ZuobianqiSetA.ShuiWenGaoDi);
	rt_kprintf("\t暖风温度: %d档\r\n",(u32)g_ZuobianqiSetA.NuanFengQiangRuo);
	rt_kprintf("\t清洗强度: %d档\r\n",(u32)g_ZuobianqiSetA.QiangXiQiangRuo);
	 
	rt_kprintf("床体传感器状态:\r\n");
	rt_kprintf("\t背部放平: %s\r\n",g_ChuangTiSensor.BeiBuFangPing?"Y":"");
	rt_kprintf("\t腿部放平: %s\r\n",g_ChuangTiSensor.TuiBuFangPing?"Y":"");
	rt_kprintf("\t左侧翻水平到位: %s\r\n",g_ChuangTiSensor.ZuoFanFangPing?"Y":"");
	rt_kprintf("\t右侧翻水平到位: %s\r\n",g_ChuangTiSensor.YouFanFangPing?"Y":"");
	rt_kprintf("\t防夹检测: %s\r\n",g_ChuangTiSensor.FangJia?"Y":"");	
	rt_kprintf("\t便门全打开: %s\r\n",g_ChuangTiSensor.BianMenKaiDaoDing?"Y":"");
	rt_kprintf("\t便门全关闭: %s\r\n",g_ChuangTiSensor.BianMenGuanDaoDi?"Y":"");
	rt_kprintf("\t便门平移到顶: %s\r\n",g_ChuangTiSensor.BianMenPingYiDaoDing?"Y":"");
	rt_kprintf("\t便门平移到底: %s\r\n",g_ChuangTiSensor.BianMenPingYiDaoDi?"Y":"");
	rt_kprintf("\t电池电量不足: %s\r\n",g_ChuangTiSensor.DianLiangBuZu?"Y":"");
	rt_kprintf("\t电机过流保护: %s\r\n",g_ChuangTiSensor.DianJiGuoLiuBao?"Y":"");

	rt_kprintf("\t市电断电: %s\r\n",g_ChuangTiSensor.TingDian?"Y":"");
	rt_kprintf("\t电池是否存在: %s\r\n",g_ChuangTiSensor.DianChiZhuOn?"Y":"");
	rt_kprintf("\t起背角度: %d °\r\n",g_ChuangTiSensor.QiBeiJiaoDu);
	rt_kprintf("\t下曲腿角度: %d °\r\n",g_ChuangTiSensor.XiaQuTuiJiaoDu);
	rt_kprintf("\t上曲腿角度: %d °\r\n",g_ChuangTiSensor.ShangQuTuiJiaoDu);	
	rt_kprintf("\t左翻身角度: %d °\r\n",g_ChuangTiSensor.ZuoFanShenJiaoDu);
	rt_kprintf("\t右翻身角度: %d °\r\n",g_ChuangTiSensor.YouFanShenJiaoDu);

	rt_kprintf("面板A变量状态:\r\n");
	rt_kprintf("\t计时: \t%X",(u32)m_alarmledYanshi);
	rt_kprintf("\tg_actionSend= ");
	rt_kprintf("\t%X",(u32)m_actionCmd);
	rt_kprintf("\r\n");
}
FINSH_FUNCTION_EXPORT(SensorList, "显示当前传感器状态");

#ifdef RT_USING_FINSH
// 自动监控
static void AutoSensor(u32 start)
{
	static u8 init = 0;
	static struct rt_timer t;
	if(init ==0)
	{
		rt_timer_init(&t, "monitor", SensorList, NULL, RT_TICK_PER_SECOND*2, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
		init = 1;
	}
	if(start)
	{
		rt_timer_start(&t);
	}
	else
	{
		rt_timer_stop(&t);
	}
}
FINSH_FUNCTION_EXPORT(AutoSensor, "自动监控传感器,参数[0]=0表示停止，否则表示开始");
#endif


static void FanZhuanIO(void)
{
   if((IO1PIN&BIT17)>0)
   {
      ALARM_ERROR_LED(0);
   }
   else
   {
	  ALARM_ERROR_LED(1);
   }
}

void LEDSetInit(void)
{
	switch(g_ZuobianqiSetA.ShuiWenGaoDi) 
	{
		case 0: SET_SHUIWEN_LOW(0);SET_SHUIWEN_MIDDLE(0);SET_SHUIWEN_HIGH(0);break;
		case 1: SET_SHUIWEN_LOW(1);SET_SHUIWEN_MIDDLE(0);SET_SHUIWEN_HIGH(0);break;
		case 2: SET_SHUIWEN_LOW(0);SET_SHUIWEN_MIDDLE(1);SET_SHUIWEN_HIGH(0);break;
		case 3: SET_SHUIWEN_LOW(0);SET_SHUIWEN_MIDDLE(0);SET_SHUIWEN_HIGH(1);break;
		default:break;	
	}
	switch(g_ZuobianqiSetA.NuanFengQiangRuo) 
	{
		case 0: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(0);break;
		case 1: SET_NUANFEN_LOW(1);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(0);break;
		case 2: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(1);SET_NUANFEN_HIGH(0);break;
		case 3: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(1);break;
		default:break;	
	}
}
void LEDSet(void)
{
	switch(g_ZuobianqiSetA.ShuiWenGaoDi) 
	{
		case 0: SET_SHUIWEN_LOW(0);SET_SHUIWEN_MIDDLE(0);SET_SHUIWEN_HIGH(0);break;
		case 1: SET_SHUIWEN_LOW(1);SET_SHUIWEN_MIDDLE(0);SET_SHUIWEN_HIGH(0);break;
		case 2: SET_SHUIWEN_LOW(0);SET_SHUIWEN_MIDDLE(1);SET_SHUIWEN_HIGH(0);break;
		case 3: SET_SHUIWEN_LOW(0);SET_SHUIWEN_MIDDLE(0);SET_SHUIWEN_HIGH(1);break;
		default:break;	
	}
	if(g_ZuobianSensor.FunctionWorkNuanFeng)	// 如果是工作在暖风烘干模式下
	{switch(g_ZuobianqiSetA.NuanFengQiangRuo) 
	{
		case 0: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(0);break;
		case 1: SET_NUANFEN_LOW(1);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(0);break;
		case 2: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(1);SET_NUANFEN_HIGH(0);break;
		case 3: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(1);break;
		default:break;	
	}}	
	if(g_ZuobianSensor.FunctionWorkHouBu
		||g_ZuobianSensor.FunctionWorkQianBu)	// 如果是工作在热水清洗模式下
	{switch(g_ZuobianqiSetA.QiangXiQiangRuo) 
	{
		case 1: SET_NUANFEN_LOW(1);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(0);break;
		case 2: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(1);SET_NUANFEN_HIGH(0);break;
		case 3: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(1);break;
		default:break;	
	}}		
}

void AlarmLED(void* p)
{
	if(g_ZuobianSensor.ShuiXiangLow)  // 检测到水位低
	{	ALARM_SHUIWEI_LOW(1);}
	else
	{	ALARM_SHUIWEI_LOW(0);}
	if(g_ZuobianSensor.WuWuXiangZhongLiang>=WU_WU_XIANG_BORDER)  // 检测到 污物箱满
	{	ALARM_WUWU_HIGN(1);}
	else
	{	ALARM_WUWU_HIGN(0);}

	LEDSet();
	
	if(m_actionCmd>0) // 表示有命令执行
	{
		m_alarmledYanshi+=1;
		if(m_alarmledYanshi>250)
			m_alarmledYanshi=250;	
	}
	if(m_alarmledState) // 报警状态
	{
//			ALARM_RUN_LED(0);
		m_alarmledTimer+=1;
		if(m_alarmledTimer>16)  // 4 秒为1 周期2014.8.13 chang
			m_alarmledTimer=1;	
		if(m_alarmledCount*2>=m_alarmledTimer)
		    FanZhuanIO();
		else 
			ALARM_ERROR_LED(0);

	}
	else
	{
       m_alarmledTimer=0;
	}
    feedWD();    // 喂狗
}


void AlarmLEDInit(void) //每0.5秒触发一次
{ 
  rt_timer_init(&LED_timer, "AlarmLED", AlarmLED, RT_NULL, RT_TICK_PER_SECOND*0.25f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
  rt_timer_start(&LED_timer);
  AutoSensor(0);
}


