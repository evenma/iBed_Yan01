#include "Sensor.h"
#include "Adc.h"
#ifdef RT_USING_FINSH
#include "CanAnalyzer.h"
#include "CAN/IncludeCan.h"
#include <Actionthread.h>

#define DIAN_YUAN_CHANGE_TIME   360000  // 36000 // 1Сʱ����һ�ι���
#define CHONG_DIAN_TIME_END    288000  // 8Сʱ��ֹͣ���
// ������״̬ȫ�ֱ���
volatile s_Sensor g_sensor = {0};		
 volatile ADS_Sensor g_AD_sensor = {0};
volatile s_MT_work g_MT_work = {0};
static u8 preLegSensor=0;
volatile s_ZuobianqiSensor g_ZuobianqiSensor = {0};
static u8 MT_All_KongXian=0;	// ���е������״̬

static u16 WDcount=0;

// ����һ�δ�����״̬�������˲�
//static s_Sensor m_lastSensorStatus;
#define DIAN_LIANG_MIN   639 // �趨�������ֵ21V    A=30.751*VCC
#define DIAN_YA_MIN      615  // �趨��Դ��ѹ���ֵ20V    A=30.751*VCC
#define DIAN_YA_MAX      700
static u32 chongdiantimecount=0;  // ���ʱ�����1 ��100mS
u32 g_diancidianliangAd=0;	// ��ص�����ȡ
u32 g_powerdianya=0;		// ���ص�Դ 24V�����ѹ��ȡ

static u8 m_ShiDianStatus=0;	// ǰһ���е��ѹ���
static u8 c_ShiDianStatus=0;	// ��ǰ�е��ѹ���
static u32 powerchangetimecount=0;	// ��Դ�л�ʱ�����  1Сʱ�л�һ������ȡ����
static u8 dianlianglvbocount=0;	//��������˲� 10S = 100
static u8 shidianjiancelvbo=0;    // �е����˲�

u8 m_TuiBuStatus=0;		// �Ȳ��Ƹ�֮ǰ��ˮƽ״̬
u8 c_TuiBuStatus=0;     // �Ȳ��Ƹ˵�ǰ��ˮƽ״̬

u8 m_bei_hall=0;  // ���������ź�ǰһ��״ֵ̬
u16 beibumapancount=0; // �����г�λ�������ۼ�ֵ
u8 m_tui_hall=0;  // �Ȳ������ź�ǰһ��״ֵ̬
u16 tuibumapancount=0; // �Ȳ��г�λ�������ۼ�ֵ
u8 m_zuofan_hall=0;  // �󷭻����ź�ǰһ��״ֵ̬
u16 zuofanmapancount=0; // ���г�λ�������ۼ�ֵ
u8 m_youfan_hall=0;  // �ҷ������ź�ǰһ��״ֵ̬
u16 youfanmapancount=0; // �ҷ��г�λ�������ۼ�ֵ

u16 m_beibu_max = BEI_MAPAN_MAX;
u16 m_beibu_middle = BEI_MAPAN_MIDDLE;
u16 m_tuibu_max = TUI_MAPAN_MAX;
u16 m_tuibu_middle = TUI_MAPAN_SHUI_PING;
u16 m_fanshen_max = FANSHEN_MAPAN_MAX;
u16 m_wucha = WUCHAJINGDU;

FINSH_VAR_EXPORT(m_beibu_max,finsh_type_short,"��������");
FINSH_VAR_EXPORT(m_beibu_middle,finsh_type_short,"�����м�");
FINSH_VAR_EXPORT(m_tuibu_max,finsh_type_short,"�Ȳ�����");
FINSH_VAR_EXPORT(m_tuibu_middle,finsh_type_short,"�Ȳ��м�");
FINSH_VAR_EXPORT(m_fanshen_max,finsh_type_short,"������");
FINSH_VAR_EXPORT(m_wucha,finsh_type_short,"���");

void GetBeiBuWeiZhi()
{
	rt_kprintf("\tbeibumapancount=%d\r\n",beibumapancount);	
	rt_kprintf("\tm_beibu_max=%d\r\n",m_beibu_max);
	rt_kprintf("\tm_beibu_middle=%d\r\n",m_beibu_middle);
}
FINSH_FUNCTION_EXPORT(GetBeiBuWeiZhi,"����������ȡ");

void SetBeiBuWeiZhi(u32 max,u32 middle)
{
	m_beibu_max = (u16)max;
	m_beibu_middle = (u16)middle;
	GetBeiBuWeiZhi();
}
FINSH_FUNCTION_EXPORT(SetBeiBuWeiZhi,"������������");

void GetTuiBuWeiZhi()
{
	rt_kprintf("\ttuibumapancount=%d\r\n",tuibumapancount);	
	rt_kprintf("\tm_tuibu_max=%d\r\n",m_tuibu_max);
	rt_kprintf("\tm_tuibu_middle=%d\r\n",m_tuibu_middle);
}
FINSH_FUNCTION_EXPORT(GetTuiBuWeiZhi,"�Ȳ�������ȡ");

void SetTuiBuWeiZhi(u32 max,u32 middle)
{
	m_tuibu_max = (u16)max;
	m_tuibu_middle = (u16)middle;
	GetTuiBuWeiZhi();
}
FINSH_FUNCTION_EXPORT(SetTuiBuWeiZhi,"�Ȳ���������");

void GetFanShenWeiZhi()
{
	rt_kprintf("\tzuofanmapancount=%d\r\n",zuofanmapancount);
	rt_kprintf("\tyoufanmapancount=%d\r\n",youfanmapancount);	
	rt_kprintf("\tm_zuofan_max=%d\r\n",m_fanshen_max);
}
FINSH_FUNCTION_EXPORT(GetFanShenWeiZhi,"���������ȡ");

void SetFanShenWeiZhi(u32 max)
{
	m_fanshen_max = (u16)max;
	GetFanShenWeiZhi();
}
FINSH_FUNCTION_EXPORT(SetFanShenWeiZhi,"�����������");


void GetWuCha()
{
	rt_kprintf("\tm_wucha=%d\r\n",m_wucha);
}
FINSH_FUNCTION_EXPORT(GetWuCha,"��������ȡ");

void SetWuCha(u32 jingdu)
{
	m_wucha = (u16)jingdu;
	GetWuCha();
}
FINSH_FUNCTION_EXPORT(SetWuCha,"����������");


// ��ʾ��������Ϣ
/*
static void SensorList(void * parameter)
{
	rt_kprintf("��ǰGPIO����:\r\n");
	rt_kprintf("\tPINSEL0=%04X    PINSEL1=%04X    PINSEL2=%04X    IO0DIR=%04X    IO1DIR=%04X    IO0PIN=%04X    IO1PIN=%04X    \r\n",
		PINSEL0,PINSEL1,PINSEL2,IO0DIR,IO1DIR,IO0PIN,IO1PIN);
	rt_kprintf("��ǰCAN����:\r\n");
//	rt_kprintf("\tCAN0MOD=%04X	CAN0ICR=%04X	CAN0SR=%04X\r\n",CANMOD(0).Word,CANICR(0).Word,CANSR(0).Word);
	
	rt_kprintf("��ǰ������״̬:\r\n");
	rt_kprintf("\tg_sensor= %X %X\r\n",(u32)(*((u8*)(&g_sensor))),(u32)(*(((u8*)(&g_sensor))+1)));

	rt_kprintf("\t������ƽ: %s\r\n",g_sensor.BeiBuFangPing?"Y":"");
	rt_kprintf("\t�Ȳ���ƽ: %s\r\n",g_sensor.TuiBuFangPing?"Y":"");
	rt_kprintf("\t��෭ˮƽ��λ: %s\r\n",g_sensor.ZuoFanFangPing?"Y":"");
	rt_kprintf("\t�Ҳ෭ˮƽ��λ: %s\r\n",g_sensor.YouFanFangPing?"Y":"");
	rt_kprintf("\t���м��: %s\r\n",g_sensor.FangJia?"Y":"");
	
	rt_kprintf("\t����ȫ��: %s\r\n",g_sensor.BianMenKaiDaoDing?"Y":"");
	rt_kprintf("\t����ȫ�ر�: %s\r\n",g_sensor.BianMenGuanDaoDi?"Y":"");
	rt_kprintf("\t����ƽ�Ƶ���: %s\r\n",g_sensor.BianMenPingYiDaoDing?"Y":"");
	rt_kprintf("\t����ƽ�Ƶ���: %s\r\n",g_sensor.BianMenPingYiDaoDi?"Y":"");

	rt_kprintf("\t��ص�������: %s\r\n",g_sensor.DianLiangBuZu?"Y":"");
	rt_kprintf("\t�����������: %s\r\n",g_sensor.DianJiGuoLiuBao?"Y":"");

	rt_kprintf("\t�е�ϵ�: %s\r\n",g_sensor.TingDian?"Y":"");
	rt_kprintf("\t12V���ص�Դ: %s\r\n",c_ShiDianStatus?"Y":"");
		
	rt_kprintf("\t����Ƿ����: %s\r\n",g_sensor.DianChiZhuOn?"Y":"");

	rt_kprintf("\t�Զ���ϴ: %s\r\n",g_ZuobianqiSensor.FunctionWorkAuto?"Y":"");

	rt_kprintf("\t�𱳽Ƕ�: %d ��\r\n",g_sensor.QiBeiJiaoDu);
	rt_kprintf("\t�����ȽǶ�: %d ��\r\n",g_sensor.XiaQuTuiJiaoDu);
	rt_kprintf("\t�����ȽǶ�: %d ��\r\n",g_sensor.ShangQuTuiJiaoDu);	
	rt_kprintf("\t����Ƕ�: %d ��\r\n",g_sensor.ZuoFanShenJiaoDu);
	rt_kprintf("\t�ҷ���Ƕ�: %d ��\r\n",g_sensor.YouFanShenJiaoDu);

	rt_kprintf("\t��������: %d \r\n",beibumapancount);
	rt_kprintf("\t�Ȳ�����: %d \r\n",tuibumapancount);
	rt_kprintf("\t��������: %d \r\n",zuofanmapancount);
	rt_kprintf("\t�ҷ�������: %d \r\n",youfanmapancount);

	rt_kprintf("\t������ѹ: %d V \r\n",(g_diancidianliangAd/31));

}
*/
static void SensorList(void * parameter)
{
	rt_kprintf("Sensor Status:\r\n");
	rt_kprintf("\tg_sensor= %X %X\r\n",(u32)(*((u8*)(&g_sensor))),(u32)(*(((u8*)(&g_sensor))+1)));

	rt_kprintf("\tBACK origin: %s\r\n",g_sensor.BeiBuFangPing?"Y":"");
	rt_kprintf("\tLEG origin: %s\r\n",g_sensor.TuiBuFangPing?"Y":"");
	rt_kprintf("\tLeft Turn origin: %s\r\n",g_sensor.ZuoFanFangPing?"Y":"");
	rt_kprintf("\tRight Turn origin: %s\r\n",g_sensor.YouFanFangPing?"Y":"");
	rt_kprintf("\tFangjia: %s\r\n",g_sensor.FangJia?"Y":"");
	
	rt_kprintf("\tOpen Toilet: %s\r\n",g_sensor.BianMenKaiDaoDing?"Y":"");
	rt_kprintf("\tClose Toilet: %s\r\n",g_sensor.BianMenGuanDaoDi?"Y":"");
	rt_kprintf("\tMove Cover end: %s\r\n",g_sensor.BianMenPingYiDaoDing?"Y":"");
	rt_kprintf("\tMove Cover origin: %s\r\n",g_sensor.BianMenPingYiDaoDi?"Y":"");

	rt_kprintf("\tBatt Low: %s\r\n",g_sensor.DianLiangBuZu?"Y":"");
	rt_kprintf("\tOver Current: %s\r\n",g_sensor.DianJiGuoLiuBao?"Y":"");

	rt_kprintf("\tCut AC Power: %s\r\n",g_sensor.TingDian?"Y":"");
	rt_kprintf("\t12V DC Power: %s\r\n",c_ShiDianStatus?"Y":"");
		
	rt_kprintf("\tBatt On: %s\r\n",g_sensor.DianChiZhuOn?"Y":"");

	rt_kprintf("\tAuto Clean: %s\r\n",g_ZuobianqiSensor.FunctionWorkAuto?"Y":"");

	rt_kprintf("\tBack Angel: %d ��\r\n",g_sensor.QiBeiJiaoDu);
	rt_kprintf("\tDown-Leg Angel: %d ��\r\n",g_sensor.XiaQuTuiJiaoDu);
	rt_kprintf("\tUp-Leg Angel: %d ��\r\n",g_sensor.ShangQuTuiJiaoDu);	

	rt_kprintf("\tBack encode: %d \r\n",beibumapancount);
	rt_kprintf("\tLeg encode: %d \r\n",tuibumapancount);
	rt_kprintf("\tDining time: %d \r\n",g_MT_work.diningCnt);
	

	rt_kprintf("\tBatt Voltage: %d V \r\n",(g_diancidianliangAd/31));

}
FINSH_FUNCTION_EXPORT(SensorList, "Display Sensor Status");

// �Զ����
static void AutoSensor(u32 start)
{
	static u8 init = 0;
	static struct rt_timer t;
	if(init ==0)
	{
		rt_timer_init(&t, "monitor", SensorList, NULL, RT_TICK_PER_SECOND, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
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

// ��ʱ�������������Ե��ô�������⺯��
static struct rt_timer sensorTimer;
static u16 DianJiDianLiuAD1=0;
static u16 DianJiDianLiuAD2=0;
static u16 DianJiDianLiuAD3=0;
#define Timotion_TA7_Guoliu  76   		// ��������5.5A
#define HUABEI_Guoliu  57		  		// ��������4A
#define BIANMEN_SHENGJIANG_Guoliu 71  	// ��������5A
#define BIANMEN_PINGYI_Guoliu 20  	// ��������1.5A
#define ZUOBIANQI_SHENGJIANG_Guoliu 57  // ��������4A
static u8 DaDianJiGuoliuCount=0;
static u8 XiaoDianJiGuoliuCount=0;
// Ӳ����ʼ��
// P1.18 �е�ϵ���;
static void SensorHwInit(void)
{
//	PINSEL0 &= ~(BIT16|BIT17); // p0.8
//	PINSEL0 &= ~(BIT18|BIT19); // p0.9
//	IO0DIR &= ~BIT8;
//	IO0DIR &= ~BIT9;
	
	PINSEL2 = 0;	// ����P1�ڶ���IO��

	IO1DIR &= ~BIT18;             // �е�ϵ���
	IO1DIR |= BIT16|BIT17|BIT31;
	IO1CLR |= BIT16|BIT17|BIT31;

	AdcInit();
}


// ��������⣬��ʱ�����Ը���ȫ�ֱ���
// �������ҷ��ڵ���˶�ʱˢ�����ﲻ�ô���
// �������
// ����1 :  �е�����
// ����2 :  ��ص�ѹ����20V  ���ŵ������ѹ19V
// ���ʱ��̶�: �趨ʱ��8   Сʱ 
static void SensorCheck(void * parameter)
{
	u32 tmp;
	// ������˶�״̬�������ѹ��� ���<20V,��GB17Ϊ0��˵��ͣ�硣��Ҫ���ϴ����ع���
	// ������˶�״̬�������ѹ��� ���<20V,��GB17Ϊ1��˵��ͣ�������ع����������͡�
//	if((g_MT_work.beibu==0)&&(g_MT_work.tuibu==0)&&(g_MT_work.youfanshen==0)
//		&&(g_MT_work.zuofanshen==0)&&(g_MT_work.bianmenshengjiang==0))
//		MT_All_KongXian = 1;	// ���е��״̬ȫ��Ϊ0��
//	else
//		MT_All_KongXian = 0;
	if(WDcount>100)
	{
		feedWD();
		WDcount = 0;
	}
	WDcount+=1;
	MT_All_KongXian = 1;
	g_diancidianliangAd = GetAdcSmoothly(3);
	g_powerdianya = GetAdcSmoothly(0);

	if(g_diancidianliangAd<399)			// ��ȡ��ѹ<12V
		g_sensor.DianChiZhuOn=0;		// �����ڵ��
	else
		g_sensor.DianChiZhuOn=1;		// ���ڵ��

	g_sensor.DianChiDianYa = g_diancidianliangAd/31;   // ����������ѹ

	if(g_sensor.DianChiZhuOn)    // ���ڵ��
	{
		if(MT_All_KongXian&&(g_powerdianya<DIAN_YA_MIN))   // ��Դ��ѹ���ͣ�˵��ͣ����ߴ��������
		{	
			IO1SET = BIT17; 	// �����ؼ̵���
			g_sensor.TingDian = 1;
//			rt_kprintf("ͣ��-ת-��ع���\r\n");
		}	
	// ��ص���ת��Ϊ�ٷֱȣ�ȡ�����Ŵ�10��������24V��ѹ(744)˵����������Ϊ100%������20V��ѹ(620)˵��û����0%   21v--639,
		g_MT_work.DianChiDianLiang = (g_diancidianliangAd-620)*8;	
	}	

	if(g_MT_work.beibu>0)		// ֻ���ڵ���˶�ʱ��ȡֵ��Ч
		g_sensor.BeiBuFangPing=(0==(IO1PIN&BIT27));
	if(g_MT_work.zuofanshen>0)  // ֻ���ڵ���˶�ʱ��ȡֵ��Ч
		g_sensor.ZuoFanFangPing=(0==(IO1PIN&BIT29));
	if(g_MT_work.youfanshen>0)  // ֻ���ڵ���˶�ʱ��ȡֵ��Ч
		g_sensor.YouFanFangPing=(0==(IO1PIN&BIT22));
	
	g_sensor.ZuoFanFangPing =1;		// ��֧��
	g_sensor.YouFanFangPing =1;		// ��֧��

	if((tuibumapancount>=(m_tuibu_middle-m_wucha))&&(tuibumapancount<=(m_tuibu_middle+m_wucha)))
	{    g_sensor.TuiBuFangPing = 1;
		 g_sensor.ShangQuTuiJiaoDu=0;
		 g_sensor.XiaQuTuiJiaoDu=0;}	  //  ��������ֵȷ���Ȳ�ˮƽλ��
	else
		g_sensor.TuiBuFangPing = 0;	// ��֧��

//	g_sensor.TuiBuFangPing=((IO1PIN&BIT20)>0);	 // �����Ȳ�ˮƽλΪ������λ

	
	if(g_MT_work.tuibu ==1 )
	{
		g_sensor.TuiBuFangPing=((IO1PIN&BIT20)>0);
	}else if(g_MT_work.tuibu == 2)
	{
		if(preLegSensor && (0==(IO1PIN&BIT20)))
		{
			g_sensor.TuiBuFangPing = 1;
		}else{
			g_sensor.TuiBuFangPing = 0;
		}
	}else
	{
		g_sensor.TuiBuFangPing=((IO1PIN&BIT20)>0);
	}
	preLegSensor = ((IO1PIN&BIT20)>0);
	
	if(g_sensor.TuiBuFangPing)
	{
		 g_sensor.ShangQuTuiJiaoDu=0;
		 g_sensor.XiaQuTuiJiaoDu=0;	
	}
	//g_sensor.FangJia = (0==(IO0PIN&BIT8))||(0==(IO0PIN&BIT9));    // ���м��
//	g_sensor.FangJia = (0==(IO1PIN&BIT23))||(0==(IO1PIN&BIT24));	  // ���м�� �޸Ĺܽţ��������ſ��ص���ӿ���
	g_sensor.FangJia= 0;
	
	g_sensor.BianMenPingYiDaoDi= (0==(IO1PIN&BIT26));
	g_sensor.BianMenPingYiDaoDing= (0==(IO1PIN&BIT25));

	if(g_sensor.BianMenPingYiDaoDi&&!g_FlagCmd.Anything)	// �����˶�״̬
			g_sensor.BianMenGuanDaoDi=1;

	if(g_sensor.FangJia)
	{
		BeiBu(0,0);
		TuiBei(0,0);
	}
		
	tmp = (beibumapancount*BEI_JIAODU_MAX) / m_beibu_max;
	g_sensor.QiBeiJiaoDu = (u8)tmp;
	if(tuibumapancount>(m_tuibu_middle+m_wucha))//��ʾ������
	{	g_sensor.ShangQuTuiJiaoDu = (u32)((tuibumapancount-m_tuibu_middle)*TUI_SHANG_JIAODU_MAX/(m_tuibu_max-m_tuibu_middle));
		g_sensor.XiaQuTuiJiaoDu=0;}
	if(tuibumapancount<(m_tuibu_middle-m_wucha))// ��ʾ������
	{	g_sensor.ShangQuTuiJiaoDu =0;
		g_sensor.XiaQuTuiJiaoDu= (u32)((m_tuibu_middle-tuibumapancount)*TUI_XIA_JIAODU_MAX/m_tuibu_middle);
		}	   
	g_sensor.ZuoFanShenJiaoDu= (u32)(zuofanmapancount*FANSHEN_JIAODU_MAX/m_fanshen_max);
	g_sensor.YouFanShenJiaoDu= (u32)(youfanmapancount*FANSHEN_JIAODU_MAX/m_fanshen_max);

	adjustTable();   // �Զ����������Ƹ�


		// �е�����
	c_ShiDianStatus = ((IO1PIN&BIT18)>0);	

	if((((m_ShiDianStatus==0)&&(c_ShiDianStatus==1))||(g_powerdianya>DIAN_YA_MAX))&&g_sensor.TingDian)	// 12v������ ����24V���磬˵���е�����
//	if(c_ShiDianStatus&&g_sensor.TingDian)		// ͣ��״̬������200mS���ٴμ�⵽�е磬�ָ�����
	{
//		shidianjiancelvbo+=1;
//		if(shidianjiancelvbo>2)	
//		{	
		    g_sensor.TingDian = 0;	
//			shidianjiancelvbo=0;
			IO1CLR = BIT17;		// �ر� ����
			rt_kprintf("����-ת-�е繩��\r\n");
			if(g_sensor.DianChiDianYa<24)
				{g_sensor.DianLiangBuZu=1;}
//		}
	}
	m_ShiDianStatus = c_ShiDianStatus;
	// �ж��Ƿ���Ҫ��� ���������е�����δͣ��
	if(g_sensor.DianChiZhuOn&&!g_sensor.TingDian)
	{
		if(g_diancidianliangAd<DIAN_LIANG_MIN)
			dianlianglvbocount+=1;
		else
			dianlianglvbocount=0;
		if(dianlianglvbocount>100)   // ������⵽��ѹ�����ź�10S
			{g_sensor.DianLiangBuZu=1;	dianlianglvbocount=0;}
	}

//	//   1 Сʱһ���л������Դ����������ص���������������㣬���е������������ģʽ��
//	if(g_sensor.DianChiZhuOn&&!g_sensor.TingDian&&!g_sensor.DianLiangBuZu)
//	{
//		if(powerchangetimecount>(DIAN_YUAN_CHANGE_TIME+30))		// 3S��ȵ�Դ�ȶ�
//		{
//			g_diancidianliangAd = GetAdcSmoothly(3);		 
//			IO1CLR = BIT17; 	// �ر� ����
//			powerchangetimecount=0;
//			if(g_diancidianliangAd<DIAN_LIANG_MIN)
//				g_sensor.DianLiangBuZu=1;
//			rt_kprintf("����ص���\r\n");
//		}
//		else
//		{		
//			powerchangetimecount+=1;
//			if(powerchangetimecount==DIAN_YUAN_CHANGE_TIME)		// 1 Сʱ��
//			{
//				IO1SET = BIT17; 	// �����ؼ̵���
//			}			
//		}
//	}

	// ����������㣬���е�������ʼ���   ���� �е���������ÿ��1Сʱ��������Ƿ���Ҫ��� (���û�е���鲻�����)
	if(!g_sensor.TingDian)   //δͣ�� �����������
	{
	 if(g_sensor.DianChiZhuOn&&g_sensor.DianLiangBuZu)
	 {
		if(chongdiantimecount>CHONG_DIAN_TIME_END)	// 8Сʱ��ֹͣ���
		{
			chongdiantimecount=0;
		 	IO1CLR = BIT16;	
			IO1CLR = BIT31;
		 	g_sensor.DianLiangBuZu=0;
			rt_kprintf("������\r\n");	
			g_sensor.ChongDian=0; // ���״̬			
		}
		else
		{
			if(chongdiantimecount==0)
			{
				IO1SET = BIT16;		// �������
				IO1SET = BIT31;		// ָʾ����
				rt_kprintf("��ʼ���\r\n");
				g_sensor.ChongDian=1; // ���״̬
			}
			chongdiantimecount+=1;
		}
	 }	
	}
	else		// ͣ���ҵ�������
	{
	 if(g_sensor.DianChiZhuOn&&g_sensor.DianLiangBuZu)
	 {
	 	 if(g_diancidianliangAd<DIAN_LIANG_MIN)	//<21V
	 	 {
		 	rt_kprintf("ͣ�粢�ҵ�ص�ص�������\r\n");
		 	IO1CLR = BIT17;	 // �ر� ����   ���ڱ�����������ڹ��ŵ�
	 	 }
	 }
	}

	DianJiDianLiuAD1 = (u16)GetAdcSmoothly(1);	// 10λAD������������
	DianJiDianLiuAD2 = (u16)GetAdcSmoothly(2);	// 10λAD�����Ȳ����ҷ�
//	DianJiDianLiuAD3 = (u16)GetAdcSmoothly(3);	// 10λAD���������ͱ������Ƹ�
	g_sensor.DianJiGuoLiuBao=0;	

	// 1S��������ȡ�������ź��൱��10�β���
	if((DianJiDianLiuAD1>Timotion_TA7_Guoliu)||(DianJiDianLiuAD2>Timotion_TA7_Guoliu))
		DaDianJiGuoliuCount+=1;
	else
	{
		if(!DaDianJiGuoliuCount)
			DaDianJiGuoliuCount=0;
		else
			DaDianJiGuoliuCount-=1;
	}
	if(DaDianJiGuoliuCount>10)
		{DoStop();g_sensor.DianJiGuoLiuBao=1;DaDianJiGuoliuCount=10;}

	if(((g_MT_work.huabei>0)&&(DianJiDianLiuAD3>HUABEI_Guoliu))
		||((g_MT_work.bianmenshengjiang>0)&&(DianJiDianLiuAD3>BIANMEN_SHENGJIANG_Guoliu))
		||((g_MT_work.bianmenpingyi>0)&&(DianJiDianLiuAD3>BIANMEN_PINGYI_Guoliu))
		||((g_MT_work.zuobianqishengjiang>0)&&(DianJiDianLiuAD3>ZUOBIANQI_SHENGJIANG_Guoliu)))
		XiaoDianJiGuoliuCount+=1;
	else
	{
		if(!XiaoDianJiGuoliuCount)
			XiaoDianJiGuoliuCount=0;
		else
			XiaoDianJiGuoliuCount-=1;
	}
	if(XiaoDianJiGuoliuCount>10)
		{DoAllStop();g_sensor.DianJiGuoLiuBao=1;XiaoDianJiGuoliuCount=10;}	
}

// �жϳ������ڶ�ȡ����ֵ
// ����IO�ڣ�P1.19 ����P1.30 �Ȳ�P1.28 ����P1.21 �ҷ���

/*
static void WeiGou(void * parameter)
{
//	feedWD();	 // ι��
	WDFEED =0XAA;
	WDFEED =0X55;
}

static void KanMenGou(void)
{
	static struct rt_timer t;
	rt_timer_init(&t, "kanmengou", WeiGou, RT_NULL, RT_TICK_PER_SECOND, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
	rt_timer_start(&t);
}
*/

// ��������ʼ��
void SensorInit(void)
{
	SensorHwInit();
	memset((void*)&g_sensor,0,sizeof(g_sensor));
	memset((void*)&g_MT_work,0,sizeof(g_MT_work));
	memset((void*)&g_AD_sensor,0,sizeof(g_AD_sensor));
	memset((void*)&g_ZuobianqiSensor,0,sizeof(g_ZuobianqiSensor));
	
//	m_lastSensorStatus =  g_sensor;

	rt_timer_init(&sensorTimer , "sensor", SensorCheck, RT_NULL, RT_TICK_PER_SECOND*0.1f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_HARD_TIMER);		// �����Ե���
	rt_timer_start(&sensorTimer);		// ��ʼ����

	AutoSensor(0);
//	KanMenGou();
}

