/*******************************
	��־:
		2021.1.12.  
				1. ���ҷ����޸�ΪҺ����������

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

u8 m_alarmledState=0;   // ��������˸״̬��ʶ
u8 m_alarmledTimer=0;   // ��������˸���ڶ���Ϊ3 ��
u8 m_alarmledCount=0;   // ��������˸�Ĵ���
u8 m_alarmledYanshi=0;  // �������жϹ���״̬����ʱ���� 

u8 m_actionCmd = 0;
// ������״̬ȫ�ֱ���
volatile s_ChuangTiSensor g_ChuangTiSensor={0};
volatile s_ZuobianSensor g_ZuobianSensor={0};
//volatile S_ZuobianqiSetB g_ZuobianqiSetB={0};
// ���յ�CAN�ص�����
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
			if(g_ZuobianqiSetB.HoldOn)		//���B���������� ͬ������
			{
	//			rt_kprintf("B������������\r\n");
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

// ���Ͱ�������ָ��
void CanSetKeyCmd(u32 msg)
{
	int i;
	if(msg == KEY_SET_SHUIWEN) 		// ˮ���ĵ�����
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
			switch(msg)  // �쳣�ж�
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
						m_alarmledCount=7;	// ÿ4����˸7��						
						Alarm("�����ȷ����ƽ","��ֹ����");
						return;
					}
					else if(!(g_ChuangTiSensor.BianMenKaiDaoDing))
					{
						m_actionCmd = 0;
						rt_kprintf("���ȴ򿪱���\r\n");
						Alarm("���ȴ򿪱���","��ֹ����");
						m_alarmledState=1;
						m_alarmledCount=1;	// ÿ3����˸1��
						return;
					}
					else if(!(g_ZuobianSensor.WuWuXiangGuan)) 
					{
						m_actionCmd = 0;
						rt_kprintf("�������·δ����\r\n");
						Alarm("�������·δ����","��ֹ����");
						m_alarmledState=1;
						m_alarmledCount=2;	// ÿ3����˸2��
						return;						
					}
				    else if(g_ZuobianSensor.WuWuXiangZhongLiang>=WU_WU_XIANG_BORDER)  
					{
						m_actionCmd = 0;
						rt_kprintf("������Һλ��\r\n");
						Alarm("������Һλ��","��ֹ����");
						m_alarmledState=1;
						m_alarmledCount=3;	// ÿ3����˸3��
						return;						
					}
				    else if(g_ZuobianSensor.ShuiXiangLow)  
					{
						m_actionCmd = 0;
						rt_kprintf("ˮ��ˮλ����\r\n");
						Alarm("ˮ��ˮλ����","��ֹ����");
						m_alarmledState=1;
						m_alarmledCount=4;	// ÿ3����˸4��
						return;						
					}	
					break;
				case KEY_GANZAO:
					if(!(g_ChuangTiSensor.ZuoFanFangPing&&g_ChuangTiSensor.YouFanFangPing))
					{
						m_actionCmd = 0;
						m_alarmledState=1;
						m_alarmledCount=7;	// ÿ4����˸7��						
						Alarm("�����ȷ����ƽ","��ֹ����");
						return;
					}
					else if(!(g_ChuangTiSensor.BianMenKaiDaoDing))
					{
						m_actionCmd = 0;
						rt_kprintf("���ȴ򿪱���\r\n");
						Alarm("���ȴ򿪱���","��ֹ����");
						m_alarmledState=1;
						m_alarmledCount=1;	// ÿ3����˸1��
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
						m_alarmledCount=7;	// ÿ4����˸7��
						rt_kprintf("�����ȷ����ƽ\r\n");
						Alarm("�����ȷ����ƽ","��ֹ����");
						return;
					}
					break;
				case KEY_FANSHEN_ZUO_UP:  
				case KEY_FANSHEN_YOU_UP: 					
					if((!(g_ChuangTiSensor.BeiBuFangPing&&g_ChuangTiSensor.TuiBuFangPing)))
					{
						m_actionCmd = 0;
						m_alarmledState=1;
						m_alarmledCount=8;	// ÿ4����˸8��
						rt_kprintf("���Ƚ������ƽ\r\n");						
						Alarm("���Ƚ������ƽ","��ֹ����");
						return;
					}
					else if(!(g_ChuangTiSensor.BianMenGuanDaoDi))
					{
						m_actionCmd = 0;
						m_alarmledState=1;
						m_alarmledCount=8;	// ÿ4����˸8��
						rt_kprintf("���ȹرձ���\r\n");
						Alarm("���ȹرձ���","��ֹ����");
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
			CanWrite(0, 0x100,1,&m_actionCmd);   // ����������Ϣ
			if(m_actionCmd>0) // ��ʾ������ִ��
			{
				ALARM_RUN_LED(1);  // ����ָʾ����
			}
			return;
		}	
	}
	m_actionCmd = 0;
}

static u8 cansend_count=0;
// 10ms ���ڷ���
static void CanSend10(void* p)
{
	static u8 count = 0;
	//	{KEY_QINGXITUNBU,	131},  
   if((m_actionCmd==131)&&(m_alarmledYanshi>15)&&(m_alarmledYanshi<140)&&!(g_ZuobianSensor.SuReQiWork))  
	{
		m_actionCmd = 100;	// ����ֹͣ����
//		Alarm("������δ����","��ֹ����");
		m_alarmledState=1;
		m_alarmledCount=6;	// ÿ3����˸6��	
	}
	CanWrite(0, 0x100,1,&m_actionCmd);   // ����������Ϣ

	if(count == 0)
	{
		if(!g_ZuobianqiSetB.HoldOn)   // ���Bδ��������
		{
			CanWrite(0, 0x110,8,(u8*)&g_ZuobianqiSetA.HoldOn);  // ����������Ϣ
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

// CANAnalyzer ��ʼ��
void CanAnalyzerInit(void)
{
	CanOpen(0,CanRxInd);	

	rt_timer_init(&timer10, "act", CanSend10, RT_NULL, RT_TICK_PER_SECOND*100/1000, RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&timer10);
	
//	rt_timer_init(&timer50, "act", CanSend50, RT_NULL, RT_TICK_PER_SECOND*50/1000, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
//	rt_timer_start(&timer50);
}

// ��ʾ��������Ϣ
static void SensorList(void * parameter)
{
	rt_kprintf("��ǰGPIO����:\r\n");
	rt_kprintf("\tPINSEL0=%04X    PINSEL1=%04X    PINSEL2=%04X    IO0DIR=%04X    IO1DIR=%04X    IO0PIN=%04X    IO1PIN=%04X    \r\n",
		PINSEL0,PINSEL1,PINSEL2,IO0DIR,IO1DIR,IO0PIN,IO1PIN);

	rt_kprintf("������������״̬:\r\n");
	rt_kprintf("\t���: %s\r\n",g_ZuobianSensor.DaBian?"Y":"");
	rt_kprintf("\t��ʿС��: %s\r\n",g_ZuobianSensor.ManXiaoBian?"Y":"");
	rt_kprintf("\tŮʿС��: %s\r\n",g_ZuobianSensor.WomenXiaoBian?"Y":"");
	rt_kprintf("\t�������·������: %s\r\n",g_ZuobianSensor.WuWuXiangGuan?"Y":"");	
	rt_kprintf("\tˮ����: %s\r\n",g_ZuobianSensor.ShuiXiangHigh?"Y":"");	
	rt_kprintf("\tˮ���: %s\r\n",g_ZuobianSensor.ShuiXiangLow?"Y":"");	
	rt_kprintf("\t����������: %s\r\n",g_ZuobianSensor.SuReQiWork?"Y":"");
	rt_kprintf("\t��������������: %d\r\n",(u32)g_ZuobianSensor.SuReQiError);
	rt_kprintf("\t����������: %d\r\n",(u32)g_ZuobianSensor.WuWuXiangZhongLiang);
	rt_kprintf("\t������¶�: %d\r\n",(u32)g_ZuobianSensor.ChuFengWenDu);
	rt_kprintf("\tˮ�µ���: %d��\r\n",(u32)g_ZuobianqiSetA.ShuiWenGaoDi);
	rt_kprintf("\tů���¶�: %d��\r\n",(u32)g_ZuobianqiSetA.NuanFengQiangRuo);
	rt_kprintf("\t��ϴǿ��: %d��\r\n",(u32)g_ZuobianqiSetA.QiangXiQiangRuo);
	 
	rt_kprintf("���崫����״̬:\r\n");
	rt_kprintf("\t������ƽ: %s\r\n",g_ChuangTiSensor.BeiBuFangPing?"Y":"");
	rt_kprintf("\t�Ȳ���ƽ: %s\r\n",g_ChuangTiSensor.TuiBuFangPing?"Y":"");
	rt_kprintf("\t��෭ˮƽ��λ: %s\r\n",g_ChuangTiSensor.ZuoFanFangPing?"Y":"");
	rt_kprintf("\t�Ҳ෭ˮƽ��λ: %s\r\n",g_ChuangTiSensor.YouFanFangPing?"Y":"");
	rt_kprintf("\t���м��: %s\r\n",g_ChuangTiSensor.FangJia?"Y":"");	
	rt_kprintf("\t����ȫ��: %s\r\n",g_ChuangTiSensor.BianMenKaiDaoDing?"Y":"");
	rt_kprintf("\t����ȫ�ر�: %s\r\n",g_ChuangTiSensor.BianMenGuanDaoDi?"Y":"");
	rt_kprintf("\t����ƽ�Ƶ���: %s\r\n",g_ChuangTiSensor.BianMenPingYiDaoDing?"Y":"");
	rt_kprintf("\t����ƽ�Ƶ���: %s\r\n",g_ChuangTiSensor.BianMenPingYiDaoDi?"Y":"");
	rt_kprintf("\t��ص�������: %s\r\n",g_ChuangTiSensor.DianLiangBuZu?"Y":"");
	rt_kprintf("\t�����������: %s\r\n",g_ChuangTiSensor.DianJiGuoLiuBao?"Y":"");

	rt_kprintf("\t�е�ϵ�: %s\r\n",g_ChuangTiSensor.TingDian?"Y":"");
	rt_kprintf("\t����Ƿ����: %s\r\n",g_ChuangTiSensor.DianChiZhuOn?"Y":"");
	rt_kprintf("\t�𱳽Ƕ�: %d ��\r\n",g_ChuangTiSensor.QiBeiJiaoDu);
	rt_kprintf("\t�����ȽǶ�: %d ��\r\n",g_ChuangTiSensor.XiaQuTuiJiaoDu);
	rt_kprintf("\t�����ȽǶ�: %d ��\r\n",g_ChuangTiSensor.ShangQuTuiJiaoDu);	
	rt_kprintf("\t����Ƕ�: %d ��\r\n",g_ChuangTiSensor.ZuoFanShenJiaoDu);
	rt_kprintf("\t�ҷ���Ƕ�: %d ��\r\n",g_ChuangTiSensor.YouFanShenJiaoDu);

	rt_kprintf("���A����״̬:\r\n");
	rt_kprintf("\t��ʱ: \t%X",(u32)m_alarmledYanshi);
	rt_kprintf("\tg_actionSend= ");
	rt_kprintf("\t%X",(u32)m_actionCmd);
	rt_kprintf("\r\n");
}
FINSH_FUNCTION_EXPORT(SensorList, "��ʾ��ǰ������״̬");

#ifdef RT_USING_FINSH
// �Զ����
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
FINSH_FUNCTION_EXPORT(AutoSensor, "�Զ���ش�����,����[0]=0��ʾֹͣ�������ʾ��ʼ");
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
	if(g_ZuobianSensor.FunctionWorkNuanFeng)	// ����ǹ�����ů����ģʽ��
	{switch(g_ZuobianqiSetA.NuanFengQiangRuo) 
	{
		case 0: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(0);break;
		case 1: SET_NUANFEN_LOW(1);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(0);break;
		case 2: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(1);SET_NUANFEN_HIGH(0);break;
		case 3: SET_NUANFEN_LOW(0);SET_NUANFEN_MIDDLE(0);SET_NUANFEN_HIGH(1);break;
		default:break;	
	}}	
	if(g_ZuobianSensor.FunctionWorkHouBu
		||g_ZuobianSensor.FunctionWorkQianBu)	// ����ǹ�������ˮ��ϴģʽ��
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
	if(g_ZuobianSensor.ShuiXiangLow)  // ��⵽ˮλ��
	{	ALARM_SHUIWEI_LOW(1);}
	else
	{	ALARM_SHUIWEI_LOW(0);}
	if(g_ZuobianSensor.WuWuXiangZhongLiang>=WU_WU_XIANG_BORDER)  // ��⵽ ��������
	{	ALARM_WUWU_HIGN(1);}
	else
	{	ALARM_WUWU_HIGN(0);}

	LEDSet();
	
	if(m_actionCmd>0) // ��ʾ������ִ��
	{
		m_alarmledYanshi+=1;
		if(m_alarmledYanshi>250)
			m_alarmledYanshi=250;	
	}
	if(m_alarmledState) // ����״̬
	{
//			ALARM_RUN_LED(0);
		m_alarmledTimer+=1;
		if(m_alarmledTimer>16)  // 4 ��Ϊ1 ����2014.8.13 chang
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
    feedWD();    // ι��
}


void AlarmLEDInit(void) //ÿ0.5�봥��һ��
{ 
  rt_timer_init(&LED_timer, "AlarmLED", AlarmLED, RT_NULL, RT_TICK_PER_SECOND*0.25f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
  rt_timer_start(&LED_timer);
  AutoSensor(0);
}


