/*******************************
	日志:
		2021.1.12.  
				1. 水路冲洗时间过短，去掉直冲，因为直冲没起到效果，马桶壁冲洗时间加长到18S
				2. 前部和后部调整，去掉男性前部的升出罩，前部放到下面位置；后部放后面；前部后部时间一致
				3. 增加液晶屏反转代码
				4. 去掉按键翻身功能，改为液晶屏反转，点动。
				5. 增加小水泵排空气时的水流检测
				6. 旋转餐桌电机延时启动
				7. 旋转屏电机控制信号的pa5改为PA24 ，飞线重新飞 
				8. pa5 和 pa2 用于红外检测，污物管路和水路管路的连接与否。软件代码编写，硬件实现。
		1.19.		去掉水泵后端的3路水阀，马桶壁水阀，直冲水阀和马桶圈清洗水阀；水泵直接连马桶壁清洗；
					去掉看门狗，防止系统初始化对人体的伤害
********************************/

#include <Actionthread.h>
#include <rtthread.h>
#include <finsh.h>
#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif
#include "CAN/IncludeCan.h"
#include "CAN/Can.h"
#include "lpc214x.h"
#include <SmsApp.h>
#include <Sensor.h>
#include "Pwm.h"
#include "SuReQi.H"
#include "Adc.h"
#include "CanAnalyzer.h"

extern unsigned char flag_qidong;
#define	LENG_SHUI_FANG_SHUI	10
// 冷水放水时长，单位秒
u8 m_LSSC = LENG_SHUI_FANG_SHUI;
u8 m_TSDK = 2;
//s32 m_SlowStep = SLOW_STEP;	// 慢喷步数
#ifdef RT_USING_FINSH
FINSH_VAR_EXPORT(m_LSSC, finsh_type_uchar, "冷水放水时长，单位秒");
FINSH_VAR_EXPORT(m_TSDK, finsh_type_uchar, "冷水放水进水同时打开时长，单位秒");
//FINSH_VAR_EXPORT(m_SlowStep, finsh_type_int, "管路分配器慢喷步数");
#endif

// 热水小水泵pwm占空比
u16 m_XSBDuty	= 500;
u16 set_XSBDuty = 1000;
u8 flag_set_qiben =1;
#ifdef RT_USING_FINSH
FINSH_VAR_EXPORT(m_XSBDuty, finsh_type_ushort, "热水小水泵占空比");
#endif


// 速热器使能，0=关闭，1=起始状态，2=加速状态
static u8 m_SuReQiState = 0;
// 速热器放冷水时长 
static u8 m_SuReQiLengShuiTimer = 0;

u8 rotateDiningCount = 0;   // TIME_ROTATE_DINING=8   80 < 255
u8 rotateDiningDir;  //电机的工作方向

// 动作参数变量
volatile s_SetAConfig  Set_A_Config={0};
volatile s_SetBConfig  Set_B_Config={0};
volatile t_ActionConfig m_ActionCfg={0};
volatile u8 Set_MianBan_Config_ShuiWen=0;
volatile u8 Set_MianBan_Config_NuanFeng=0;
volatile u8 Set_MianBan_Config_QingXi=0;
volatile u8 PowerWorkFlag=0;		// 加热垫相关

#define TIME_PAI_WU_BENG 6		//排污泵一次启动时长 0.5S/PCS    排污泵1S == 水泵 3S 水量
#define TIME_ADD_FLUSH  7			//增加水泵冲洗时间    实际水泵流量 0.138 L/S

#define QING_XI_PENG_TOU   700// 250			 // 清洗喷头力度
#define QIAN_BU_CHONG_XI_LI_DU  900//700      // 前部清洗力度
// 市电总控制开关
#define SHI_DIAN_ZONG_KAI_GUAN(on) if(on) {IO1SET = BIT28;} else {IO1CLR = BIT28;}
// 烘干屁股电热丝
#define HONG_GAN_PI_GU_DIAN_RE_SI(on)	if(on) {IO0SET = BIT20;} else {IO0CLR = BIT20;}
// 烘干屁股风扇
#define HONG_GAN_PI_GU_FENG_SHAN(on)	if(on) {IO0SET = BIT12;} else {IO0CLR = BIT12;}

// 热水小气泵
//#define RE_SHUI_XIAO_QI_BENG(on)	if(on) {PwmChange(5, m_XSBDuty);} else {PwmChange(5,0);}
#define XIAO_QI_BENG(on)	if(on) {IO0CLR = BIT10;} else {IO0CLR = BIT10;}

// 热水前部清洗水阀
#define RE_SHUI_QIAN_BU_SHUI_FA(on)	if(on) {IO0SET = BIT16;} else {IO0CLR = BIT16;}
// 热水后部清洗水阀
#define RE_SHUI_HOU_BU_SHUI_FA(on)	if(on) {IO0SET = BIT17;} else {IO0CLR = BIT17;}
// 冲洗马桶-壁
//#define CHONG_XI_SHUI_FA_TO_MATONG(on)	if(on) {IO1SET = BIT22;} else {IO1CLR = BIT22;}
#define CHONG_XI_SHUI_FA_TO_MATONG(on)	if(on) {IO1CLR = BIT22;} else {IO1CLR = BIT22;}
// 冲洗马桶-直冲水阀
//#define ZHI_CHONG_SHUI_FA(on)	if(on) {IO1SET = BIT21;} else {IO1CLR = BIT21;}
#define ZHI_CHONG_SHUI_FA(on)	if(on) {IO1CLR = BIT21;} else {IO1CLR = BIT21;}
// 加热片 右手边
#define JIA_RE_PIAN_YOU(on) if(on) {IO1SET = BIT27;} else {IO1CLR = BIT27;}
// 加热片 中上边
#define JIA_RE_PIAN_ZHONG_UP(on) if(on) {IO1SET = BIT25;} else {IO1CLR = BIT25;}
// 加热片 中下边
#define JIA_RE_PIAN_ZHONG_DOWN(on) if(on) {IO1SET = BIT24;} else {IO1CLR = BIT24;}
// 加热片 左手边
#define JIA_RE_PIAN_ZUO(on) if(on) {IO1SET = BIT23;} else {IO1CLR = BIT23;}
// 小夜灯开关
#define NIGHT_LAMP_WORK(on) if(on) {IO1SET = BIT20;} else {IO1CLR = BIT20;}

static struct rt_thread thread_rotate;
static char thread_rotate_stack[512];

static void DoVacuumPump(u8 timeS);
// 平板显示器旋转  p0.19 ; P0.05 ;// pa5改为PA24
void rotatePAD(u8 work,u8 dir)
{
	if(work)
	{
		if(dir)
		{
			IO0SET = BIT19;
			IO0CLR = BIT24;
		}
		else
		{
			IO0CLR = BIT19;
			IO0SET= BIT24;
		}
	}
	else
	{
		IO0CLR=BIT19|BIT24;
	}
}

// 旋转餐桌电机  p1.29 ; P0.18 ;
void rotateDining(u8 work,u8 dir)
{
	if(work)
	{
		if(dir)
		{
			rotateDiningDir = 1;
			IO0SET = BIT18;
			IO1CLR = BIT29;
		}
		else
		{
			rotateDiningDir = 2;
			IO0CLR = BIT18;
			IO1SET= BIT29;
		}
	}
	else
	{
		rotateDiningDir = 0;
		IO0CLR = BIT18;
		IO1CLR = BIT29;	
	}
}
void rotateTable(u8 work,u8 dir)
{
	rt_kprintf("rotateTable(%d,%d)\r\n",work,dir);
	rotateDining(work,dir);
}

#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(rotateTable, "旋转餐桌,work,dir");
#endif

void ActionStartCmd(ActionCmd cmd);


// 加热模块
static void JiaReModule(int work)
{
	if(work)
	{
		JIA_RE_PIAN_YOU(1);
		JIA_RE_PIAN_ZHONG_UP(1);
		JIA_RE_PIAN_ZHONG_DOWN(1);
		JIA_RE_PIAN_ZUO(1);
	}
	else
	{
		JIA_RE_PIAN_YOU(0);
		JIA_RE_PIAN_ZHONG_UP(0);
		JIA_RE_PIAN_ZHONG_DOWN(0);
		JIA_RE_PIAN_ZUO(0);
	}
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(JiaReModule, "床垫加热[],work");
#endif

// 小水路
static void Xiaoshuibeng(int work,int dir,int pwm)
{
	m_XSBDuty=pwm;
	if(work)
	{
		ZENG_YA_BENG_ZHU_SHUI_FA(1);
		if(0 == dir)
		{	
			RE_SHUI_FANG_SHUI_FA(1);
		}else if(1==dir)
		{
			RE_SHUI_QIAN_BU_SHUI_FA(1);
		}else	if(2==dir)
		{	
			RE_SHUI_HOU_BU_SHUI_FA(1);
			XIAO_QI_BENG(1);
		}
		XIAO_SHUI_BENG(1);
	}
	else
		{XIAO_SHUI_BENG(0);XIAO_QI_BENG(0);RE_SHUI_FANG_SHUI_FA(0);RE_SHUI_QIAN_BU_SHUI_FA(0);RE_SHUI_HOU_BU_SHUI_FA(0);ZENG_YA_BENG_ZHU_SHUI_FA(0);}
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(Xiaoshuibeng, "小水泵[][][],0关,PWM1000");
#endif

//增压泵打开
static void Chongximatong(int beng)
{
	if(beng)
	{
//	CHONG_XI_SHUI_FA_TO_MATONG(1);	// 马桶冲洗侧壁
	ZHI_CHONG_SHUI_FA(1);				// 马桶冲洗直冲
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);}		
	else
	{
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(0);
	ZHI_CHONG_SHUI_FA(0);
	CHONG_XI_SHUI_FA_TO_MATONG(0);}
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(Chongximatong, "冲洗马桶0关");
#endif

//增压泵打开
static void Chongxizuoquan(int beng)
{
	if(beng)
	{CHONG_XI_SHUI_FA_ZUOBIANQUAN(1);QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);}		
	else
	{QING_SHUI_CHONG_XI_ZENG_YA_BENG(0);CHONG_XI_SHUI_FA_ZUOBIANQUAN(0);}
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(Chongxizuoquan, "冲洗座便圈0关");
#endif

//排污泵打开
static void Paiwubeng(int beng)
{
	if(beng)
	{PAI_WU_SUI_WU_BENG(1);}		
	else
	{PAI_WU_SUI_WU_BENG(0);}
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(Paiwubeng, "排污泵0关");
#endif


// 阀切换
static void Fa(int fa)
{
	RE_SHUI_FANG_SHUI_FA(fa&BIT0);	 		// 热水水路放冷水阀
	RE_SHUI_QIAN_BU_SHUI_FA(fa&BIT1);  		// 热水水路前部阀
	RE_SHUI_HOU_BU_SHUI_FA(fa&BIT2);  		// 热水水路后部阀
	CHONG_XI_SHUI_FA_TO_MATONG(fa&BIT3); 	// 冲洗马桶水路 --侧壁
	ZHI_CHONG_SHUI_FA(fa&BIT3);				// 冲洗马桶--直冲
	CHONG_XI_SHUI_FA_ZUOBIANQUAN(fa&BIT4);  // 冲洗座便器
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(Fa, "阀切换，[Bit0=冷],[BIT1=前],[Bit2=后],[BIT3=内],[BIT4=外]");
#endif

//暖风
static void Nuanfeng(int work,int jiare)
{
	if(work)
	{
		HONG_GAN_PI_GU_FENG_SHAN(1);
		if(jiare)
		{SHI_DIAN_ZONG_KAI_GUAN(1);HONG_GAN_PI_GU_DIAN_RE_SI(1);}
	}		
	else
	{HONG_GAN_PI_GU_DIAN_RE_SI(0);HONG_GAN_PI_GU_FENG_SHAN(0);SHI_DIAN_ZONG_KAI_GUAN(0);}
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(Nuanfeng, "暖风烘干[][]0关");
#endif

// 停止所有动作
void DoStop(void)
{
	CHONG_XI_SHUI_FA_ZUOBIANQUAN(0);
	CHONG_XI_SHUI_FA_TO_MATONG(0);
	ZHI_CHONG_SHUI_FA(0);
	PAI_WU_SUI_WU_BENG(0);
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(0);

	HONG_GAN_PI_GU_DIAN_RE_SI(0);
	SHI_DIAN_ZONG_KAI_GUAN(0);
	HONG_GAN_PI_GU_FENG_SHAN(0);
	
	RE_SHUI_FANG_SHUI_FA(0);
	RE_SHUI_QIAN_BU_SHUI_FA(0);
	RE_SHUI_HOU_BU_SHUI_FA(0);
	XIAO_QI_BENG(0);
	XIAO_SHUI_BENG(0);
	ZENG_YA_BENG_ZHU_SHUI_FA(0);
	
	ZHU_SHUI_SHUI_BENG(0);
	ZENG_YA_BENG_ZHU_SHUI_FA(0);
	JiaReModule(0);
}

// 看门狗初始化
// 2880000 ,1秒
void WDInit(void)
{
//	WDTC = 0x05265C00;	// 设置看门狗定时器的固定装载值 : 30 秒
//	WDMOD= 0X03;   // 模式设定
//	feedWD();
}

void feedWD(void) // 喂狗
{
//	WDFEED =0XAA;
//	WDFEED =0X55;
}

u32 WDtimers(void)
{
	return WDTV; //看门狗定时器的当前值
}


// 硬件初始化
static void ActionHwInit(void)
{
	rotatePAD(0,0);
	rotateDining(0,0);
	NIGHT_LAMP_WORK(0);
	TOILET_LAMP_WORK(0);
	DoStop();
	PwmInit();	
	DoStop();
}


// 邮箱相关变量
static struct rt_mailbox m_mb;
static rt_uint32_t mbPool[1];
// 事件
static struct rt_event m_evt;

static void ActionInit(void)
{
	u8 buffer[7];
	// 硬件初始化
	ActionHwInit();
	// 变量初始化
	Set_A_Config.ShuiWen = 2;								// 面板A控制器设置清洗水温温度 0-3 档位控制
	Set_A_Config.NuanFen = 3;								// 面板A控制器设置暖风温度 0-3 档位控制
	Set_A_Config.ShuiLiu = 3;								// 面板A控制器设置清洗强度 1-3 档位控制

	Set_B_Config.ShuiWen = 2;								// 面板B控制设置清洗水温温度 单位1℃，0-41℃
	Set_B_Config.NuanFen = 3;								// 面板B控制设置暖风温度 单位1℃，40-65℃
	Set_B_Config.ShuiLiu = 3;								// 面板B控制设置清洗水流强度1-3 档位控制
	Set_B_Config.ZhiNengModel=0;							// 智能模式开关
	Set_B_Config.ChuangDianJiaRe=38;						// 设定默认温度在38℃

	Set_MianBan_Config_ShuiWen = 2;
	Set_MianBan_Config_NuanFeng = 3;
	Set_MianBan_Config_QingXi= 3;

	Set_B_Config.ChuangDianPower =0;
	Set_B_Config.NightLampSwitch =0;
	Set_B_Config.ToiletLampSwitch =0;
	
	m_ActionCfg.BianPenZiDongPaiWuYanShi = 2;				// 便盆自动排污时间，单位秒，0-60000有效
	m_ActionCfg.BianPenZiDongPauWuJianCeYanShi = 30;		// 便盆自动排污检测延时，单位秒，0-60000有效
	m_ActionCfg.BianPenShouDongQingJieShiJian = 7;			// 便盆手动清洁时间，单位秒，0-60000有效
	m_ActionCfg.HongGanShiJian = 30;						// 烘干时间，单位秒，范围0-60000有效
	m_ActionCfg.TunBuQingXiShiJian = 40;					// 臀部清洗时间，单位秒，范围0-60000有效
	m_ActionCfg.QianBuYanShi = 18;							// 前部清洗时间，单位秒，范围0-60000有效
	m_ActionCfg.MaTongQuanYanShi= 8;						// 马桶圈冲洗时间，单位秒，范围0-60000有效

	g_sensor.FunctionWorkAuto=0;
	g_sensor.FunctionWorkHouBu=0;
	g_sensor.FunctionWorkMaTong=0;
	g_sensor.FunctionWorkNuanFeng=0;
	g_sensor.FunctionWorkQianBu=0;

	SuReQiInit();
	SuReQiSet(0,38);
	rt_thread_delay(RT_TICK_PER_SECOND*0.3f);
	SuReQiRead(buffer); 
	g_sensor.ReShuiWenDu = buffer[3];
	g_sensor.LengShuiWenDu= buffer[2];
	JiaReModule(0);
	// 初始化邮箱
	rt_mb_init(&m_mb, "act mb", mbPool, sizeof(mbPool)/sizeof(rt_uint32_t), RT_IPC_FLAG_FIFO);
	// 初始化事件
	rt_event_init(&m_evt, "act evt", RT_IPC_FLAG_FIFO);
	
}

void ActionSetBianPenZiDongPaiWuYanShi(u16 val)
{
	if(val&&(val<60000)&&(m_ActionCfg.BianPenZiDongPaiWuYanShi!= val))
	{
		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		m_ActionCfg.BianPenZiDongPaiWuYanShi = val;
	}
}
void ActionSetBianPenZiDongPauWuJianCeYanShi(u16 val)
{
	if(val&&(val<60000)&&(m_ActionCfg.BianPenZiDongPauWuJianCeYanShi!= val))
	{
		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		m_ActionCfg.BianPenZiDongPauWuJianCeYanShi= val;
	}
}
void ActionSetBianPenShouDongQingJieShiJian(u16 val)
{
	if(val&&(val<60000)&&(m_ActionCfg.BianPenShouDongQingJieShiJian!= val))
	{
		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		m_ActionCfg.BianPenShouDongQingJieShiJian= val;
	}
}
void ActionSetMaTongQuanQingJieYanShi(u16 val)
{
	if(val&&(val<60000)&&(m_ActionCfg.MaTongQuanYanShi!= val))
	{
		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		m_ActionCfg.MaTongQuanYanShi= val;
	}
}

void ActionSetHongGanShiJian(u16 val)
{
	if(val&&(val<60000)&&(m_ActionCfg.HongGanShiJian!= val))
	{
		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		m_ActionCfg.HongGanShiJian= val;
	}
}
void ActionSetTunBuQingXiShiJian(u16 val)
{
	if(val&&(val<60000)&&(m_ActionCfg.TunBuQingXiShiJian!= val))
	{
		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		m_ActionCfg.TunBuQingXiShiJian= val;
	}
}

void ActionSetQianBuQingXiShiJian(u16 val)
{
	if(val&&(val<60000)&&(m_ActionCfg.QianBuYanShi!= val))
	{
		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		m_ActionCfg.QianBuYanShi= val;
	}
}

void ActionSetZuoBianQuanQingXiShiJian(u16 val)
{
	if(val&&(val<60000)&&(m_ActionCfg.MaTongQuanYanShi!= val))
	{
		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		m_ActionCfg.MaTongQuanYanShi= val;
	}
}

void ActionMianBanASetShuiWen(u8 val)
{
	if((val<=3)&&(Set_A_Config.ShuiWen != val))
	{
		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		if(!Set_B_Config.HoldOn)	
		{	Set_A_Config.ShuiWen = val;Set_MianBan_Config_ShuiWen=val;}
//		ActionStartCmd(SheZhiShuiWen);
	}
}

void ActionMianBanBSetShuiWen(u8 val)
{
	if((val<=3)&&(Set_B_Config.ShuiWen!= val))
	{
	//	DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
	if(!Set_A_Config.HoldOn)
	{	Set_B_Config.ShuiWen = val;Set_MianBan_Config_ShuiWen=val;}
	//	ActionStartCmd(SheZhiShuiWen);
	}
}

void ActionMianBanASetNuanFengWenDu(u8 val)
{
	if((val<=3)&&(Set_A_Config.NuanFen!= val))
	{
//		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		if(!Set_B_Config.HoldOn)
		{	Set_A_Config.NuanFen = val;Set_MianBan_Config_NuanFeng=val;}
	}
}

void ActionMianBanBSetNuanFengWenDu(u8 val)
{
	if((val<=3)&&(Set_B_Config.NuanFen!= val))
	{
//		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		if(!Set_A_Config.HoldOn)
		{	Set_B_Config.NuanFen= val;Set_MianBan_Config_NuanFeng=val;}
	//	ActionStartCmd(SheZhiShuiWen);
	}
}

void ActionMianBanASetShuiLiuQiangDu(u8 val)
{
	if((val<=3)&&(Set_A_Config.ShuiLiu!= val))
	{
//		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		if(!Set_B_Config.HoldOn)
		{	Set_A_Config.ShuiLiu= val;Set_MianBan_Config_QingXi=val;}
	}
}
void ActionMianBanBSetShuiLiuQiangDu(u8 val)
{
	if((val<=3)&&(Set_B_Config.ShuiLiu!= val))
	{
//		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		if(!Set_A_Config.HoldOn)
		{	Set_B_Config.ShuiLiu= val;Set_MianBan_Config_QingXi=val;}
	//	ActionStartCmd(SheZhiShuiWen);
	}
}
void ActionMianBanBSetZhiNengModel(u8 val)
{
	if((Set_B_Config.ZhiNengModel!= val))
	{
//		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		Set_B_Config.ZhiNengModel= val;
	//	ActionStartCmd(SheZhiShuiWen);
	}
}

void ActionMianBanBSetChuangDianPower(u8 val)
{
	if((Set_B_Config.ChuangDianPower!= val))
	{
//		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
		Set_B_Config.ChuangDianPower= val;
		if(Set_B_Config.ChuangDianPower)
			ActionStartCmd(ChuangDianJiaReWork);
		else
			ActionStartCmd(ChuangDianJiaReEnd); 				
	}
}

void ActionMianBanBSetToiletLampSwitch(u8 val)
{
	TOILET_LAMP_WORK(val);
}

void ActionMianBanBSetNightLampSwitch(u8 val)
{
	NIGHT_LAMP_WORK(val);
}

void ActionMianBanBSetSwitchControl(void)
{	
		ActionMianBanBSetShuiWen(Set_B_Config.ShuiWen);
		ActionMianBanBSetNuanFengWenDu(Set_B_Config.NuanFen);
		ActionMianBanBSetShuiLiuQiangDu(Set_B_Config.ShuiLiu);
		ActionMianBanBSetChuangDianWenDu(Set_B_Config.ChuangDianJiaRe);
		ActionMianBanBSetZhiNengModel(Set_B_Config.ZhiNengModel);
//			pData+=1;	// 植物人模式暂时不支持
		ActionMianBanBSetChuangDianPower(Set_B_Config.ChuangDianPower);
		ActionMianBanBSetToiletLampSwitch(Set_B_Config.ToiletLampSwitch);
		ActionMianBanBSetNightLampSwitch(Set_B_Config.NightLampSwitch);
}



void ActionMianBanBSetChuangDianWenDu(u8 val)
{
	if((val>10)&&(val<60)&&(Set_B_Config.ChuangDianJiaRe!= val))
	{
//		DbgPrintf("%s(%d)\r\n",__FUNCTION__,(u32)val);
			Set_B_Config.ChuangDianJiaRe= val;
	}
}


// 开始动作命令
void ActionStartCmd(ActionCmd cmd)
{
	static ActionCmd cmdbuf = ActionCmdNone;
	if(cmd != cmdbuf)  // 换命令前清除其余动作 
	{
		cmdbuf = cmd;
	}

#if 0
	if(cmd != ActionCmdNone)
	{
		DbgPrintf("cmd=%d\r\n",cmd);
	}
#endif

	switch(cmd)
	{
	case ActionCmdNone:
		flag_qidong=0;
		break;
	default:
	//	DbgPrintf("发送消息到动作进程\r\n");
		rt_mb_send(&m_mb, cmd);
		break;
	}
}
FINSH_FUNCTION_EXPORT(ActionStartCmd,"开始命令[cmd:1=冲洗便盆，2=清洗臀部，3=干燥]");

// 停止动作命令
void ActionStopCmd(ActionCmd cmd)
{
	rt_event_send(&m_evt, 1<<cmd);
}
FINSH_FUNCTION_EXPORT(ActionStopCmd, "停止命令 [cmd:0=停止所有动作，1=冲洗便盆，2=清洗臀部，3=干燥]");

// 等待，如果返回非0表示该函数是由于外部停止动作命令而导致的返回
static int WaitTimeout(rt_int32_t timeout,rt_uint32_t set)
{
	rt_uint32_t evt;
	if(rt_event_recv(&m_evt,set , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, timeout, &evt) == RT_EOK)
	{
		rt_kprintf("手动退出\r\n");
		return -1;		// 接收到退出信号
	}
	else
	{
		return 0;		// 正常超时
	}
}

// 判断便门是否打开，0表示未打卡，1表示已打开
static int IsOpen(void)
{
	if( g_bedSensor.BianMenKaiDaoDing) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// 判断便门是否已关闭，0表示未打开，1表示已打开
static int IsClose(void)
{
	if( g_bedSensor.BianMenGuanDaoDi)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// 判断水箱以及污物箱是否满足冲水条件
static int AllowChongShui(void)
{
	if((g_sensor.WuWuXiangZhongLiang>=WU_WU_XIANG_MAN)||(g_sensor.ShuiXiangLow==1)||(g_sensor.WuWuXiangGuan==0)||(g_sensor.MaTongFull==1))
	{
		rt_kprintf("清水不足或污物箱已满或污物箱不在位置或马桶满\r\n");
		return 0;
	}
 	else if(!IsOpen())
	{
		rt_kprintf("便门未打开\r\n");
		return 0;
	}
	else
	{
		return 1;
	}
}
// 判断水箱以及污物箱是否满足冲水条件
static int AllowChongMaTong(void)
{
	if((g_sensor.WuWuXiangZhongLiang>=WU_WU_XIANG_MAN)||(g_sensor.ShuiXiangLow==1)||(g_sensor.WuWuXiangGuan==0))
	{
		rt_kprintf("清水不足或污物箱已满或污物箱不在位置或马桶满\r\n");
		return 0;
	}
 	else if(!IsOpen())
	{
		rt_kprintf("便门未打开\r\n");
		return 0;
	}
	else
	{
		return 1;
	}
}

// 水封
static void DoShuiFeng(void)
{
	CHONG_XI_SHUI_FA_TO_MATONG(1);
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);
	rt_thread_delay(RT_TICK_PER_SECOND);	// 1 秒
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(0);
	CHONG_XI_SHUI_FA_TO_MATONG(0);
	ZHI_CHONG_SHUI_FA(0);
}

// 把增压泵抽真空，排出内部空气
static void DoVacuumPump(u8 timeS)
{
	u16 i,cnt;
	u8 buff[7];
	u16 templiuliang=0;
	cnt=0;
	if((g_sensor.ShuiXiangLow==1)||(g_sensor.WuWuXiangGuan==0))
	{
		rt_kprintf("清水不足或或污物管路未连接\r\n");
		return ;
	}	
	m_XSBDuty = 1000;
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开
	RE_SHUI_FANG_SHUI_FA(1);						// 热水废水水阀打开
	XIAO_SHUI_BENG(1);								// 小水泵打开
	
	for(i=0;i<2*timeS;i++)
	{			
		if(WaitTimeout(100, (1<<STOP)|(1<<ChongXiBianPeng)))
		{
			XIAO_SHUI_BENG(0);	
			RE_SHUI_FANG_SHUI_FA(0);
			ZENG_YA_BENG_ZHU_SHUI_FA(0);
			return;
		}	
	}
	XIAO_SHUI_BENG(0);	
	RE_SHUI_FANG_SHUI_FA(0);
	ZENG_YA_BENG_ZHU_SHUI_FA(0);
	rt_kprintf("End");
}

// 初始化旋转餐桌变靠背 调用延时函数，不耽误其他动作执行
// 增压小水泵打水，用于排空气
static void thread1_entry(void* parameter)
{
		rt_kprintf("rotate Dining\r\n");
	//等待床体控制板3S  如果是座椅模式餐桌放平，坐便器顶住餐桌，需要等背部降一点后启动
  rt_thread_delay(3*RT_TICK_PER_SECOND);
  
	rotateDiningCount = 10*TIME_ROTATE_DINING ; // 初始化最大值
	rt_kprintf("Start 变靠背 \r\n");
	rotateDining(1,0);		// 变靠背

	rt_thread_delay(TIME_ROTATE_DINING*RT_TICK_PER_SECOND); // 运行时长 = TIME_ROTATE_DINING 秒
	
	rt_kprintf("End 完成靠背\r\n");
	rotateDiningCount=0;
	rotateDining(0,0);
	
	// 增压小水泵打水，用于排空气
	DoVacuumPump(25);	
}

// 冲洗便盆  水量=设定时间*0.33L/S 如9S=3L水，排水水泵比进水快1倍  用水量1.2L,用时10S
static void DoChongXiBianPeng(void)
{
	if(!AllowChongMaTong())
	{
		return;
	}
	g_sensor.FunctionWorkMaTong =1;
	rt_kprintf("开始冲洗便盆\r\n");
	if(g_sensor.MaTongFull == 1)	// 直接开启排污泵
	{
		PAI_WU_SUI_WU_BENG(1);				// 打开排污泵
		if(WaitTimeout(TIME_PAI_WU_BENG*RT_TICK_PER_SECOND/2, (1<<STOP)|(1<<ChongXiBianPeng)))
		{
			PAI_WU_SUI_WU_BENG(0);				
			g_sensor.FunctionWorkMaTong =0;
			return;
		}	
		PAI_WU_SUI_WU_BENG(0);				// 再关排污泵
		g_sensor.FunctionWorkMaTong =0;		
		return;		
	}

//	CHONG_XI_SHUI_FA_TO_MATONG(0);			// 打开到马桶大水阀
//	ZHI_CHONG_SHUI_FA(1);					// 打开马桶冲洗-直冲阀
	CHONG_XI_SHUI_FA_TO_MATONG(1);		// 打开侧壁冲洗
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);		// 打开增压水泵
//	if(WaitTimeout(4*RT_TICK_PER_SECOND, (1<<STOP)|(1<<ChongXiBianPeng)))
//	{
//		QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 
//		PAI_WU_SUI_WU_BENG(0);				
//		CHONG_XI_SHUI_FA_TO_MATONG(0);			
//		ZHI_CHONG_SHUI_FA(0);					// 关闭马桶冲洗-直冲阀
//		g_sensor.FunctionWorkMaTong =0;
//		return;
//	}
//	CHONG_XI_SHUI_FA_TO_MATONG(1);		// 打开侧壁冲洗
//	ZHI_CHONG_SHUI_FA(0);					// 关闭马桶冲洗-直冲阀
	if(WaitTimeout((m_ActionCfg.BianPenShouDongQingJieShiJian+TIME_ADD_FLUSH)*RT_TICK_PER_SECOND, (1<<STOP)|(1<<ChongXiBianPeng)))
	{
		QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 
		PAI_WU_SUI_WU_BENG(0);				
		CHONG_XI_SHUI_FA_TO_MATONG(0);			
		ZHI_CHONG_SHUI_FA(0);					// 关闭马桶冲洗-直冲阀
		g_sensor.FunctionWorkMaTong =0;
		return;
	}
	PAI_WU_SUI_WU_BENG(1);				// 打开排污泵
	if(WaitTimeout(TIME_PAI_WU_BENG*RT_TICK_PER_SECOND/2, (1<<STOP)|(1<<ChongXiBianPeng)))
	{
		QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 
		PAI_WU_SUI_WU_BENG(0);				
		CHONG_XI_SHUI_FA_TO_MATONG(0);			
		ZHI_CHONG_SHUI_FA(0);					// 关闭马桶冲洗-直冲阀
		g_sensor.FunctionWorkMaTong =0;
		return;
	}
//	QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 	// 先关水泵
//	CHONG_XI_SHUI_FA_TO_MATONG(0);			
//	rt_thread_delay(RT_TICK_PER_SECOND/2);	// 1/2 秒 不要同时开，启动功率过大
	PAI_WU_SUI_WU_BENG(0);				// 再关排污泵
	DoShuiFeng();						// 防止臭味
	g_sensor.FunctionWorkMaTong =0;

}

// 调节清洗强度
//static void TiaoJieLiDu(void)
//{
//   if(Set_A_Config.ShuiLiu==1)	   //关闭气泵 水泵 70%
//		{set_XSBDuty=700;flag_set_qiben=0;}
//   if(Set_A_Config.ShuiLiu==2)	   // 关气泵   水泵 100% 
//		{set_XSBDuty=1000;flag_set_qiben=0;}   
//   if(Set_A_Config.ShuiLiu==3)	   // 开气泵 水泵 100% 
//   		{set_XSBDuty=1000;flag_set_qiben=1;}
//}
static void TiaoJieLiDu(void)
{
   if(Set_MianBan_Config_QingXi==1)	   //关闭气泵 水泵 70%
		{set_XSBDuty=700;flag_set_qiben=0;}
   if(Set_MianBan_Config_QingXi==2)	   // 关气泵   水泵 100% 
		{set_XSBDuty=1000;flag_set_qiben=0;}   
   if(Set_MianBan_Config_QingXi==3)	   // 开气泵 水泵 100% 
   		{set_XSBDuty=1000;flag_set_qiben=1;}
}


// 调节水温
//static u8 TiaoJieShuiWen(void)
//{
//	u8 set_tt;
//   if(Set_A_Config.ShuiWen==0)	   // 冷水
//	   set_tt=0;
//   if(Set_A_Config.ShuiWen==1)	   // 35°
//	   set_tt=35;
//   if(Set_A_Config.ShuiWen==2)	   // 38°
//	   set_tt=38; 	   
//   if(Set_A_Config.ShuiWen==3)	   // 41° 
//	   set_tt=41; 		   
//	if(set_tt>42)
//		set_tt = 42;	// 设定最高温
//	return set_tt;
//}
static u8 TiaoJieShuiWen(void)
{
	u8 set_tt;
   if(Set_MianBan_Config_ShuiWen==0)	   // 冷水
	   set_tt=0;
   if(Set_MianBan_Config_ShuiWen==1)	   // 35°
	   set_tt=35;
   if(Set_MianBan_Config_ShuiWen==2)	   // 38°
	   set_tt=38; 	   
   if(Set_MianBan_Config_ShuiWen==3)	   // 41° 
	   set_tt=41; 		   
	if(set_tt>42)
		set_tt = 42;	// 设定最高温
	return set_tt;
}

//static u8 TiaoJieNuanFen(void)
//{
//	u8 set_tt;
//	if(Set_A_Config.NuanFen==0) 	// 冷风
//		set_tt=0;
//	if(Set_A_Config.NuanFen==1) 	// 45°半功率
//		set_tt=40;
//	if(Set_A_Config.NuanFen==2) 	// 55°
//		set_tt=60;	
////		set_tt=80;		
//	if(Set_A_Config.NuanFen==3) 	// 65° 全功率开
//		set_tt=80;	
////	set_tt=100;				
//	return set_tt;
//}

static u8 TiaoJieNuanFen(void)
{
	u8 set_tt;
	if(Set_MianBan_Config_NuanFeng==0) 	// 冷风
		set_tt=0;
	if(Set_MianBan_Config_NuanFeng==1) 	// 45°半功率
		set_tt=45;
	if(Set_MianBan_Config_NuanFeng==2) 	// 55°
		set_tt=65;	
//		set_tt=80;		
	if(Set_MianBan_Config_NuanFeng==3) 	// 65° 全功率开
		set_tt=85;	
//		set_tt=100;				
	return set_tt;
}

// 干燥
static void DoGanZao(void)
{
	unsigned int i;
	u8 j,tmp;
	u8 d_set_tt;

	if(!IsOpen())
	{
		rt_kprintf("便门未打开\r\n");
		return;
	}
	g_sensor.FunctionWorkNuanFeng=1;
	rt_kprintf("开始烘干动作\r\n");
	// 启动继电器，开始加热，启动风扇，并且开始读取温度值，工作时间为设定时间
	SHI_DIAN_ZONG_KAI_GUAN(1);		// 开启市电输入
	HONG_GAN_PI_GU_FENG_SHAN(1);	// 开启暖风风扇
	for(i=0;i<(m_ActionCfg.HongGanShiJian);i++)   //  1S一个周期
	{
		d_set_tt= TiaoJieNuanFen();
		// 2 读取出风口温度值
		//g_sensor.ChuFengWenDu =	ReadWenDu();

	 // 3 温度数据对比调整PWM
//		if(!d_set_tt)		// 在非冷风模式下
//		{
//			tmp = (d_set_tt-50)/2+40;	// 转温度设定值
//			// 如果温度值未达到设定值，并且1S内未见温度上调，重新调整设定值
//			if(g_sensor.ChuFengWenDu<tmp)
//			{	
//				tmp=tmp-g_sensor.ChuFengWenDu;
//				if(tmp>=10)
//					d_set_tt+=4;
//				if((tmp>5)&&(tmp<10))
//					d_set_tt+=2;
//			}
//			// 温度过高 ，下调
//			if(g_sensor.ChuFengWenDu>d_set_tt)
//			{	
//				tmp=g_sensor.ChuFengWenDu-d_set_tt;
//				if(tmp>=10)
//					d_set_tt-=4;
//				if((tmp>5)&&(tmp<10))
//					d_set_tt-=2;
//			}
//		}
		
		if(d_set_tt>100)
			d_set_tt=100;
//		if((d_set_tt<20)&&(!d_set_tt))
//			d_set_tt=20;
		HONG_GAN_PI_GU_DIAN_RE_SI(1);	// 开启暖风电热丝，电热丝控制采用模拟PWM2秒一周期，100mS一个单位
		for(j=0;j<100;j++)	
		{
		 if(j>=d_set_tt)	//如果到达设定值 关闭加热
		 {	HONG_GAN_PI_GU_DIAN_RE_SI(0);}
		 if(WaitTimeout(RT_TICK_PER_SECOND*0.01f, (1<<STOP)|(1<<GanZao)))
		 {
			 HONG_GAN_PI_GU_DIAN_RE_SI(0);
			 SHI_DIAN_ZONG_KAI_GUAN(0);	
			 HONG_GAN_PI_GU_FENG_SHAN(0);		
			 g_sensor.FunctionWorkNuanFeng=0;			 
			 return;
		 }
		}
	}
	HONG_GAN_PI_GU_DIAN_RE_SI(0);
	SHI_DIAN_ZONG_KAI_GUAN(0); 
	HONG_GAN_PI_GU_FENG_SHAN(0);  
	g_sensor.FunctionWorkNuanFeng=0;
	flag_ganzao=0;
}


// 清洗臀部   用水量 0.72L 用时40S
static void DoQingXiTunBu(void)
{
	u16 i;u8 buff[7];
	u16 templiuliang=0;
	u8 d_set_tt;	
	if(!AllowChongShui())
	{
		return;
	}
	g_sensor.FunctionWorkHouBu=1;
	rt_kprintf("开始清洗臀部\r\n");
	// 1 先放冷水
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开
	RE_SHUI_FANG_SHUI_FA(1);
	m_XSBDuty=1000;
	XIAO_SHUI_BENG(1);
	SuReQiSet(1,38);	
	rt_thread_delay(RT_TICK_PER_SECOND*0.1f);
	SuReQiRead(buff); 
	m_SuReQiState=1;	// 进入线程static void ReShuiTimer(void *p)
	m_SuReQiLengShuiTimer=0;
	rt_thread_delay(50); 		// 速热器彼此通讯间距至少200mS
	
	for(i=0;i<(m_ActionCfg.TunBuQingXiShiJian*2);i++)	// 30ML*40S=1.2L水max
	{
		d_set_tt=TiaoJieShuiWen();
		SuReQiSet(1,d_set_tt);  // 速热器每半秒设置一次温度
		TiaoJieLiDu();
		if(WaitTimeout(RT_TICK_PER_SECOND*0.1f, (1<<STOP)|(1<<QingXiTunBu)))
		{
			m_SuReQiState=0;
			m_SuReQiLengShuiTimer=0;
			SuReQiSet(0,0);		// 速热器关闭			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_HOU_BU_SHUI_FA(0);	
			ZENG_YA_BENG_ZHU_SHUI_FA(0);     // 排气阀打开		
			g_sensor.FunctionWorkHouBu=0;			
			rt_kprintf("关\r\n");
			PAI_WU_SUI_WU_BENG(1);//	排污泵开启
			rt_thread_delay(RT_TICK_PER_SECOND*1);
			PAI_WU_SUI_WU_BENG(0);				
			return;
		}
		SuReQiRead(buff); 
		g_sensor.SuReQiWork=buff[0];
		g_sensor.SuReQiError=buff[1];
		g_sensor.LengShuiWenDu= buff[2];
		g_sensor.ReShuiWenDu = buff[3];
		templiuliang=(buff[4]<<8)+buff[5];

		if(WaitTimeout(RT_TICK_PER_SECOND*0.3f, (1<<STOP)|(1<<QingXiTunBu)))
		{
			m_SuReQiState=0;
			m_SuReQiLengShuiTimer=0;
			SuReQiSet(0,0);		// 速热器关闭			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_HOU_BU_SHUI_FA(0);	
				ZENG_YA_BENG_ZHU_SHUI_FA(0);     // 排气阀打开
			g_sensor.FunctionWorkHouBu=0;			
			rt_kprintf("关\r\n");
			PAI_WU_SUI_WU_BENG(1);//	排污泵开启
			rt_thread_delay(RT_TICK_PER_SECOND*1);
			PAI_WU_SUI_WU_BENG(0);			
			return;
		}
	}
	SuReQiSet(0,0); 	// 速热器关闭			
	XIAO_SHUI_BENG(0);
	m_SuReQiState=0;
	m_SuReQiLengShuiTimer=0;
	XIAO_QI_BENG(0);
	RE_SHUI_FANG_SHUI_FA(0);
	RE_SHUI_HOU_BU_SHUI_FA(0);
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // 排气阀打开
	
	CHONG_XI_SHUI_FA_TO_MATONG(1);		// 冲水
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);
	rt_thread_delay(RT_TICK_PER_SECOND*(m_ActionCfg.BianPenShouDongQingJieShiJian+2));		
	PAI_WU_SUI_WU_BENG(1);//	排污泵开启
	rt_thread_delay((TIME_PAI_WU_BENG-3)*RT_TICK_PER_SECOND/2);
	PAI_WU_SUI_WU_BENG(0);
	DoShuiFeng();
	g_sensor.FunctionWorkHouBu=0;
	flag_houbu=0;

}

// 清洗男士前部  用水量 0.4L，用时 15S
static void DoQingXiManQianBu(void)
{
	u8 i;u8 buff[7];
	u16 templiuliang=0;
	u8 d_set_tt;	
	if(!AllowChongShui())
	{
		return;
	}
	g_sensor.FunctionWorkQianBu=1;
	rt_kprintf("清洗男士前部\r\n");
	// 1 先放冷水
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开
	RE_SHUI_FANG_SHUI_FA(1);
	m_XSBDuty=1000;
	XIAO_SHUI_BENG(1);
	SuReQiSet(1,38);
	rt_thread_delay(RT_TICK_PER_SECOND*0.1f);
	SuReQiRead(buff); 
	m_SuReQiState=1;	// 进入线程static void ReShuiTimer(void *p)
	m_SuReQiLengShuiTimer=0;
	rt_thread_delay(50); 		// 速热器彼此通讯间距至少200mS
	
	for(i=0;i<((m_ActionCfg.QianBuYanShi+20)*2);i++)		// 20s*30mL=0.6L	+20s
	{
		d_set_tt=TiaoJieShuiWen();	
		SuReQiSet(1,d_set_tt);  // 速热器每半秒设置一次温度
		if(WaitTimeout(RT_TICK_PER_SECOND*0.1f, (1<<STOP)|(1<<QingXiQianBu)))
		{
			m_SuReQiState=0;
			m_SuReQiLengShuiTimer=0;
			SuReQiSet(0,0);		// 速热器关闭			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_QIAN_BU_SHUI_FA(0);	
		ZENG_YA_BENG_ZHU_SHUI_FA(0);     // 排气阀打开
			g_sensor.FunctionWorkQianBu=0;			
			rt_kprintf("关\r\n");
			return;
		}
		SuReQiRead(buff); 
		g_sensor.SuReQiWork=buff[0];
		g_sensor.SuReQiError=buff[1];
		g_sensor.LengShuiWenDu= buff[2];
		g_sensor.ReShuiWenDu = buff[3];
		templiuliang=(buff[4]<<8)+buff[5];

//		// 反馈控制
//		if(i>30)	//15秒以后
//		{
//			if(g_sensor.ReShuiWenDu<d_set_tt)	// 输出温度达不到设定温度
//				if((d_set_tt-g_sensor.ReShuiWenDu)>2)
//					d_set_tt+=1;
//			else
//				if(((g_sensor.ReShuiWenDu-d_set_tt)>2)&&(!d_set_tt))
//					d_set_tt-=1;
//		}

		if(WaitTimeout(RT_TICK_PER_SECOND*0.3f, (1<<STOP)|(1<<QingXiQianBu)))
		{
			m_SuReQiState=0;
			m_SuReQiLengShuiTimer=0;
			SuReQiSet(0,0);		// 速热器关闭			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_QIAN_BU_SHUI_FA(0);
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // 排气阀打开
			g_sensor.FunctionWorkQianBu=0;			
			rt_kprintf("关\r\n");
			return;
		}
	}
	SuReQiSet(0,0); 	// 速热器关闭			
	XIAO_SHUI_BENG(0);
	m_SuReQiState=0;
	m_SuReQiLengShuiTimer=0;
	XIAO_QI_BENG(0);
	RE_SHUI_FANG_SHUI_FA(0);
	RE_SHUI_QIAN_BU_SHUI_FA(0);	
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // 排气阀打开
	CHONG_XI_SHUI_FA_TO_MATONG(1);		// 冲水
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);
	rt_thread_delay(RT_TICK_PER_SECOND*(m_ActionCfg.BianPenShouDongQingJieShiJian+2));	
	PAI_WU_SUI_WU_BENG(1);//	排污泵开启
	rt_thread_delay((TIME_PAI_WU_BENG-3)*RT_TICK_PER_SECOND/2);
	PAI_WU_SUI_WU_BENG(0);
	DoShuiFeng();						// 防止臭味	
	g_sensor.FunctionWorkQianBu=0;
	flag_qianbu=0;
}


// 智能模式下和瘫痪模式下，自动完成洁便过程。

static char zidongpaiwu_stack[512];
static struct rt_thread zidongpaiwu_thread;

// 自动排污进程，低优先级进程
static void ZiDongPaiWuThreadEntry(void* parameter)
{
	rt_kprintf("进入自动排污进程\r\n");
	while(1)
	{
		rt_thread_delay(RT_TICK_PER_SECOND/10);
		if(!(g_sensor.DaBian||g_sensor.ManXiaoBian||g_sensor.WomenXiaoBian))
		{
			continue;
		}
		rt_thread_delay(RT_TICK_PER_SECOND/10);
		if(!(g_sensor.DaBian||g_sensor.ManXiaoBian||g_sensor.WomenXiaoBian))
		{
			continue;
		}
		rt_thread_delay(RT_TICK_PER_SECOND*5);
		if(AllowChongShui())
		{
//			DoVacuumPump(10);		// 增加水泵排气	
			if(g_sensor.DaBian)
			{
				rt_kprintf("检测到大便，开始自动排污动作\r\n");
				g_sensor.FunctionWorkHouBu=1;
				CHONG_XI_SHUI_FA_TO_MATONG(0);			// 打开到马桶大水阀
				ZHI_CHONG_SHUI_FA(1);					// 打开马桶冲洗-直冲阀
				QING_SHUI_CHONG_XI_ZENG_YA_BENG(1); 	// 打开增压水泵
				rt_thread_delay(RT_TICK_PER_SECOND*4);	// 7 秒 不要同时开，启动功率过大
				CHONG_XI_SHUI_FA_TO_MATONG(1);		// 打开侧壁冲洗
				ZHI_CHONG_SHUI_FA(0);					// 关闭马桶冲洗-直冲阀
				rt_thread_delay(RT_TICK_PER_SECOND*4);	// 7 秒 不要同时开，启动功率过大
				PAI_WU_SUI_WU_BENG(1);				// 打开排污泵				
				rt_thread_delay(RT_TICK_PER_SECOND*(m_ActionCfg.BianPenShouDongQingJieShiJian-3));				
			}
			else 
			{
				rt_kprintf("检测到小便，开始自动排污动作\r\n");
				if(g_sensor.ManXiaoBian)
					g_sensor.FunctionWorkQianBu=1;		// 男性
				if(g_sensor.WomenXiaoBian)
					g_sensor.FunctionWorkQianBu=1;		// 男性					
				//	g_sensor.FunctionWorkHouBu=1;		// 女性
				CHONG_XI_SHUI_FA_TO_MATONG(1);			// 打开到马桶大水阀
				QING_SHUI_CHONG_XI_ZENG_YA_BENG(1); 	// 打开增压水泵
				rt_thread_delay(RT_TICK_PER_SECOND*3);	//不要同时开，启动功率过大
				PAI_WU_SUI_WU_BENG(1);				// 打开排污泵				
				rt_thread_delay(RT_TICK_PER_SECOND*1);
			}

			QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 	// 先关水泵
			CHONG_XI_SHUI_FA_TO_MATONG(0);			
			rt_thread_delay(RT_TICK_PER_SECOND/2);	// 1/2 秒 不要同时开，启动功率过大
			PAI_WU_SUI_WU_BENG(0);				// 再关排污泵

			g_sensor.DaBian = 0;
			g_sensor.ManXiaoBian = 0;
			g_sensor.WomenXiaoBian = 0;
			rt_kprintf("开始自动排污动作结束，重新等待大小便信号\r\n");
			rt_thread_delay(RT_TICK_PER_SECOND*10);
		}
	}
}

// 自动排污线程启用标志
static u8 ZiDongPaiWuThreadRun = 0;

static void StartZiDongPaiWu(void)
{
	u32 i;
	if(!Set_B_Config.ZhiNengModel)   // 智能模式未打开
		return;
	for(i=0;i<90;i++)
	{
		if(!g_bedSensor.BianMenKaiDaoDing)	// 等待便门开到顶
		{
			if(WaitTimeout(RT_TICK_PER_SECOND*0.5f, (1<<STOP)|(1<<ZuoBianKaiShi)|((1<<ZuoBianJieShu))))
			{
				return;
			}
		}
	}
	if(!g_bedSensor.BianMenKaiDaoDing)	// 时间到，便门未到位
	{	return;}
	if(!AllowChongShui())
	{
		return;
	}

	rt_enter_critical();
	if(!ZiDongPaiWuThreadRun)
	{
	    rt_thread_init(&zidongpaiwu_thread,
	                   "paiwu",
	                   ZiDongPaiWuThreadEntry, RT_NULL,
	                   &zidongpaiwu_stack[0], sizeof(zidongpaiwu_stack),
	                   25, 10);
	    rt_thread_startup(&zidongpaiwu_thread);
	}
	ZiDongPaiWuThreadRun = 1;
	rt_exit_critical();
}

static void StopZiDongPaiWu(void)
{
	rt_kprintf("停止自动排污\r\n");
	rt_enter_critical();
	if(ZiDongPaiWuThreadRun)
	{
		rt_thread_detach(&zidongpaiwu_thread);
		DoStop();
	}
	ZiDongPaiWuThreadRun = 0;
	rt_exit_critical();
}

// 座便结束后动作
static void DoZuoBianJieShu(void)
{
	u16 i;u8 j,buff[7];
	u8 d_set_tt=38;	
	u16 yanshi=0;

	if(!Set_B_Config.ZhiNengModel)   // 非智能模式下退出
		return;
	if(!AllowChongShui())
	{
		return;
	}
	rt_kprintf("开始座便结束后动作\r\n");
	g_sensor.FunctionWorkAuto=1;

	// 1. 清洗前后部
		ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开
	RE_SHUI_FANG_SHUI_FA(1);
	m_XSBDuty=1000;
	XIAO_SHUI_BENG(1);	 
	SuReQiSet(1,d_set_tt);
	rt_thread_delay(RT_TICK_PER_SECOND*0.1f);
	SuReQiRead(buff); 	
	m_SuReQiState=1;	// 进入线程static void ReShuiTimer(void *p)
	m_SuReQiLengShuiTimer=0;
	rt_thread_delay(50); 		// 速热器彼此通讯间距至少200mS

	if(g_sensor.FunctionWorkQianBu)
		yanshi=m_ActionCfg.QianBuYanShi + 20;   // 增加20S

	if(g_sensor.FunctionWorkHouBu)
		yanshi=m_ActionCfg.TunBuQingXiShiJian;

	for(i=0;i<(yanshi*2);i++)	// 30ML*40S=1.2L水max
	{
		if(g_sensor.FunctionWorkHouBu&&g_sensor.FunctionWorkQianBu)  // 前后部一起冲洗
		{	
			if(i>=m_ActionCfg.TunBuQingXiShiJian*2)  // 前部清洗时间到
			{	
				RE_SHUI_QIAN_BU_SHUI_FA(0);			 // 前部水阀关闭 其他不动
				g_sensor.FunctionWorkQianBu= 0;
			}
		}
		d_set_tt=TiaoJieShuiWen();	
		SuReQiSet(1,d_set_tt);  // 速热器每半秒设置一次温度
		if(WaitTimeout(RT_TICK_PER_SECOND*0.1f, (1<<STOP)|(1<<ZuoBianJieShu)))
		{
			m_SuReQiState=0;
			m_SuReQiLengShuiTimer=0;
			SuReQiSet(0,0);		// 速热器关闭			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_QIAN_BU_SHUI_FA(0);
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // 排气阀打开
			g_sensor.FunctionWorkAuto=0;
			g_sensor.FunctionWorkHouBu=0;
			g_sensor.FunctionWorkQianBu=0;
			rt_kprintf("关\r\n");
			return;
		}
		SuReQiRead(buff); 
		g_sensor.SuReQiWork=buff[0];
		g_sensor.SuReQiError=buff[1];
		g_sensor.LengShuiWenDu= buff[2];
		g_sensor.ReShuiWenDu = buff[3];

		// 反馈控制
//		if(i>30)	//15秒以后
//		{
//			if(g_sensor.ReShuiWenDu<d_set_tt)	// 输出温度达不到设定温度
//				if((d_set_tt-g_sensor.ReShuiWenDu)>2)
//					d_set_tt+=1;
//			else
//				if(((g_sensor.ReShuiWenDu-d_set_tt)>2)&&(!d_set_tt))
//					d_set_tt-=1;
//		}
//		if(d_set_tt>42)
//			d_set_tt = 42;	// 设定最高温		
		if(WaitTimeout(RT_TICK_PER_SECOND*0.3f, (1<<STOP)|(1<<ZuoBianJieShu)))
		{
			m_SuReQiState=0;
			m_SuReQiLengShuiTimer=0;
			SuReQiSet(0,0);		// 速热器关闭			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_HOU_BU_SHUI_FA(0);	
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // 排气阀打开			
			g_sensor.FunctionWorkAuto=0;
			g_sensor.FunctionWorkHouBu=0;
			g_sensor.FunctionWorkQianBu=0;			
			return;
		}
	}
	SuReQiSet(0,0); 	// 速热器关闭			
	XIAO_SHUI_BENG(0);
	m_SuReQiState=0;
	m_SuReQiLengShuiTimer=0;
	g_sensor.FunctionWorkHouBu=0;
	g_sensor.FunctionWorkQianBu=0;
	XIAO_QI_BENG(0);
	RE_SHUI_FANG_SHUI_FA(0);
	RE_SHUI_HOU_BU_SHUI_FA(0);	
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // 排气阀打开

	// 2. 冲洗便盆
	CHONG_XI_SHUI_FA_TO_MATONG(1);			// 打开到马桶大水阀
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);		// 打开增压水泵
	if(WaitTimeout((m_ActionCfg.BianPenZiDongPaiWuYanShi+2+10)*RT_TICK_PER_SECOND, (1<<STOP)|(1<<ZuoBianJieShu)))
	{
		QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 
		PAI_WU_SUI_WU_BENG(0);				
		CHONG_XI_SHUI_FA_TO_MATONG(0);		
		g_sensor.FunctionWorkAuto=0;
		return;
	}
	PAI_WU_SUI_WU_BENG(1);//	排污泵开启
	rt_thread_delay(TIME_PAI_WU_BENG*RT_TICK_PER_SECOND/2);	// 2 秒 不要同时开，启动功率过大
	
//	QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 	// 先关水泵
//	CHONG_XI_SHUI_FA_TO_MATONG(0);			
//	rt_thread_delay(RT_TICK_PER_SECOND/2);	// 1/2 秒 不要同时开，启动功率过大
	PAI_WU_SUI_WU_BENG(0);				// 再关排污泵
	DoShuiFeng();

	// 3 干燥
	SHI_DIAN_ZONG_KAI_GUAN(1);		// 开启市电输入
	HONG_GAN_PI_GU_FENG_SHAN(1);	// 开启暖风风扇
	for(i=0;i<(m_ActionCfg.HongGanShiJian);i++)   //  1S一个周期
	{
		d_set_tt = 80;

		HONG_GAN_PI_GU_DIAN_RE_SI(1);	// 开启暖风电热丝，电热丝控制采用模拟PWM2秒一周期，100mS一个单位
		for(j=0;j<100;j++)	
		{
		 if(j>=d_set_tt)	//如果到达设定值 关闭加热
		 {	HONG_GAN_PI_GU_DIAN_RE_SI(0);}
		 if(WaitTimeout(RT_TICK_PER_SECOND*0.01f, (1<<STOP)|(1<<ZuoBianJieShu)))
		 {
			 HONG_GAN_PI_GU_DIAN_RE_SI(0);
			 SHI_DIAN_ZONG_KAI_GUAN(0);	
			 HONG_GAN_PI_GU_FENG_SHAN(0);		
			 g_sensor.FunctionWorkAuto=0;			 
			 return;
		 }
		}
	}
	HONG_GAN_PI_GU_DIAN_RE_SI(0);
	SHI_DIAN_ZONG_KAI_GUAN(0); 
	HONG_GAN_PI_GU_FENG_SHAN(0);  
	g_sensor.FunctionWorkAuto=0;
	flag_zuobianjieshu=0;
}
// 由于加热功率输出需要24V，5A左右。所以，在大功率设备工作时不能加热；
// 面板有任何操作控制，暂停加热模式,操作结束后，恢复加热模式。
// 1.停电，蓄电池供电时；2.电动推杆运行时，3.座便器工作时。
// 由于电动推杆的运行，没发读取反馈，而且功率基本<5A,暂时不考虑暂停加热工作。
static int AllowJiaRePower(void)
{
	if(PowerWorkFlag)
	{	rt_kprintf("座便器工作时\r\n");
		return 0;}
	if(g_bedSensor.TingDian)
	{
		rt_kprintf("停电，蓄电池工作时\r\n");
		return 0;	
	}
	return 1;
}
static char chuangdianjiare_stack[512];
static struct rt_thread chuangdianjiare_thread;

static void JiaReThreadEntry(void )
{
	u8 c_set_tt,c_read_tt;
	rt_kprintf("进入加热进程\r\n");
	while(1)
	{
		if(AllowJiaRePower())
		{	
//			c_read_tt=g_sensor.ChuanDianWenDu;		// 读取到的温度
			c_set_tt= Set_B_Config.ChuangDianJiaRe;	//设定温度
			if(c_read_tt<c_set_tt)
				JiaReModule(1);
			else
				JiaReModule(0);
		}
		else
		{
			JiaReModule(0);
		}
		rt_thread_delay(RT_TICK_PER_SECOND);
	}

}


// 床垫加热线程启用标志
static u8 JiaReThreadRun = 0;
// 开启自动加热床垫
static void StartJiaReChuangDian(void)
{

	rt_enter_critical();
	if(!JiaReThreadRun)
	{
	    rt_thread_init(&chuangdianjiare_thread,
	                   "chuangdianjiare",
	                   JiaReThreadEntry, RT_NULL,
	                   &chuangdianjiare_stack[0], sizeof(chuangdianjiare_stack),
	                   25, 10);
	    rt_thread_startup(&chuangdianjiare_thread);
	}
	JiaReThreadRun = 1;
	rt_exit_critical();
}

static void StopJiaReChuangDian(void)
{
	rt_kprintf("停止床垫加热\r\n");
	rt_enter_critical();
	if(JiaReThreadRun)
	{
		rt_thread_detach(&chuangdianjiare_thread);
		JiaReModule(0);
	}
	JiaReThreadRun = 0;
	JiaReModule(0);
	rt_exit_critical();
}

// 接收到CAN回调函数
extern rt_err_t CanRxInd(rt_device_t dev, rt_size_t size);
extern void CanAnalyzerInit(void);

static struct rt_timer reshui_timer;
// 热水泵定时器每秒的响应次数
#define	RE_SHUI_BENG_TIMER_PER_SEC	2

// 速热器水泵冲水定时器
// 每0.5 秒调用一次
static void ReShuiTimer(void *p)
{	
	feedWD();	 // 喂狗
	if(!g_bedSensor.BianMenKaiDaoDing)			// 便门未打开
	{
			return ;
	}
	if(m_SuReQiState == 1)   // 开始换水路为中间位
	{	
		if(++m_SuReQiLengShuiTimer/RE_SHUI_BENG_TIMER_PER_SEC > m_LSSC) //大于10S
		{
			m_SuReQiLengShuiTimer = 0;
			m_SuReQiState = 2;
			if(g_sensor.FunctionWorkQianBu)
			{
				m_XSBDuty=QIAN_BU_CHONG_XI_LI_DU; 	
		ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开			
				XIAO_SHUI_BENG(1);
			}
			if(g_sensor.FunctionWorkHouBu)
			{	
				m_XSBDuty=set_XSBDuty;		
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开				
				XIAO_SHUI_BENG(1);
			}
		}	
		if(m_SuReQiLengShuiTimer==10) // 前5 秒高速排水 ,5秒后开启低速排水
		{
			m_XSBDuty=QING_XI_PENG_TOU;		
		ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开		
			XIAO_SHUI_BENG(1);
			RE_SHUI_FANG_SHUI_FA(0);
			if(g_sensor.FunctionWorkHouBu )
			{	RE_SHUI_HOU_BU_SHUI_FA(1);}
			if(g_sensor.FunctionWorkQianBu)
			{	RE_SHUI_QIAN_BU_SHUI_FA(1);}
		}
		if(m_SuReQiLengShuiTimer>12) // 前5 秒高速排水 中间5秒低速排接头管路水
		{
//				m_XSBDuty=QIAN_BU_CHONG_XI_LI_DU;
//				XIAO_SHUI_BENG(1);
			if(g_sensor.FunctionWorkHouBu)
			{
				m_XSBDuty+=100;
				if(m_XSBDuty>=set_XSBDuty)
					m_XSBDuty=set_XSBDuty;
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开				
				XIAO_SHUI_BENG(1);
			}
			if(g_sensor.FunctionWorkQianBu)
			{
				m_XSBDuty+=50;
				if(m_XSBDuty>=QIAN_BU_CHONG_XI_LI_DU)
					m_XSBDuty=QIAN_BU_CHONG_XI_LI_DU;
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开				
				XIAO_SHUI_BENG(1);				
			}
		}
	}
	else if(m_SuReQiState == 2)
	{
	//	if(++m_SuReQiLengShuiTimer/RE_SHUI_BENG_TIMER_PER_SEC > m_TSDK)
	//	{
			if(g_sensor.FunctionWorkHouBu)
			{
				if(flag_set_qiben)
					{
						XIAO_QI_BENG(1);
					}
				else
					{
						XIAO_QI_BENG(0);
					}
				if(m_XSBDuty!=set_XSBDuty)
					{
						ZENG_YA_BENG_ZHU_SHUI_FA(1);     // 排气阀打开
						m_XSBDuty=set_XSBDuty;
						XIAO_SHUI_BENG(1);
					}	
			}
	//		m_SuReQiState = 3;
	//	}
	}
	else 
	{

	}
}

void DoRotatePADUp(void)
{
	u16 i;
	u8 timeS = 7;   //旋转到顶X秒
	rotatePAD(1,1);
	for(i=0;i<10*timeS;i++)
	{			
		if(WaitTimeout(20, (1<<STOP)|(1<<RotatePADUp)|(1<<RotatePADDown)))
		{
			rotatePAD(0,0);
			return;
		}	
	}
	rotatePAD(0,0);
	flag_rotatePad=0;
}

void DoRotatePADDown(void)
{
	u16 i;
	u8 timeS = 7;   //旋转到底X秒
	rotatePAD(1,0);
	for(i=0;i<10*timeS;i++)
	{			
		if(WaitTimeout(20, (1<<STOP)|(1<<RotatePADDown)|(1<<RotatePADUp)))
		{
			rotatePAD(0,0);
			return;
		}	
	}
	rotatePAD(0,0);
	flag_rotatePad =0;
}

// 动作进程入口
void ActionThreadEntry(void* parameter)
{
	rt_uint32_t cmd,evt;
	
	ActionInit();
	// 打开 CAN1 设备
	CanAnalyzerInit();	
	rt_timer_init(&reshui_timer, "re shui", ReShuiTimer, RT_NULL, RT_TICK_PER_SECOND*0.5f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
	rt_timer_start(&reshui_timer);

//	SuReQiSet(0,Set_B_Config.ShuiWen);  // 初始化速热器 不工作
	
 /* 创建线程1  用于旋转餐桌初始化为靠背模式*/	
	rt_thread_init(&thread_rotate,
							 "rotate",
							 thread1_entry, RT_NULL,
							 &thread_rotate_stack[0], sizeof(thread_rotate_stack),
							 12, 10);
	rt_thread_startup(&thread_rotate);
			
	rt_kprintf("init system end\r\n");
	while(1)
	{
		// 等待接收邮件命令
		rt_mb_recv(&m_mb, &cmd, RT_WAITING_FOREVER);	// 等待接收命令
		DoStop();
		rt_kprintf("接收到动作命令 %d\r\n",(int)cmd);
		rt_event_recv(&m_evt,0xffffffff , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, RT_WAITING_NO, &evt);	// 清事件
		switch(cmd)
		{
			case RotatePADUp:
				DoRotatePADUp();
				break;
				case RotatePADDown:
				DoRotatePADDown();
				break;
			case ChongXiBianPeng:
				PowerWorkFlag=1;
				StopZiDongPaiWu();
				DoChongXiBianPeng();
				PowerWorkFlag=0;
				break;
			case QingXiTunBu:
				PowerWorkFlag=1;
				StopZiDongPaiWu();
				DoQingXiTunBu();	
				PowerWorkFlag=0;
				break;
			case QingXiQianBu:
				PowerWorkFlag=1;	
				StopZiDongPaiWu();
				DoQingXiManQianBu();	
				PowerWorkFlag=0;
				break;				
			case GanZao:
				StopZiDongPaiWu();
				DoGanZao();
				break;	
			case ZuoBianKaiShi:
				PowerWorkFlag=1;
				StartZiDongPaiWu();
				break;
			case ZuoBianJieShu:				
				StopZiDongPaiWu();
				DoZuoBianJieShu(); 
				PowerWorkFlag=0;
				break;
			case ChuangDianJiaReWork:
				StartJiaReChuangDian();
				break;
			case ChuangDianJiaReEnd:
				StopJiaReChuangDian();
				break;				
			case STOP:
				DoStop();
				rt_kprintf("强制停止所有动作\r\n");
				break;				
			default:
				DoStop();
				rt_kprintf("没有找到相应的动作指令\r\n");
				break;
		}
		DoStop();
		rt_mb_recv(&m_mb, &cmd, RT_WAITING_NO);	// 清命令
		rt_event_recv(&m_evt,0xffffffff , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, RT_WAITING_NO, &evt);	// 清事件
		rt_kprintf("动作结束\r\n");
	}
}

