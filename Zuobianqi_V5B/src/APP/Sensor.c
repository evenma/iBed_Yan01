#include "Sensor.h"
#include "Adc.h"
extern void rotateDining(u8 work,u8 dir);
extern u8 rotateDiningCount;
extern u8 rotateDiningDir;
// 传感器状态全局变量
volatile s_Sensor g_sensor = {0};		
// 床板传感器状态全局变量
volatile s_BedSensor g_bedSensor = {0};
volatile s_BedSensor_MT_work g_bedsensor_MT_work={0};
// 更换餐桌机械臂推杆，改为TA16,4.7mm/s,110mm行程,时长在25s。在行程55mm处有个中间位置传感器
#define TIME_RUN_DINING_MT 		21		// 实际时间
//#define TIME_RUN_DINING_MT 		10  //13  修改了推杆尺寸130改为100mm //S 推杆运行时长 需要同床体控制板的参数一致，为餐桌推杆运行时间
// 最新一次传感器状态，用于滤波
static s_Sensor m_lastSensorStatus;
static u8 m_lastBianMen = 0;   // 便门打开记录
// 注水 水泵 5L/min;
static u8 m_lastzhushui=0;
static u8 g_actionzhushui=0;
#define ZHU_SHUI_TIME 80//100   // 10S计数 空载10S后停止
#define QI_DONG_TIME 200    // 20S 启动注水时间内禁止检测流量计
#define ZENG_YA_FA_TIME 200   // 20S
static u8 g_zhushui_time=0;
static u8 g_qidong_time=0;		// 注水过程启动后，直到注水停止
// 自动清洗座便器
static u8 g_actionzuobianquan=0;
static u8 m_lastbianmenguanbi=1;
//#define ZUO_BIAN_QUAN_TIME 100  // 10S*0.33L/S=3.3L
static u8 g_zuobianquan_time=0;
static u16 DianJiDianLiuAD1=0;
#define Pai_Wu_Beng_Guoliu 100	// 过流保护电流 7A
static u8 DaDianJiGuoliuCount=0;
static u8 m_waterIO = 0;
static u8 m_flag_waterIO = 0;
static u16 water_cnt =0;
// 称重
#define CHENG_ZHONG_TIME  20      // 2s1次读取， 100ms/次计数
static u32 g_chengzhong_time = 0;  // 称重污物箱 
static u16 TuoJiaZhongLiang=(230+610);//230;	// 托架和托盘的重量g
#define WU_WU_XIANG_KONG  	1313// 500  	// 污物箱空的重量 g
#define WU_WU_XIANG_BU_ZAI_WEI  1000    // 污物箱不在位；
#define WU_WU_XIANG_BORDER 8000	// 污物箱临界值 g
#define Cheng_Zhong_Work(on)	if(on){IO1CLR = BIT26;} else {IO1SET = BIT26;} 
unsigned long ChengZhongReg=0;

#define WenDuJianCe_TIME  100      // 1 MIN 60s 100ms/次
static u32 g_wendu_time = 0;  //温度检测计时

u8 flag_rotate = 0;    // 表示启动过旋转餐桌电机

#ifdef RT_USING_FINSH

#include "CAN/IncludeCan.h"
// 显示传感器信息
static void SensorList(void * parameter)
{
	s32 i;
	rt_kprintf("当前GPIO配置:\r\n");
	rt_kprintf("\tPINSEL0=%04X    PINSEL1=%04X    PINSEL2=%04X    IO0DIR=%04X    IO1DIR=%04X    IO0PIN=%04X    IO1PIN=%04X    \r\n",
		PINSEL0,PINSEL1,PINSEL2,IO0DIR,IO1DIR,IO0PIN,IO1PIN);
	rt_kprintf("当前CAN配置:\r\n");
	rt_kprintf("\tCAN0MOD=%04X	CAN0ICR=%04X	CAN0SR=%04X\r\n",CANMOD(0).Word,CANICR(0).Word,CANSR(0).Word);
	
	rt_kprintf("当前传感器状态:\r\n");
	rt_kprintf("\t便门开: %s\r\n",g_bedSensor.BianMenKaiDaoDing?"Y":"");
	rt_kprintf("\t便门关: %s\r\n",g_bedSensor.BianMenGuanDaoDi?"Y":"");
	rt_kprintf("\t大便: %s\r\n",g_sensor.DaBian?"Y":"");
	rt_kprintf("\t男士小便: %s\r\n",g_sensor.ManXiaoBian?"Y":"");
	rt_kprintf("\t女士小便: %s\r\n",g_sensor.WomenXiaoBian?"Y":"");
	rt_kprintf("\t污物箱管路连接状态: %s\r\n",g_sensor.WuWuXiangGuan?"Yes":"No");	
	rt_kprintf("\t水箱管路连接状态: %s\r\n",g_sensor.ShuiXiangLow?"No":"Yes");		
	
	rt_kprintf("\t水箱满: %s\r\n",g_sensor.ShuiXiangHigh?"Y":"");	
	rt_kprintf("\t水箱低: %s\r\n",g_sensor.ShuiXiangLow?"Y":"");	
	rt_kprintf("\t自动注水水泵工作: %s\r\n",g_actionzhushui?"Y":"");
	rt_kprintf("\t速热器工作: %s\r\n",g_sensor.SuReQiWork?"Y":"");
	rt_kprintf("\t停电: %s\r\n",g_bedSensor.TingDian?"Y":"");
	rt_kprintf("\t电量不足: %s\r\n",g_bedSensor.DianLiangBuZu?"Y":"");
	rt_kprintf("\t速热器报警错误: %d\r\n",(u32)g_sensor.SuReQiError);
	rt_kprintf("\t污物箱重量: %d\r\n",(u32)g_sensor.WuWuXiangZhongLiang);
	rt_kprintf("\t称重寄存器值: %d\r\n",(u32)ChengZhongReg);
	rt_kprintf("\t餐桌旋转电机: %d\r\n",g_sensor.rotateDiningCnt);
	rt_kprintf("\trotate: %d\r\n",rotateDiningCount);
//	rt_kprintf("\t床垫温度: %d\r\n",(u32)g_sensor.ChuanDianWenDu);
	rt_kprintf("\tg_sensor= %X, %X\r\n",(u32)(*((u8*)(&g_sensor))),(u32)(*(((u8*)(&g_sensor))+1)));
}
FINSH_FUNCTION_EXPORT(SensorList, "显示当前传感器状态");


// 自动监控
static void AutoSensor(u32 start)
{
	static u8 init = 0;
	static struct rt_timer t;
	if(init ==0)
	{
		rt_timer_init(&t, "monitor", SensorList, NULL, RT_TICK_PER_SECOND*1, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
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

BOOL Read_Cheng_Zhong_Dout(void)
{
	if(0==(IO1PIN&BIT16))
		return 0;
	else
		return 1;
}

// 24位数据输出
static unsigned long ChengZhongReadCount(void)
{
	unsigned long Count,j;
	unsigned char i;
	Cheng_Zhong_Work(1); //使能AD（PD_SCK 置低）
	Count=0;
//	while(Read_Cheng_Zhong_Dout()); //AD转换未结束则等待，否则开始读取
	for(j=0;j<10000;j++)
	{
		if(j==9999)
			return 0xffffff;		//	通讯异常
		if(Read_Cheng_Zhong_Dout())
			DelayXus(100);	//延时 100uS
		else
			break;			// 已经建立，通讯开始		
	}
//	rt_kprintf("\tj: %d\r\n",(u32)j);	
	
	for (i=0;i<24;i++)
	{
		Cheng_Zhong_Work(0); //PD_SCK 置高（发送脉冲）
		Count=Count<<1; //下降沿来时变量Count左移一位，右侧补零
		DelayXus(2);	//延时 2uS
		Cheng_Zhong_Work(1); //PD_SCK 置低
		if((IO1PIN&BIT16)>0) Count+=1;
		DelayXus(2);	//延时 2uS			
	}
	Cheng_Zhong_Work(0);
//	Count=Count^0x800000;//第25个脉冲下降沿来时，转换数据
	if((Count& 0x800000) == 0x800000)
	{
		Count= (~(Count - 1)&0xffffff);		   //这里是由补码求出其除符号位的原码的计算。
	}	  		
	DelayXus(2);	//延时 2uS	
	Cheng_Zhong_Work(1);
	DelayXus(2);	//延时 2uS	
	return(Count);
}

// 把称重值为寄存器值转换为重量单位g  传感器重量最大值为20KG
// 4.3V 供电，20kg传感器， 灵敏度 2mV/V 相当于满量程最大输出电压8.6mV,最小分辨度 2.86g
// hx711 放大128倍，相当于满量程最大输出电压 1100.8mV(1.1V),24位转换后Reg=4294967,
// 假设重量值为A kg,测得的AD值为Reg
// AD电压=A*8.6mV/20kg  128倍放大再转为24bit数字信号=(A*8.6mV/20Kg)*128*2^24/4.3V
// 相当于Reg = 214748.36*A, 将kg转为g后，得到Reg = 214.75*A,
// 所以，测得重量A=Reg/214.75 (g) 浮点运算
static unsigned int ChengZhongRegToG(unsigned long Reg)
{
	unsigned int Weight_Shiwu;
	Weight_Shiwu = (unsigned int)((float)Reg/210.00+0.05);		// 原始值 214.75
	if(Weight_Shiwu>TuoJiaZhongLiang)
		Weight_Shiwu = Weight_Shiwu - TuoJiaZhongLiang;	// 减去金属托架的重量
	else
		Weight_Shiwu = 0 ;
	if(0xffffff==Reg)	// 表示通讯异常
		Weight_Shiwu = 0xffff;
	return Weight_Shiwu;
}

// 定时器，用于周期性调用传感器检测函数
static struct rt_timer sensorTimer;

// 硬件初始化
static void SensorHwInit(void)
{
	Cheng_Zhong_Work(0);
	IO1DIR &= ~(BIT16);		// 称重值
	IO0DIR &= ~(BIT30);		// 水箱水位 低
	IO0DIR &= ~(BIT29);     // 水箱水位 高
//	IO0DIR &= ~(BIT2);		// 管路连接开关
	IO0DIR &= ~(BIT3);      // 注水启动开关
	IO0DIR &= ~(BIT4);	    // 注水水流开关
	IO1DIR &= ~(BIT17);		// 大便检测
	IO1DIR &= ~(BIT18);     // 女士小便检测
	IO1DIR &= ~(BIT19);     // 男士小便检测
	IO0DIR &= ~(BIT23);     // 马桶内液位满检测
	
	AdcInit();  // ad1
}

static u8 flag_zhushuibutton=0;
static u8 m_shuiwei=0;
static u8 g_zengyafa_time=0;
static u8 flag_zengyafa=0;
static u8 m_lastdabian=0;
// 传感器检测，定时调用以更新全局变量
static void SensorCheck(void * parameter)
{
//	unsigned long ChengZhongReg=0;
	u8 currzhushui,shuiliukaiguan;

//	g_sensor.WuWuXiangGuan		= ((IO0PIN&BIT2)>0);	// 污物箱管路连接状态
	g_sensor.WuWuXiangGuan = ((IO0PIN&BIT2)>0);	// 污物箱管路连接状态 0：未连接
	g_sensor.ShuiXiangHigh 		= ((IO0PIN&BIT29)>0);	// 水箱满状态
//	g_sensor.ShuiXiangLow 		= (0==(IO0PIN&BIT30));	// 水箱低状态
	g_sensor.ShuiXiangLow = ((IO0PIN&BIT5)==0);   // 水箱管路连接状态  1：未连接
	g_sensor.DaBian 			= (0==(IO1PIN&BIT17));
	g_sensor.WomenXiaoBian		= (0==(IO1PIN&BIT18));
	g_sensor.ManXiaoBian		= (0==(IO1PIN&BIT19));
	g_sensor.MaTongFull			= (0==(IO0PIN&BIT23));	// 马桶满信号
	// 出风口温度值 平时读取 1min/pcs
	if(g_wendu_time>WenDuJianCe_TIME)
	{
		//g_sensor.ChuanDianWenDu=ReadTemperature();
		g_wendu_time=0;
	}
	else
		g_wendu_time+=1;
	
	// 称重
			g_sensor.WuWuXiangZhongLiang	= 2000;

	
	// 便门打开或关闭 开启或关闭马桶内夜视灯，操作1次
	if(m_lastBianMen != g_bedSensor.BianMenKaiDaoDing)
	{			if(g_bedSensor.BianMenKaiDaoDing)
				{	TOILET_LAMP_WORK(1);}
				else
				{	TOILET_LAMP_WORK(0);}	
	}				
	m_lastBianMen = g_bedSensor.BianMenKaiDaoDing;
	
	// 餐桌推杆收放时，餐桌旋转减速电机同步工作
	// 位置1：到顶 10*TIME_RUN_DINING_MT，位置2：离开滑背位 2*TIME_RUN_DINING_MT ， 位置3：中间位 5*TIME_RUN_DINING_MT
	// 旋转餐桌需要时长 5s, 旋转机械臂需要时长TIME_RUN_DINING_MT =25s 只要1/5时间=2*TIME_RUN_DINING_MT
	if(rotateDiningDir == 1)		// 变餐桌
	{
		if(rotateDiningCount >= 10*TIME_ROTATE_DINING)
		{
			rotateDiningCount = 10*TIME_ROTATE_DINING;
			rotateDining(0,0);
		}else{
			rotateDiningCount += 1;
		}
	}else if(rotateDiningDir == 2)		//变靠背
	{
		if(rotateDiningCount == 0 || rotateDiningCount > 10*TIME_ROTATE_DINING)
		{	
			rotateDiningCount = 0;
			rotateDining(0,0); 
		}else{
			rotateDiningCount -= 1;
		}
	}
		g_sensor.rotateDiningCnt = rotateDiningCount;
	
	if(g_bedsensor_MT_work.dining == 1)   // 座椅模式下，变餐桌，diningCnt 增大 需要旋转电机工作，
	{
		if(g_bedsensor_MT_work.diningCnt > 4*TIME_RUN_DINING_MT)		// 0.1S一次采样 max = 10*TIME_RUN_DINING_MT   6*TIME_RUN_DINING_MT为起背和机械臂同时启动时的数据；
		{
				rt_kprintf("+");
				rotateDining(1,1);   // 变餐桌    如果线路板死机，这边动作，对人体不构成危险
				flag_rotate = 1;
		}	
	}else if(g_bedsensor_MT_work.dining == 2)    // 变靠背 ，diningCnt 减少
	{		
		if(g_bedsensor_MT_work.diningCnt < 9*TIME_RUN_DINING_MT)     // 确保机械臂缩回(抬高)一小段距离，旋转餐桌不碰撞人体
		{
			rt_kprintf("-");
			rotateDining(1,0);   // 变靠背			如果线路板死机，这边动作，对人体不构成危险
			flag_rotate = 1;
		}
	}else{									// 启动过了，暂停
		if(flag_rotate)
		{
			flag_rotate = 0;
			rotateDining(0,0);
		}			
	}
	
		
	// 每次水管接入，需要排空气一次,且污物管路接上
	if(!g_sensor.ShuiXiangLow && m_waterIO )   // 边沿触发  1-->0
	{
		m_flag_waterIO = 1;
	}
	if(m_flag_waterIO && g_sensor.WuWuXiangGuan)   // 条件满足时触发
	{	
		if(water_cnt == 10*5 )   // 5S后 启动
		{
			m_XSBDuty = 1000;
			ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开
			RE_SHUI_FANG_SHUI_FA(1);						// 热水废水水阀打开
			XIAO_SHUI_BENG(1);								// 小水泵打开		
			rt_kprintf("Start Pump\r\n");			
		}
		if(water_cnt > 10*(5+18))     // 延时18s		
		{
			XIAO_SHUI_BENG(0);	
			RE_SHUI_FANG_SHUI_FA(0);
			ZENG_YA_BENG_ZHU_SHUI_FA(0);
			m_flag_waterIO = 0;	
			water_cnt = 0;
			rt_kprintf("End Pump\r\n");
		}
		water_cnt += 1;
	}else{				// 5S插管时间内反复触发，重新计数
			water_cnt = 0;
	}
	m_waterIO = g_sensor.ShuiXiangLow;  // 刷新前一刻状态信息 1:未接
	
	DianJiDianLiuAD1 = (u16)GetAdcSmoothly(0);	// 10位AD采样背部和左翻
	// 1S内连续读取到过流信号相当于10次采样
	if(DianJiDianLiuAD1>Pai_Wu_Beng_Guoliu)
		DaDianJiGuoliuCount+=1;
	else
	{
		if(!DaDianJiGuoliuCount)
			DaDianJiGuoliuCount=0;
		else
			DaDianJiGuoliuCount-=1;
	}
	if(DaDianJiGuoliuCount>30)
	{PAI_WU_SUI_WU_BENG(0);DaDianJiGuoliuCount=30;}

}


// 传感器初始化
void SensorInit(void)
{
	SensorHwInit();
	memset((void*)&g_sensor,0,sizeof(g_sensor));
	memset((void*)&g_bedSensor,0,sizeof(g_bedSensor));

	m_lastSensorStatus =  g_sensor;
	
	m_waterIO = g_sensor.ShuiXiangLow = ((IO0PIN&BIT5)==0);

	rt_timer_init(&sensorTimer , "sensor", SensorCheck, RT_NULL, RT_TICK_PER_SECOND*0.1f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);		// 周期性调用
	rt_timer_start(&sensorTimer);		// 开始调用

	AutoSensor(0);
}

