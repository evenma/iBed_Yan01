#include <Actionthread.h>
#include <rtthread.h>
#include <finsh.h>
#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif
#include "CAN/IncludeCan.h"
#include "CAN/Can.h"
#include "lpc214x.h"
#include <Sensor.h>
#include "CanAnalyzer.h"

// ������ر���
static struct rt_mailbox m_mb;
static rt_uint32_t mbPool[1];
// �¼�
static struct rt_event m_evt;
// ��ʱ�������ڳ�ʱ
static struct rt_timer m_timer={0};


// �����õ���������3.8L/min ����
#define qidianfangpingyanshi 600   // �����ƽ��ʱʱ��10min
#define qidiancefanyanshi     300  // �趨����ҡʱ�෭ʱ���5 min
#define qidianzuoyouyaoyanshi 3600 // ����ҡ��ʱ�趨Ϊ1Сʱ
#define FaMaxValue 0x12C  //AD��ֵ��1.5V 
#define FaMinValue 0x020  //0x12Ϊ��ѹ��ֵ�� AD��ֵ��V 


// ���е�����Ǹߵ�ƽ�����͵�ƽ����
// work ��ʹ�ܣ�dir�Ƿ���1=�����0=����

#define ZuoCQF(on)	    if(on) {IO0SET = BIT22;} else {IO0CLR = BIT22;} 
#define YouCQF(on)	    if(on) {IO0SET = BIT23;} else {IO0CLR = BIT23;} 

#define ZuoXQF(on)	    if(on) {IO1SET = BIT19;} else {IO1CLR = BIT19;} 
#define YouXQF(on)	    if(on) {IO0SET = BIT24;} else {IO0CLR = BIT24;}

#define ChongQiBeng(on)	    if(on) {IO1SET = BIT30;} else {IO1CLR = BIT30;} 
#define ChouQiBeng(on)	    if(on) {IO1SET = BIT29;} else {IO1CLR = BIT29;} 

// ����ʹ��״̬��
u8 m_FanShenEnable = 1;	// 1=������0=������
u8 m_ShuiPing=0;				// ��¼֮ǰ�Ƿ���ˮƽ״̬��1=ˮƽ

extern bool flag_zuofan;
extern bool flag_youfan;
extern bool flag_zuoyou;
extern bool flag_pingtang;
extern unsigned char flag_qidong;


void DoBianmenkai(void)
{
#if !DONG_ZUO_DEBUG
	if(g_sensor.ShuiPing)
#endif
	{
		TuiBei(1, 0);  // �����Ƹ˲����Ʊ���� 
		while(!g_ToiletSensor.QianBianMenZuoDaKai)
		{
			//	�����⵽�а�����ͣ
			if(WaitTimeout(1*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<BianMenQuanGuan)))
			{
				TuiBei(0, 0); // �ر�
				return;
			}
		}
		TuiBei(0, 0); // �ر�
	}


}


// ���� 
void QiDianFanShen(u8 work,u8 dir)
{
	if(work)
	{
		if(dir)  // �ҷ�������߳������ұ߷���
		{
			ZuoCQF(1);
			YouCQF(0);
			ZuoXQF(0);
			YouXQF(1);
		}
		else      // �������ұ߳�������߷���
		{
			ZuoCQF(0);
			YouCQF(1);
			ZuoXQF(1);
			YouXQF(0);				
		}
		ChongQiBeng(1);
		ChouQiBeng(1);
	}
	else
	{
		ChongQiBeng(0);
		ChouQiBeng(0);
		ZuoCQF(0);
		YouCQF(0);
		ZuoXQF(0);
		YouXQF(0);				
	}
}
// �رճ����ã��رս�������й����
void DoStopQiDian(void)
{
	QiDianFanShen(0, 0);
}

// ��ƽ����������ȫ���������
static void DoFangPing(void)
{
    rt_kprintf("ƽ�� \r\n");
	ZuoCQF(0);
	YouCQF(0);
	ZuoXQF(1);
	YouXQF(1);	
	ChouQiBeng(1);
//   �����⵽û����ѹ����һ��ʱ���ͣ��
	if((ADG_sensor.ZuoADValue<FaMinValue)&&(ADG_sensor.YouADValue<FaMinValue))
	{
	    rt_kprintf("��⵽����ѹ \r\n");
		WaitTimeout(10*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<QiDianPingTang));
	    flag_pingtang = 0;		
		DoStopQiDian();
		return;		
	}
	rt_kprintf("��ʱ10min\r\n");
	//  �����⵽�а�����ͣ
	if(WaitTimeout(qidianfangpingyanshi*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<QiDianPingTang)))
	{
		DoStopQiDian();
		return;
	}
	flag_pingtang = 0;
	DoStopQiDian();
}

// ��
static void DoZuoFan(void)
{
    rt_kprintf("���� \r\n");
	QiDianFanShen(1,1);
	while(!(ADG_sensor.ZuoADValue>FaMaxValue))
	{
		//  �����⵽�а�����ͣ
		if(WaitTimeout(5*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<QiDianZuoFan)|(1<<QiDianZuoYouYao)))
		{
			DoStopQiDian();
			return;
		}
	}
	flag_zuofan= 0;
	DoStopQiDian();	
}

// �ҷ�
static void DoYouFan(void)
{
    rt_kprintf("�ҷ��� \r\n");
	QiDianFanShen(1,0);
	while(!(ADG_sensor.YouADValue>FaMaxValue))
	{
		//	�����⵽�а�����ͣ
		if(WaitTimeout(5*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<QiDianYouFan)))
		{
			DoStopQiDian();
			return;
		}
	}
	flag_youfan = 0;
	DoStopQiDian(); 
}

// ����ҡ ������ң�ÿСʱ�Զ�����һ��,һ������12Сʱ
void DoZuoYouYao(void)
{
unsigned char i;
  rt_kprintf("����ҡ \r\n");
  for(i=0;i<12;i++)
  {
	rt_kprintf("�� %d ������ѭ���෭��\r\n",(i+1)); 
	
	DoZuoFan();  // 6min
	//	�����⵽�а�����ͣ,���û����ʱһ��ʱ��5min
	if(WaitTimeout(qidiancefanyanshi*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<QiDianZuoYouYao)))
	{
		DoStopQiDian();
		return;
	}	
	DoFangPing(); // 10min
	//	�����⵽�а�����ͣ���û����ʱһ��ʱ��10min
	if(WaitTimeout(qidianfangpingyanshi*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<QiDianZuoYouYao)))
	{
		DoStopQiDian();
		return;
	}	
	
	DoYouFan();  // 6min
	//	�����⵽�а�����ͣ,���û����ʱһ��ʱ��5min
	if(WaitTimeout(qidiancefanyanshi*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<QiDianZuoYouYao)))
	{
		DoStopQiDian();
		return;
	}	
	DoFangPing(); // 10min
	//	�����⵽�а�����ͣ
	if(WaitTimeout(qidianfangpingyanshi*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<QiDianZuoYouYao)))
	{
		DoStopQiDian();
		return;
	}	

  }

	DoStopQiDian(); 


}
//---------------------����Ϊ��������----------------//





//---------------------����Ϊ�Ƹ˿���----------------//
// ���е�����Ǹߵ�ƽ�����͵�ƽ����
// work ��ʹ�ܣ�dir�Ƿ���1=�����0=����

// ���� M1  P0.19 up; P0.20 Down;
void BeiBu(u8 work,u8 dir)
{
	if(work)
	{
		if(dir)
		{
			IO0SET = BIT19;
			IO0CLR = BIT20;
		}
		else
		{
			IO0CLR = BIT19;
			IO0SET= BIT20;
		}
	}
	else
	{
		IO0CLR=BIT19|BIT20;
	}
}

// �Ȳ� M2   P0.18 up; P0.24 Down;
void TuiBu(u8 work,u8 dir)
{	
	if(work)
	{
		if(dir)
		{
			IO0SET = BIT18;
			IO0CLR = BIT24;
		}
		else
		{
			IO0CLR = BIT18;
			IO0SET= BIT24;
		}
	}
	else
	{
		IO0CLR=BIT18|BIT24;
	}

}

// ���� M3   P0.17 up; P0.23 Down;
void ZuoFanShen(u8 work,u8 dir)
{
	if(work)
	{
		if(dir)
		{
			IO0SET = BIT17;
			IO0CLR = BIT23;
		}
		else
		{
			IO0CLR = BIT17;
			IO0SET= BIT23;
		}
	}
	else
	{
		IO0CLR=BIT17|BIT23;
	}


}

// �ҷ��� M4  P0.16 up; P0.22 Down;
void YouFanShen(u8 work,u8 dir)
{
	if(work)
	{
		if(dir)
		{
			IO0SET = BIT16;
			IO0CLR = BIT22;
		}
		else
		{
			IO0CLR = BIT16;
			IO0SET= BIT22;
		}
	}
	else
	{
		IO0CLR=BIT16|BIT22;
	}

}

// �Ʊ� M5  ���ڱ��Ŵ��Ƹ�P0.16 work ;P0.17 dir
void TuiBei(u8 work,u8 dir)
{
	if(work)
	{
		if(dir)
		{
			IO0SET = BIT17;
		}
		else
		{
			IO0CLR = BIT17;
		}
		IO0SET = BIT16;
	}
	else
	{
		IO0CLR = BIT16|BIT17;
	}
}

void DoStop(void)
{
	BeiBu(0, 0);
//	ChuangTi(0, 0);
//	FanShen(0, 0);
	TuiBu(0, 0);
	ZuoFanShen(0,0);
	YouFanShen(0,0);
//	TuiBei(0, 0);
//	QiDianFanShen(0,0);		
}

void Doautotest(void)
{
	unsigned char i;
	rt_kprintf("�Զ����Կ�ʼ\r\n");
	if(!flag_zuoyou)
		return; 
	for(i=0;i<10;i++)
	{
// step1 : ��ƽ
	rt_kprintf("step1:��ʼ��ƽ\r\n");
	TuiBu(1, 1);
	BeiBu(1,0);
	DelayXms(30);
	g_sensor.TuiBuFangPing = (0==(IO0PIN&BIT24));
	g_sensor.BeiBuFangPing = (0==(IO0PIN&BIT4));	
	if((!ZYG_sensor.ZuoShuiPing)||(!ZYG_sensor.YouShuiPing)||(!g_sensor.BeiBuFangPing)||(!g_sensor.TuiBuFangPing)) // �ȷ�ƽ
	{
		if(ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
		{
			BeiBu(1, 0);
			TuiBu(1, 1);
			//ChuangTi(1, 0);
			while((!g_sensor.TuiBuFangPing)||(!g_sensor.BeiBuFangPing)) // �ȵ�λ����				
			{
				if(WaitTimeout(2*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
				{
					flag_zuoyou = 0;
					DoStop();
					return;
				}
			}
		}
		else 
		{
			ZuoFanShen(1,0);
			YouFanShen(1,0);
			while((!ZYG_sensor.ZuoShuiPing)||(!ZYG_sensor.YouShuiPing)) // �ȵ�λ����				
			{if(WaitTimeout(2*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
			{
				flag_zuoyou = 0;
				DoStop();
				return;
			}}

		}
	}
	DoStop();
	rt_kprintf("step1:��ƽ����\r\n");
	if(WaitTimeout(2*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
	{
		flag_zuoyou = 0;
		DoStop();
		return;
	}
// step2: ��	����
		rt_kprintf("step2:��ʼ����\r\n");
		if(ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
		{
			BeiBu(1, 1);
		}
		if(!flag_zuoyou)
			return;
		if(WaitTimeout(40*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
		{
			flag_zuoyou = 0;
			DoStop();
			return;
		}
		DoStop();		
		rt_kprintf("step2:��������\r\n");	
// step3: �Ƚ�		
		rt_kprintf("step3:��ʼ�Ƚ�\r\n");
		if(ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
		{
			TuiBu(1, 0);
		}
		if(!flag_zuoyou)
			return;
		if(WaitTimeout(40*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
		{
			flag_zuoyou = 0;
			DoStop();
			return;
		}
		DoStop();
		rt_kprintf("step3:�Ƚ�����\r\n");	
// step4:����
		rt_kprintf("step4:��ʼ����\r\n");
		if(ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
		{
			BeiBu(1, 0);
			if(!flag_zuoyou)
				return;
			if(WaitTimeout(40*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
			{
				flag_zuoyou = 0;
				DoStop();
				return;
			}
			while(!g_sensor.BeiBuFangPing); // �ȵ�λ����	
		}
		DoStop();		
		rt_kprintf("step4:��������\r\n");	
// step5:����
		rt_kprintf("step5:��ʼ����\r\n");
		if(ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
		{
			TuiBu(1, 1);
			if(!flag_zuoyou)
				return;
			if(WaitTimeout(40*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
			{
				flag_zuoyou = 0;
				DoStop();
				return;
			}
			while(!g_sensor.TuiBuFangPing); // �ȵ�λ����	
		}
		DoStop();
		rt_kprintf("step5:��������\r\n");	
// step6: ����
		rt_kprintf("step6:��ʼ����\r\n");
		TuiBu(1, 1);
		BeiBu(1,0);
		DelayXms(30);
		g_sensor.TuiBuFangPing = (0==(IO0PIN&BIT24));
		g_sensor.BeiBuFangPing = (0==(IO0PIN&BIT4));	
		if(g_sensor.TuiBuFangPing&&g_sensor.BeiBuFangPing)
		{
			ZuoFanShen(1,1);		
		}
		if(!flag_zuoyou)
			return;
		if(WaitTimeout(30*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
		{
			flag_zuoyou = 0;
			DoStop();
			return;
		}	
		ZuoFanShen(0,0);
		rt_kprintf("step6:��������\r\n");	
// step7: ��ƽ
		rt_kprintf("step7:��ʼ���ƽ\r\n");
		ZuoFanShen(1,0);	
		if(!flag_zuoyou)
			return;
		if(WaitTimeout(30*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
		{
			flag_zuoyou = 0;
			DoStop();
			return;
		}	
		while(!ZYG_sensor.ZuoShuiPing); // �ȵ�λ����		
		DoStop();
		rt_kprintf("step7:��ƽ����\r\n");	
// step8: �ҷ���
		rt_kprintf("step8:��ʼ�ҷ���\r\n");
		TuiBu(1, 1);
		BeiBu(1,0);
		DelayXms(30);
		g_sensor.TuiBuFangPing = (0==(IO0PIN&BIT24));
		g_sensor.BeiBuFangPing = (0==(IO0PIN&BIT4));	
		if(g_sensor.TuiBuFangPing&&g_sensor.BeiBuFangPing)
		{
			YouFanShen(1,1);		
		}
		if(!flag_zuoyou)
			return;
		if(WaitTimeout(30*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
		{
			flag_zuoyou = 0;
			DoStop();
			return;
		}	
		YouFanShen(0,0);
		rt_kprintf("step8:�ҷ�������\r\n"); 
// step9: ��ƽ
		rt_kprintf("step9:��ʼ�ҷ�ƽ\r\n");
		YouFanShen(1,0);	
		if(!flag_zuoyou)
			return;
		if(WaitTimeout(30*RT_TICK_PER_SECOND, (1<<ActionCmdNone)|(1<<FanShenZuoYouYao)))
		{
			flag_zuoyou = 0;
			DoStop();
			return;
		}	
		while(!ZYG_sensor.YouShuiPing); // �ȵ�λ����		
		DoStop();
		rt_kprintf("step9:��ƽ����\r\n");	
	}
	flag_zuoyou = 0;	
  	DoStop(); 	 
	rt_kprintf("�Զ������Խ���\r\n");	
}

// ��ƽ����������Ѿ���ƽ�򷵻�true�����򷵻�false
BOOL FangPing(void)
{
	if(g_sensor.ShuiPing&&ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
	{
		return TRUE;
	}
	else
	{
#if 0
		if(!g_sensor.TuiBuFangPing)
		{
			TuiBu(1,1);
		}
		if(!g_sensor.BeiBuFangPing)
		{
			BeiBu(1,0);
		}
		if(g_sensor.BeiBuFangPing&&g_sensor.TuiBuFangPing)
		{
			if(g_sensor.ZuoQing)
			{
				FanShen(1,1);
			}
			else
			{
				FanShen(1,0);
			}
		}
#endif
		return FALSE;
	}
}

// ������ʱ
static void ActionTimeout(void* parameter)
{
		BeiBu(0, 0);
	//	ChuangTi(0, 0);
	//	FanShen(0, 0);
		TuiBu(0, 0);
		ZuoFanShen(0,0);
		YouFanShen(0,0);
	//	TuiBei(0, 0);
	//	QiDianFanShen(0,0); 

}

// ���Ź���ʼ��
// 2880000 ,1��
void WDInit(void)
{
//	WDTC = 0x00DBBA00;	// ���ÿ��Ź���ʱ���Ĺ̶�װ��ֵ : 5 ��
	WDTC = 0x04F1A000;	// ���ÿ��Ź���ʱ���Ĺ̶�װ��ֵ : 30 ��
	WDMOD= 0X03;   // ģʽ�趨
	feedWD();
}

void feedWD(void) // ι��
{
	WDFEED =0XAA;
	WDFEED =0X55;
}

u32 WDtimers(void)
{
	return WDTV; //���Ź���ʱ���ĵ�ǰֵ
}


// �ж���Ͱ�����Ƿ�ر�,�ر�̬����1   �����ҷ��й�
static int IsToiletClose()
{
	if(g_ToiletSensor.QianBianMenZuoGuanBi&&g_ToiletSensor.QianBianMenYouGuanBi&&g_ToiletSensor.HouBianMenGuanBi)
	{
		return 1;
	}
	return 1;		//��ʱ���ԣ� �ر���Ͱ���
}


// ��������ģʽ��1��ʾ����ģʽ
#define DONG_ZUO_DEBUG	0

// ��ʼ��������
void ActionStartCmd(ActionCmd cmd)
{
#if 0
	if(cmd != ActionCmdNone)
	{
		rt_kprintf("ActionStartCmd= %d \r\n",cmd);
	}
#endif
	if(g_sensor.ShuiPing&&!m_ShuiPing)	// ���䷢���ˣ���ˮƽ���ˮƽ��
	{
		m_FanShenEnable = 0;	       // ���ڻ�е�ṹ�ڶ�����Ĵ������仯
	}
	m_FanShenEnable=1;
	switch(cmd)
	{
	case TuiSheng	:
		DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.ShuiPing&&ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
#endif
		{
			TuiBu(1, 1);
		}
		RestartTimer();
		break;
	case TuiJiang	:
		DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.ShuiPing&&!g_sensor.TuiBuDaoDi&&ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
#endif
		{
			TuiBu(1, 0);
		}
		RestartTimer();
		break;
	case PingTang	:
		DoStop();
		if(g_sensor.ShuiPing&&ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
		{
			BeiBu(1, 0);
			TuiBu(1, 1);
		//	ChuangTi(1, 0);
		}
		else 
		{
			ZuoFanShen(1,0);
			YouFanShen(1,0);
		}
		
		RestartTimer();
		break;	

	case QiZuo	:
		DoStop();
		if(FangPing())
		{
			BeiBu(1, 1);
			TuiBu(1, 0);
		}
		RestartTimer();
		break;
	case ChuiTiYangWo	:
		DoStop();
	//	ChuangTi(1, 1);
		RestartTimer();
		break;
	case XinZangTangWei	:
		DoStop();
		if(FangPing())
		{
			BeiBu(1, 1);
			TuiBu(1, 0);
	//		ChuangTi(1, 1);
		}
		RestartTimer();
		break;
	case BeiSheng	:
		DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.ShuiPing&&ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
#endif
		{
			BeiBu(1, 1);
		}
		RestartTimer();
		break;
	case BeiJiang:
		DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.ShuiPing&&ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
#endif
		{
			BeiBu(1,0);
		}	
		RestartTimer();
		break;

case BianMenQuanGuan	:  // add mayb 2014.3.18.
	DoStop();
#if !DONG_ZUO_DEBUG
	if(g_sensor.ShuiPing)
#endif
	{
	  //  if((g_ToiletSensor.QianBianMenYouGuanBi==1)&&(g_ToiletSensor.HouBianMenGuanBi==1))
				//	     TuiBei(1, 1);  // �����Ƹ˲����Ʊ���� 
	}
	RestartTimer();
	break;

	case ZuoFan:	// todo
#if !DONG_ZUO_DEBUG
		if(ZYG_sensor.YouShuiPing)
#endif			
	{
		TuiBu(1, 1);
		BeiBu(1,0);
		DelayXms(30);
		g_sensor.TuiBuFangPing = (0==(IO0PIN&BIT24));
		g_sensor.BeiBuFangPing = (0==(IO0PIN&BIT4));
		
		if(	IsToiletClose()&&
			m_FanShenEnable&&
			g_sensor.BeiBuFangPing&&
			g_sensor.TuiBuFangPing)
		{
			ZuoFanShen(1,1);
		}
		else
		{
			ZuoFanShen(0,0);
		}
	}
	else
	{
		DoStop();
	}
		RestartTimer();
		break;
		
	case YouFan:	// todo
#if !DONG_ZUO_DEBUG
	if(ZYG_sensor.ZuoShuiPing)
#endif			
	{
	TuiBu(1, 1);
	BeiBu(1,0);
	DelayXms(30);
	g_sensor.TuiBuFangPing = (0==(IO0PIN&BIT24));
	g_sensor.BeiBuFangPing = (0==(IO0PIN&BIT4));
	

		if( IsToiletClose()
			&&m_FanShenEnable
			&&g_sensor.BeiBuFangPing
			&&g_sensor.TuiBuFangPing)
		{
			YouFanShen(1,1);
		}
		else
		{
			YouFanShen(0,0);
		}
	}
	else
	{
		DoStop();
	}
		RestartTimer();
		break;

	case PingTangFanShen:
		DoStop();
		if(g_sensor.ShuiPing&&ZYG_sensor.ZuoShuiPing&&ZYG_sensor.YouShuiPing)
		{
			BeiBu(1, 0);
			TuiBu(1, 1);
		//	ChuangTi(1, 0);
		}
		else 
		{
			ZuoFanShen(1,0);
			YouFanShen(1,0);
		}
		
		RestartTimer();
		break;		
		
	case ActionCmdNone:
		m_FanShenEnable = 1;
		flag_qidong=0;
//		ActionTimeout(NULL);
//		RestartTimer();
		break;
	default:
		DbgPrintf("������Ϣ����������\r\n");
		rt_mb_send(&m_mb, cmd);
		break;
//		case ActionCmdNone:
//			m_FanShenEnable = 1;
//		rt_timer_stop(&m_timer);
//		DoStop();			
//			ActionTimeout(NULL);
//			RestartTimer();
//			ActionStopCmd(ActionCmdNone);
//			break;
	}
	m_ShuiPing = g_sensor.ShuiPing;
}
FINSH_FUNCTION_EXPORT(ActionStartCmd,"��ʼ����[cmd]");

// ֹͣ��������
void ActionStopCmd(ActionCmd cmd)
{
//	rt_timer_stop(&m_timer);
//	DoStop();
#if 1
		if(cmd != ActionCmdNone)
		{
			rt_kprintf("ActionStopCmd= %d \r\n",cmd);
		}
#endif
	rt_event_send(&m_evt, 1<<cmd);
}

FINSH_FUNCTION_EXPORT(ActionStopCmd, "ֹͣ����");


// Ӳ����ʼ��
static void ActionHwInit(void)
{
	// PA8,9 ��Ϊ����
	PINSEL0 &= ~(BIT16|BIT17);
	PINSEL0 &= ~(BIT18|BIT19);	
	
	DoStop();
	ActionTimeout(RT_NULL);
}

static void ActionInit(void)
{

	// Ӳ����ʼ��
	ActionHwInit();
	// ������ʼ��

	// ��ʼ������
	rt_mb_init(&m_mb, "act mb", mbPool, sizeof(mbPool)/sizeof(rt_uint32_t), RT_IPC_FLAG_FIFO);
	// ��ʼ���¼�
	rt_event_init(&m_evt, "act evt", RT_IPC_FLAG_FIFO);
	// ��ʼ����ʱ��
	rt_timer_init(&m_timer, "act", ActionTimeout, RT_NULL, 0.1f*RT_TICK_PER_SECOND, RT_TIMER_FLAG_ONE_SHOT);
//	rt_timer_init(&m_timer , "act", ActionTimeout, RT_NULL, RT_TICK_PER_SECOND*0.1f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_HARD_TIMER);		// �����Ե���

}


static void RestartTimer(void)
{
	rt_timer_stop(&m_timer);
	rt_timer_start(&m_timer);
}

// �ȴ���������ط�0��ʾ�ú����������ⲿֹͣ������������µķ���
static int WaitTimeout(rt_int32_t timeout,rt_uint32_t set)
{
	rt_uint32_t evt;
	if(rt_event_recv(&m_evt,set , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, timeout, &evt) == RT_EOK)
	{
		rt_kprintf("�ֶ��˳�\r\n");
		return -1;		// ���յ��˳��ź�
	}
	else
	{
		return 0;		// ������ʱ
	}
}

// ���յ�CAN�ص�����
extern rt_err_t CanRxInd(rt_device_t dev, rt_size_t size);
extern void CanAnalyzerInit(void);


// �����������
void ActionThreadEntry(void* parameter)
{
	rt_uint32_t cmd,evt;
	
	ActionInit();

	// �� CAN1 �豸
	CanAnalyzerInit();
	
	while(1)
	{
		// �ȴ������ʼ�����
		rt_mb_recv(&m_mb, &cmd, RT_WAITING_FOREVER);	// �ȴ���������
		DoStop();
		rt_kprintf("���յ��������� %d\r\n",(int)cmd);
		rt_event_recv(&m_evt,0xffffffff , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, RT_WAITING_NO, &evt);	// ���¼�
		switch(cmd)
		{
			case BianMenQuanKai :	 // add mayb 2014.3.18.   // Ϊ�������Զ������������
				DoStop();
				DoBianmenkai();
				break;
			case QiDianZuoFan:
				DoStopQiDian();
				DoZuoFan();
				break;
			case QiDianYouFan:
				DoStopQiDian();
				DoYouFan();
				break;
			case QiDianPingTang:		// add mayb
				DoStopQiDian();
				DoFangPing();
				break;
			case QiDianZuoYouYao:
				DoStopQiDian();
				DoZuoYouYao();
				break;

			default:
				DoStopQiDian();
				rt_kprintf("û���ҵ���Ӧ�Ķ���ָ��\r\n");
				break;
		}
		DoStop();
		rt_mb_recv(&m_mb, &cmd, RT_WAITING_NO);	// ������
		rt_event_recv(&m_evt,0xffffffff , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, RT_WAITING_NO, &evt);	// ���¼�
		rt_kprintf("��������\r\n");
	}
}
