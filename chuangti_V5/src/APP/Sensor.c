#include "Sensor.h"
#include "Adc.h"
#ifdef RT_USING_FINSH
#include "CanAnalyzer.h"
#include "CAN/IncludeCan.h"
#include <Actionthread.h>

#define DIAN_YUAN_CHANGE_TIME   360000  // 36000 // 1小时采样一次供电
#define CHONG_DIAN_TIME_END    288000  // 8小时后停止充电
// 传感器状态全局变量
volatile s_Sensor g_sensor = {0};		
 volatile ADS_Sensor g_AD_sensor = {0};
volatile s_MT_work g_MT_work = {0};
static u8 preLegSensor=0;
volatile s_ZuobianqiSensor g_ZuobianqiSensor = {0};
static u8 MT_All_KongXian=0;	// 所有电机空闲状态

static u16 WDcount=0;

// 最新一次传感器状态，用于滤波
//static s_Sensor m_lastSensorStatus;
#define DIAN_LIANG_MIN   639 // 设定电量最低值21V    A=30.751*VCC
#define DIAN_YA_MIN      615  // 设定电源电压最低值20V    A=30.751*VCC
#define DIAN_YA_MAX      700
static u32 chongdiantimecount=0;  // 充电时间计数1 次100mS
u32 g_diancidianliangAd=0;	// 电池电量读取
u32 g_powerdianya=0;		// 开关电源 24V输出电压读取

static u8 m_ShiDianStatus=0;	// 前一刻市电电压情况
static u8 c_ShiDianStatus=0;	// 当前市电电压情况
static u32 powerchangetimecount=0;	// 电源切换时间计数  1小时切换一次来读取电量
static u8 dianlianglvbocount=0;	//电量检测滤波 10S = 100
static u8 shidianjiancelvbo=0;    // 市电检测滤波

u8 m_TuiBuStatus=0;		// 腿部推杆之前的水平状态
u8 c_TuiBuStatus=0;     // 腿部推杆当前的水平状态

u8 m_bei_hall=0;  // 背部霍尔信号前一个状态值
u16 beibumapancount=0; // 背部行程位置码盘累计值
u8 m_tui_hall=0;  // 腿部霍尔信号前一个状态值
u16 tuibumapancount=0; // 腿部行程位置码盘累计值
u8 m_zuofan_hall=0;  // 左翻霍尔信号前一个状态值
u16 zuofanmapancount=0; // 左翻行程位置码盘累计值
u8 m_youfan_hall=0;  // 右翻霍尔信号前一个状态值
u16 youfanmapancount=0; // 右翻行程位置码盘累计值

u16 m_beibu_max = BEI_MAPAN_MAX;
u16 m_beibu_middle = BEI_MAPAN_MIDDLE;
u16 m_tuibu_max = TUI_MAPAN_MAX;
u16 m_tuibu_middle = TUI_MAPAN_SHUI_PING;
u16 m_fanshen_max = FANSHEN_MAPAN_MAX;
u16 m_wucha = WUCHAJINGDU;

FINSH_VAR_EXPORT(m_beibu_max,finsh_type_short,"背部到顶");
FINSH_VAR_EXPORT(m_beibu_middle,finsh_type_short,"背部中间");
FINSH_VAR_EXPORT(m_tuibu_max,finsh_type_short,"腿部到顶");
FINSH_VAR_EXPORT(m_tuibu_middle,finsh_type_short,"腿部中间");
FINSH_VAR_EXPORT(m_fanshen_max,finsh_type_short,"翻身到顶");
FINSH_VAR_EXPORT(m_wucha,finsh_type_short,"误差");

void GetBeiBuWeiZhi()
{
	rt_kprintf("\tbeibumapancount=%d\r\n",beibumapancount);	
	rt_kprintf("\tm_beibu_max=%d\r\n",m_beibu_max);
	rt_kprintf("\tm_beibu_middle=%d\r\n",m_beibu_middle);
}
FINSH_FUNCTION_EXPORT(GetBeiBuWeiZhi,"背部参数获取");

void SetBeiBuWeiZhi(u32 max,u32 middle)
{
	m_beibu_max = (u16)max;
	m_beibu_middle = (u16)middle;
	GetBeiBuWeiZhi();
}
FINSH_FUNCTION_EXPORT(SetBeiBuWeiZhi,"背部参数设置");

void GetTuiBuWeiZhi()
{
	rt_kprintf("\ttuibumapancount=%d\r\n",tuibumapancount);	
	rt_kprintf("\tm_tuibu_max=%d\r\n",m_tuibu_max);
	rt_kprintf("\tm_tuibu_middle=%d\r\n",m_tuibu_middle);
}
FINSH_FUNCTION_EXPORT(GetTuiBuWeiZhi,"腿部参数获取");

void SetTuiBuWeiZhi(u32 max,u32 middle)
{
	m_tuibu_max = (u16)max;
	m_tuibu_middle = (u16)middle;
	GetTuiBuWeiZhi();
}
FINSH_FUNCTION_EXPORT(SetTuiBuWeiZhi,"腿部参数设置");

void GetFanShenWeiZhi()
{
	rt_kprintf("\tzuofanmapancount=%d\r\n",zuofanmapancount);
	rt_kprintf("\tyoufanmapancount=%d\r\n",youfanmapancount);	
	rt_kprintf("\tm_zuofan_max=%d\r\n",m_fanshen_max);
}
FINSH_FUNCTION_EXPORT(GetFanShenWeiZhi,"翻身参数获取");

void SetFanShenWeiZhi(u32 max)
{
	m_fanshen_max = (u16)max;
	GetFanShenWeiZhi();
}
FINSH_FUNCTION_EXPORT(SetFanShenWeiZhi,"翻身参数设置");


void GetWuCha()
{
	rt_kprintf("\tm_wucha=%d\r\n",m_wucha);
}
FINSH_FUNCTION_EXPORT(GetWuCha,"误差参数获取");

void SetWuCha(u32 jingdu)
{
	m_wucha = (u16)jingdu;
	GetWuCha();
}
FINSH_FUNCTION_EXPORT(SetWuCha,"误差参数设置");


// 显示传感器信息
/*
static void SensorList(void * parameter)
{
	rt_kprintf("当前GPIO配置:\r\n");
	rt_kprintf("\tPINSEL0=%04X    PINSEL1=%04X    PINSEL2=%04X    IO0DIR=%04X    IO1DIR=%04X    IO0PIN=%04X    IO1PIN=%04X    \r\n",
		PINSEL0,PINSEL1,PINSEL2,IO0DIR,IO1DIR,IO0PIN,IO1PIN);
	rt_kprintf("当前CAN配置:\r\n");
//	rt_kprintf("\tCAN0MOD=%04X	CAN0ICR=%04X	CAN0SR=%04X\r\n",CANMOD(0).Word,CANICR(0).Word,CANSR(0).Word);
	
	rt_kprintf("当前传感器状态:\r\n");
	rt_kprintf("\tg_sensor= %X %X\r\n",(u32)(*((u8*)(&g_sensor))),(u32)(*(((u8*)(&g_sensor))+1)));

	rt_kprintf("\t背部放平: %s\r\n",g_sensor.BeiBuFangPing?"Y":"");
	rt_kprintf("\t腿部放平: %s\r\n",g_sensor.TuiBuFangPing?"Y":"");
	rt_kprintf("\t左侧翻水平到位: %s\r\n",g_sensor.ZuoFanFangPing?"Y":"");
	rt_kprintf("\t右侧翻水平到位: %s\r\n",g_sensor.YouFanFangPing?"Y":"");
	rt_kprintf("\t防夹检测: %s\r\n",g_sensor.FangJia?"Y":"");
	
	rt_kprintf("\t便门全打开: %s\r\n",g_sensor.BianMenKaiDaoDing?"Y":"");
	rt_kprintf("\t便门全关闭: %s\r\n",g_sensor.BianMenGuanDaoDi?"Y":"");
	rt_kprintf("\t便门平移到顶: %s\r\n",g_sensor.BianMenPingYiDaoDing?"Y":"");
	rt_kprintf("\t便门平移到底: %s\r\n",g_sensor.BianMenPingYiDaoDi?"Y":"");

	rt_kprintf("\t电池电量不足: %s\r\n",g_sensor.DianLiangBuZu?"Y":"");
	rt_kprintf("\t电机过流保护: %s\r\n",g_sensor.DianJiGuoLiuBao?"Y":"");

	rt_kprintf("\t市电断电: %s\r\n",g_sensor.TingDian?"Y":"");
	rt_kprintf("\t12V开关电源: %s\r\n",c_ShiDianStatus?"Y":"");
		
	rt_kprintf("\t电池是否存在: %s\r\n",g_sensor.DianChiZhuOn?"Y":"");

	rt_kprintf("\t自动清洗: %s\r\n",g_ZuobianqiSensor.FunctionWorkAuto?"Y":"");

	rt_kprintf("\t起背角度: %d °\r\n",g_sensor.QiBeiJiaoDu);
	rt_kprintf("\t下曲腿角度: %d °\r\n",g_sensor.XiaQuTuiJiaoDu);
	rt_kprintf("\t上曲腿角度: %d °\r\n",g_sensor.ShangQuTuiJiaoDu);	
	rt_kprintf("\t左翻身角度: %d °\r\n",g_sensor.ZuoFanShenJiaoDu);
	rt_kprintf("\t右翻身角度: %d °\r\n",g_sensor.YouFanShenJiaoDu);

	rt_kprintf("\t背部码盘: %d \r\n",beibumapancount);
	rt_kprintf("\t腿部码盘: %d \r\n",tuibumapancount);
	rt_kprintf("\t左翻身码盘: %d \r\n",zuofanmapancount);
	rt_kprintf("\t右翻身码盘: %d \r\n",youfanmapancount);

	rt_kprintf("\t电池组电压: %d V \r\n",(g_diancidianliangAd/31));

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

	rt_kprintf("\tBack Angel: %d °\r\n",g_sensor.QiBeiJiaoDu);
	rt_kprintf("\tDown-Leg Angel: %d °\r\n",g_sensor.XiaQuTuiJiaoDu);
	rt_kprintf("\tUp-Leg Angel: %d °\r\n",g_sensor.ShangQuTuiJiaoDu);	

	rt_kprintf("\tBack encode: %d \r\n",beibumapancount);
	rt_kprintf("\tLeg encode: %d \r\n",tuibumapancount);
	rt_kprintf("\tDining time: %d \r\n",g_MT_work.diningCnt);
	

	rt_kprintf("\tBatt Voltage: %d V \r\n",(g_diancidianliangAd/31));

}
FINSH_FUNCTION_EXPORT(SensorList, "Display Sensor Status");

// 自动监控
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
FINSH_FUNCTION_EXPORT(AutoSensor, "自动监控传感器,参数[0]=0表示停止，否则表示开始");
#endif

// 定时器，用于周期性调用传感器检测函数
static struct rt_timer sensorTimer;
static u16 DianJiDianLiuAD1=0;
static u16 DianJiDianLiuAD2=0;
static u16 DianJiDianLiuAD3=0;
#define Timotion_TA7_Guoliu  76   		// 过流保护5.5A
#define HUABEI_Guoliu  57		  		// 过流保护4A
#define BIANMEN_SHENGJIANG_Guoliu 71  	// 过流保护5A
#define BIANMEN_PINGYI_Guoliu 20  	// 过流保护1.5A
#define ZUOBIANQI_SHENGJIANG_Guoliu 57  // 过流保护4A
static u8 DaDianJiGuoliuCount=0;
static u8 XiaoDianJiGuoliuCount=0;
// 硬件初始化
// P1.18 市电断电检测;
static void SensorHwInit(void)
{
//	PINSEL0 &= ~(BIT16|BIT17); // p0.8
//	PINSEL0 &= ~(BIT18|BIT19); // p0.9
//	IO0DIR &= ~BIT8;
//	IO0DIR &= ~BIT9;
	
	PINSEL2 = 0;	// 所有P1口多做IO口

	IO1DIR &= ~BIT18;             // 市电断电检测
	IO1DIR |= BIT16|BIT17|BIT31;
	IO1CLR |= BIT16|BIT17|BIT31;

	AdcInit();
}


// 传感器检测，定时调用以更新全局变量
// 背部左翻右翻在电机运动时刷新这里不用处理
// 开启充电
// 条件1 :  市电正常
// 条件2 :  电池电压低于20V  ，放电截至电压19V
// 充电时间固定: 设定时间8   小时 
static void SensorCheck(void * parameter)
{
	u32 tmp;
	// 电机非运动状态，供电电压检测 如果<20V,且GB17为0，说明停电。需要马上打开蓄电池供电
	// 电机非运动状态，供电电压检测 如果<20V,且GB17为1，说明停电且蓄电池工作，电量低。
//	if((g_MT_work.beibu==0)&&(g_MT_work.tuibu==0)&&(g_MT_work.youfanshen==0)
//		&&(g_MT_work.zuofanshen==0)&&(g_MT_work.bianmenshengjiang==0))
//		MT_All_KongXian = 1;	// 所有电机状态全部为0，
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

	if(g_diancidianliangAd<399)			// 读取电压<12V
		g_sensor.DianChiZhuOn=0;		// 不存在电池
	else
		g_sensor.DianChiZhuOn=1;		// 存在电池

	g_sensor.DianChiDianYa = g_diancidianliangAd/31;   // 反馈电池组电压

	if(g_sensor.DianChiZhuOn)    // 存在电池
	{
		if(MT_All_KongXian&&(g_powerdianya<DIAN_YA_MIN))   // 电源电压过低，说明停电或者大电流拉低
		{	
			IO1SET = BIT17; 	// 打开蓄电池继电器
			g_sensor.TingDian = 1;
//			rt_kprintf("停电-转-电池供电\r\n");
		}	
	// 电池电量转换为百分比，取整，放大10倍，大于24V电压(744)说明电量充足为100%，低于20V电压(620)说明没电了0%   21v--639,
		g_MT_work.DianChiDianLiang = (g_diancidianliangAd-620)*8;	
	}	

	if(g_MT_work.beibu>0)		// 只有在电机运动时读取值有效
		g_sensor.BeiBuFangPing=(0==(IO1PIN&BIT27));
	if(g_MT_work.zuofanshen>0)  // 只有在电机运动时读取值有效
		g_sensor.ZuoFanFangPing=(0==(IO1PIN&BIT29));
	if(g_MT_work.youfanshen>0)  // 只有在电机运动时读取值有效
		g_sensor.YouFanFangPing=(0==(IO1PIN&BIT22));
	
	g_sensor.ZuoFanFangPing =1;		// 不支持
	g_sensor.YouFanFangPing =1;		// 不支持

	if((tuibumapancount>=(m_tuibu_middle-m_wucha))&&(tuibumapancount<=(m_tuibu_middle+m_wucha)))
	{    g_sensor.TuiBuFangPing = 1;
		 g_sensor.ShangQuTuiJiaoDu=0;
		 g_sensor.XiaQuTuiJiaoDu=0;}	  //  根据码盘值确定腿部水平位置
	else
		g_sensor.TuiBuFangPing = 0;	// 不支持

//	g_sensor.TuiBuFangPing=((IO1PIN&BIT20)>0);	 // 假设腿部水平位为电机最高位

	
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
	//g_sensor.FangJia = (0==(IO0PIN&BIT8))||(0==(IO0PIN&BIT9));    // 防夹检测
//	g_sensor.FangJia = (0==(IO1PIN&BIT23))||(0==(IO1PIN&BIT24));	  // 防夹检测 修改管脚，换到便门开关电机接口上
	g_sensor.FangJia= 0;
	
	g_sensor.BianMenPingYiDaoDi= (0==(IO1PIN&BIT26));
	g_sensor.BianMenPingYiDaoDing= (0==(IO1PIN&BIT25));

	if(g_sensor.BianMenPingYiDaoDi&&!g_FlagCmd.Anything)	// 不在运动状态
			g_sensor.BianMenGuanDaoDi=1;

	if(g_sensor.FangJia)
	{
		BeiBu(0,0);
		TuiBei(0,0);
	}
		
	tmp = (beibumapancount*BEI_JIAODU_MAX) / m_beibu_max;
	g_sensor.QiBeiJiaoDu = (u8)tmp;
	if(tuibumapancount>(m_tuibu_middle+m_wucha))//表示上曲腿
	{	g_sensor.ShangQuTuiJiaoDu = (u32)((tuibumapancount-m_tuibu_middle)*TUI_SHANG_JIAODU_MAX/(m_tuibu_max-m_tuibu_middle));
		g_sensor.XiaQuTuiJiaoDu=0;}
	if(tuibumapancount<(m_tuibu_middle-m_wucha))// 表示下曲腿
	{	g_sensor.ShangQuTuiJiaoDu =0;
		g_sensor.XiaQuTuiJiaoDu= (u32)((m_tuibu_middle-tuibumapancount)*TUI_XIA_JIAODU_MAX/m_tuibu_middle);
		}	   
	g_sensor.ZuoFanShenJiaoDu= (u32)(zuofanmapancount*FANSHEN_JIAODU_MAX/m_fanshen_max);
	g_sensor.YouFanShenJiaoDu= (u32)(youfanmapancount*FANSHEN_JIAODU_MAX/m_fanshen_max);

	adjustTable();   // 自动调整餐桌推杆


		// 市电来电
	c_ShiDianStatus = ((IO1PIN&BIT18)>0);	

	if((((m_ShiDianStatus==0)&&(c_ShiDianStatus==1))||(g_powerdianya>DIAN_YA_MAX))&&g_sensor.TingDian)	// 12v上升沿 或者24V供电，说明市电来电
//	if(c_ShiDianStatus&&g_sensor.TingDian)		// 停电状态，并且200mS后再次检测到有电，恢复供电
	{
//		shidianjiancelvbo+=1;
//		if(shidianjiancelvbo>2)	
//		{	
		    g_sensor.TingDian = 0;	
//			shidianjiancelvbo=0;
			IO1CLR = BIT17;		// 关闭 蓄电池
			rt_kprintf("来电-转-市电供电\r\n");
			if(g_sensor.DianChiDianYa<24)
				{g_sensor.DianLiangBuZu=1;}
//		}
	}
	m_ShiDianStatus = c_ShiDianStatus;
	// 判断是否需要充电 条件必须有电池组和未停电
	if(g_sensor.DianChiZhuOn&&!g_sensor.TingDian)
	{
		if(g_diancidianliangAd<DIAN_LIANG_MIN)
			dianlianglvbocount+=1;
		else
			dianlianglvbocount=0;
		if(dianlianglvbocount>100)   // 连续检测到电压过低信号10S
			{g_sensor.DianLiangBuZu=1;	dianlianglvbocount=0;}
	}

//	//   1 小时一次切换供电电源，用来检测电池电量，如果电量不足，且市电正常则开启充电模式。
//	if(g_sensor.DianChiZhuOn&&!g_sensor.TingDian&&!g_sensor.DianLiangBuZu)
//	{
//		if(powerchangetimecount>(DIAN_YUAN_CHANGE_TIME+30))		// 3S后等电源稳定
//		{
//			g_diancidianliangAd = GetAdcSmoothly(3);		 
//			IO1CLR = BIT17; 	// 关闭 蓄电池
//			powerchangetimecount=0;
//			if(g_diancidianliangAd<DIAN_LIANG_MIN)
//				g_sensor.DianLiangBuZu=1;
//			rt_kprintf("检测电池电量\r\n");
//		}
//		else
//		{		
//			powerchangetimecount+=1;
//			if(powerchangetimecount==DIAN_YUAN_CHANGE_TIME)		// 1 小时后
//			{
//				IO1SET = BIT17; 	// 打开蓄电池继电器
//			}			
//		}
//	}

	// 如果电量不足，且市电正常开始充电   或者 市电正常，且每隔1小时检测电池组是否需要充电 (如果没有电池组不允许打开)
	if(!g_sensor.TingDian)   //未停电 电量不足情况
	{
	 if(g_sensor.DianChiZhuOn&&g_sensor.DianLiangBuZu)
	 {
		if(chongdiantimecount>CHONG_DIAN_TIME_END)	// 8小时后停止充电
		{
			chongdiantimecount=0;
		 	IO1CLR = BIT16;	
			IO1CLR = BIT31;
		 	g_sensor.DianLiangBuZu=0;
			rt_kprintf("充电完成\r\n");	
			g_sensor.ChongDian=0; // 充电状态			
		}
		else
		{
			if(chongdiantimecount==0)
			{
				IO1SET = BIT16;		// 充电器打开
				IO1SET = BIT31;		// 指示灯亮
				rt_kprintf("开始充电\r\n");
				g_sensor.ChongDian=1; // 充电状态
			}
			chongdiantimecount+=1;
		}
	 }	
	}
	else		// 停电且电量不足
	{
	 if(g_sensor.DianChiZhuOn&&g_sensor.DianLiangBuZu)
	 {
	 	 if(g_diancidianliangAd<DIAN_LIANG_MIN)	//<21V
	 	 {
		 	rt_kprintf("停电并且电池电池电量不足\r\n");
		 	IO1CLR = BIT17;	 // 关闭 蓄电池   用于保护电池组免于过放电
	 	 }
	 }
	}

	DianJiDianLiuAD1 = (u16)GetAdcSmoothly(1);	// 10位AD采样背部和左翻
	DianJiDianLiuAD2 = (u16)GetAdcSmoothly(2);	// 10位AD采样腿部和右翻
//	DianJiDianLiuAD3 = (u16)GetAdcSmoothly(3);	// 10位AD采样滑背和便门三推杆
	g_sensor.DianJiGuoLiuBao=0;	

	// 1S内连续读取到过流信号相当于10次采样
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

// 中断程序，用于读取码盘值
// 码盘IO口，P1.19 背部P1.30 腿部P1.28 左翻身P1.21 右翻身

/*
static void WeiGou(void * parameter)
{
//	feedWD();	 // 喂狗
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

// 传感器初始化
void SensorInit(void)
{
	SensorHwInit();
	memset((void*)&g_sensor,0,sizeof(g_sensor));
	memset((void*)&g_MT_work,0,sizeof(g_MT_work));
	memset((void*)&g_AD_sensor,0,sizeof(g_AD_sensor));
	memset((void*)&g_ZuobianqiSensor,0,sizeof(g_ZuobianqiSensor));
	
//	m_lastSensorStatus =  g_sensor;

	rt_timer_init(&sensorTimer , "sensor", SensorCheck, RT_NULL, RT_TICK_PER_SECOND*0.1f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_HARD_TIMER);		// 周期性调用
	rt_timer_start(&sensorTimer);		// 开始调用

	AutoSensor(0);
//	KanMenGou();
}

