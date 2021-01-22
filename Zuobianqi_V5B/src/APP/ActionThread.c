/*******************************
	��־:
		2021.1.12.  
				1. ˮ·��ϴʱ����̣�ȥ��ֱ�壬��Ϊֱ��û��Ч������Ͱ�ڳ�ϴʱ��ӳ���18S
				2. ǰ���ͺ󲿵�����ȥ������ǰ���������֣�ǰ���ŵ�����λ�ã��󲿷ź��棻ǰ����ʱ��һ��
				3. ����Һ������ת����
				4. ȥ�����������ܣ���ΪҺ������ת���㶯��
				5. ����Сˮ���ſ���ʱ��ˮ�����
				6. ��ת���������ʱ����
				7. ��ת����������źŵ�pa5��ΪPA24 ���������·� 
				8. pa5 �� pa2 ���ں����⣬�����·��ˮ·��·�����������������д��Ӳ��ʵ�֡�
		1.19.		ȥ��ˮ�ú�˵�3·ˮ������Ͱ��ˮ����ֱ��ˮ������ͰȦ��ϴˮ����ˮ��ֱ������Ͱ����ϴ��
					ȥ�����Ź�����ֹϵͳ��ʼ����������˺�
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
// ��ˮ��ˮʱ������λ��
u8 m_LSSC = LENG_SHUI_FANG_SHUI;
u8 m_TSDK = 2;
//s32 m_SlowStep = SLOW_STEP;	// ���粽��
#ifdef RT_USING_FINSH
FINSH_VAR_EXPORT(m_LSSC, finsh_type_uchar, "��ˮ��ˮʱ������λ��");
FINSH_VAR_EXPORT(m_TSDK, finsh_type_uchar, "��ˮ��ˮ��ˮͬʱ��ʱ������λ��");
//FINSH_VAR_EXPORT(m_SlowStep, finsh_type_int, "��·���������粽��");
#endif

// ��ˮСˮ��pwmռ�ձ�
u16 m_XSBDuty	= 500;
u16 set_XSBDuty = 1000;
u8 flag_set_qiben =1;
#ifdef RT_USING_FINSH
FINSH_VAR_EXPORT(m_XSBDuty, finsh_type_ushort, "��ˮСˮ��ռ�ձ�");
#endif


// ������ʹ�ܣ�0=�رգ�1=��ʼ״̬��2=����״̬
static u8 m_SuReQiState = 0;
// ����������ˮʱ�� 
static u8 m_SuReQiLengShuiTimer = 0;

u8 rotateDiningCount = 0;   // TIME_ROTATE_DINING=8   80 < 255
u8 rotateDiningDir;  //����Ĺ�������

// ������������
volatile s_SetAConfig  Set_A_Config={0};
volatile s_SetBConfig  Set_B_Config={0};
volatile t_ActionConfig m_ActionCfg={0};
volatile u8 Set_MianBan_Config_ShuiWen=0;
volatile u8 Set_MianBan_Config_NuanFeng=0;
volatile u8 Set_MianBan_Config_QingXi=0;
volatile u8 PowerWorkFlag=0;		// ���ȵ����

#define TIME_PAI_WU_BENG 6		//���۱�һ������ʱ�� 0.5S/PCS    ���۱�1S == ˮ�� 3S ˮ��
#define TIME_ADD_FLUSH  7			//����ˮ�ó�ϴʱ��    ʵ��ˮ������ 0.138 L/S

#define QING_XI_PENG_TOU   700// 250			 // ��ϴ��ͷ����
#define QIAN_BU_CHONG_XI_LI_DU  900//700      // ǰ����ϴ����
// �е��ܿ��ƿ���
#define SHI_DIAN_ZONG_KAI_GUAN(on) if(on) {IO1SET = BIT28;} else {IO1CLR = BIT28;}
// ���ƨ�ɵ���˿
#define HONG_GAN_PI_GU_DIAN_RE_SI(on)	if(on) {IO0SET = BIT20;} else {IO0CLR = BIT20;}
// ���ƨ�ɷ���
#define HONG_GAN_PI_GU_FENG_SHAN(on)	if(on) {IO0SET = BIT12;} else {IO0CLR = BIT12;}

// ��ˮС����
//#define RE_SHUI_XIAO_QI_BENG(on)	if(on) {PwmChange(5, m_XSBDuty);} else {PwmChange(5,0);}
#define XIAO_QI_BENG(on)	if(on) {IO0CLR = BIT10;} else {IO0CLR = BIT10;}

// ��ˮǰ����ϴˮ��
#define RE_SHUI_QIAN_BU_SHUI_FA(on)	if(on) {IO0SET = BIT16;} else {IO0CLR = BIT16;}
// ��ˮ����ϴˮ��
#define RE_SHUI_HOU_BU_SHUI_FA(on)	if(on) {IO0SET = BIT17;} else {IO0CLR = BIT17;}
// ��ϴ��Ͱ-��
//#define CHONG_XI_SHUI_FA_TO_MATONG(on)	if(on) {IO1SET = BIT22;} else {IO1CLR = BIT22;}
#define CHONG_XI_SHUI_FA_TO_MATONG(on)	if(on) {IO1CLR = BIT22;} else {IO1CLR = BIT22;}
// ��ϴ��Ͱ-ֱ��ˮ��
//#define ZHI_CHONG_SHUI_FA(on)	if(on) {IO1SET = BIT21;} else {IO1CLR = BIT21;}
#define ZHI_CHONG_SHUI_FA(on)	if(on) {IO1CLR = BIT21;} else {IO1CLR = BIT21;}
// ����Ƭ ���ֱ�
#define JIA_RE_PIAN_YOU(on) if(on) {IO1SET = BIT27;} else {IO1CLR = BIT27;}
// ����Ƭ ���ϱ�
#define JIA_RE_PIAN_ZHONG_UP(on) if(on) {IO1SET = BIT25;} else {IO1CLR = BIT25;}
// ����Ƭ ���±�
#define JIA_RE_PIAN_ZHONG_DOWN(on) if(on) {IO1SET = BIT24;} else {IO1CLR = BIT24;}
// ����Ƭ ���ֱ�
#define JIA_RE_PIAN_ZUO(on) if(on) {IO1SET = BIT23;} else {IO1CLR = BIT23;}
// Сҹ�ƿ���
#define NIGHT_LAMP_WORK(on) if(on) {IO1SET = BIT20;} else {IO1CLR = BIT20;}

static struct rt_thread thread_rotate;
static char thread_rotate_stack[512];

static void DoVacuumPump(u8 timeS);
// ƽ����ʾ����ת  p0.19 ; P0.05 ;// pa5��ΪPA24
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

// ��ת�������  p1.29 ; P0.18 ;
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
FINSH_FUNCTION_EXPORT(rotateTable, "��ת����,work,dir");
#endif

void ActionStartCmd(ActionCmd cmd);


// ����ģ��
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
FINSH_FUNCTION_EXPORT(JiaReModule, "�������[],work");
#endif

// Сˮ·
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
FINSH_FUNCTION_EXPORT(Xiaoshuibeng, "Сˮ��[][][],0��,PWM1000");
#endif

//��ѹ�ô�
static void Chongximatong(int beng)
{
	if(beng)
	{
//	CHONG_XI_SHUI_FA_TO_MATONG(1);	// ��Ͱ��ϴ���
	ZHI_CHONG_SHUI_FA(1);				// ��Ͱ��ϴֱ��
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);}		
	else
	{
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(0);
	ZHI_CHONG_SHUI_FA(0);
	CHONG_XI_SHUI_FA_TO_MATONG(0);}
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(Chongximatong, "��ϴ��Ͱ0��");
#endif

//��ѹ�ô�
static void Chongxizuoquan(int beng)
{
	if(beng)
	{CHONG_XI_SHUI_FA_ZUOBIANQUAN(1);QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);}		
	else
	{QING_SHUI_CHONG_XI_ZENG_YA_BENG(0);CHONG_XI_SHUI_FA_ZUOBIANQUAN(0);}
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(Chongxizuoquan, "��ϴ����Ȧ0��");
#endif

//���۱ô�
static void Paiwubeng(int beng)
{
	if(beng)
	{PAI_WU_SUI_WU_BENG(1);}		
	else
	{PAI_WU_SUI_WU_BENG(0);}
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(Paiwubeng, "���۱�0��");
#endif


// ���л�
static void Fa(int fa)
{
	RE_SHUI_FANG_SHUI_FA(fa&BIT0);	 		// ��ˮˮ·����ˮ��
	RE_SHUI_QIAN_BU_SHUI_FA(fa&BIT1);  		// ��ˮˮ·ǰ����
	RE_SHUI_HOU_BU_SHUI_FA(fa&BIT2);  		// ��ˮˮ·�󲿷�
	CHONG_XI_SHUI_FA_TO_MATONG(fa&BIT3); 	// ��ϴ��Ͱˮ· --���
	ZHI_CHONG_SHUI_FA(fa&BIT3);				// ��ϴ��Ͱ--ֱ��
	CHONG_XI_SHUI_FA_ZUOBIANQUAN(fa&BIT4);  // ��ϴ������
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(Fa, "���л���[Bit0=��],[BIT1=ǰ],[Bit2=��],[BIT3=��],[BIT4=��]");
#endif

//ů��
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
FINSH_FUNCTION_EXPORT(Nuanfeng, "ů����[][]0��");
#endif

// ֹͣ���ж���
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

// ���Ź���ʼ��
// 2880000 ,1��
void WDInit(void)
{
//	WDTC = 0x05265C00;	// ���ÿ��Ź���ʱ���Ĺ̶�װ��ֵ : 30 ��
//	WDMOD= 0X03;   // ģʽ�趨
//	feedWD();
}

void feedWD(void) // ι��
{
//	WDFEED =0XAA;
//	WDFEED =0X55;
}

u32 WDtimers(void)
{
	return WDTV; //���Ź���ʱ���ĵ�ǰֵ
}


// Ӳ����ʼ��
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


// ������ر���
static struct rt_mailbox m_mb;
static rt_uint32_t mbPool[1];
// �¼�
static struct rt_event m_evt;

static void ActionInit(void)
{
	u8 buffer[7];
	// Ӳ����ʼ��
	ActionHwInit();
	// ������ʼ��
	Set_A_Config.ShuiWen = 2;								// ���A������������ϴˮ���¶� 0-3 ��λ����
	Set_A_Config.NuanFen = 3;								// ���A����������ů���¶� 0-3 ��λ����
	Set_A_Config.ShuiLiu = 3;								// ���A������������ϴǿ�� 1-3 ��λ����

	Set_B_Config.ShuiWen = 2;								// ���B����������ϴˮ���¶� ��λ1�棬0-41��
	Set_B_Config.NuanFen = 3;								// ���B��������ů���¶� ��λ1�棬40-65��
	Set_B_Config.ShuiLiu = 3;								// ���B����������ϴˮ��ǿ��1-3 ��λ����
	Set_B_Config.ZhiNengModel=0;							// ����ģʽ����
	Set_B_Config.ChuangDianJiaRe=38;						// �趨Ĭ���¶���38��

	Set_MianBan_Config_ShuiWen = 2;
	Set_MianBan_Config_NuanFeng = 3;
	Set_MianBan_Config_QingXi= 3;

	Set_B_Config.ChuangDianPower =0;
	Set_B_Config.NightLampSwitch =0;
	Set_B_Config.ToiletLampSwitch =0;
	
	m_ActionCfg.BianPenZiDongPaiWuYanShi = 2;				// �����Զ�����ʱ�䣬��λ�룬0-60000��Ч
	m_ActionCfg.BianPenZiDongPauWuJianCeYanShi = 30;		// �����Զ����ۼ����ʱ����λ�룬0-60000��Ч
	m_ActionCfg.BianPenShouDongQingJieShiJian = 7;			// �����ֶ����ʱ�䣬��λ�룬0-60000��Ч
	m_ActionCfg.HongGanShiJian = 30;						// ���ʱ�䣬��λ�룬��Χ0-60000��Ч
	m_ActionCfg.TunBuQingXiShiJian = 40;					// �β���ϴʱ�䣬��λ�룬��Χ0-60000��Ч
	m_ActionCfg.QianBuYanShi = 18;							// ǰ����ϴʱ�䣬��λ�룬��Χ0-60000��Ч
	m_ActionCfg.MaTongQuanYanShi= 8;						// ��ͰȦ��ϴʱ�䣬��λ�룬��Χ0-60000��Ч

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
	// ��ʼ������
	rt_mb_init(&m_mb, "act mb", mbPool, sizeof(mbPool)/sizeof(rt_uint32_t), RT_IPC_FLAG_FIFO);
	// ��ʼ���¼�
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
//			pData+=1;	// ֲ����ģʽ��ʱ��֧��
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


// ��ʼ��������
void ActionStartCmd(ActionCmd cmd)
{
	static ActionCmd cmdbuf = ActionCmdNone;
	if(cmd != cmdbuf)  // ������ǰ������ද�� 
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
	//	DbgPrintf("������Ϣ����������\r\n");
		rt_mb_send(&m_mb, cmd);
		break;
	}
}
FINSH_FUNCTION_EXPORT(ActionStartCmd,"��ʼ����[cmd:1=��ϴ���裬2=��ϴ�β���3=����]");

// ֹͣ��������
void ActionStopCmd(ActionCmd cmd)
{
	rt_event_send(&m_evt, 1<<cmd);
}
FINSH_FUNCTION_EXPORT(ActionStopCmd, "ֹͣ���� [cmd:0=ֹͣ���ж�����1=��ϴ���裬2=��ϴ�β���3=����]");

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

// �жϱ����Ƿ�򿪣�0��ʾδ�򿨣�1��ʾ�Ѵ�
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

// �жϱ����Ƿ��ѹرգ�0��ʾδ�򿪣�1��ʾ�Ѵ�
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

// �ж�ˮ���Լ��������Ƿ������ˮ����
static int AllowChongShui(void)
{
	if((g_sensor.WuWuXiangZhongLiang>=WU_WU_XIANG_MAN)||(g_sensor.ShuiXiangLow==1)||(g_sensor.WuWuXiangGuan==0)||(g_sensor.MaTongFull==1))
	{
		rt_kprintf("��ˮ����������������������䲻��λ�û���Ͱ��\r\n");
		return 0;
	}
 	else if(!IsOpen())
	{
		rt_kprintf("����δ��\r\n");
		return 0;
	}
	else
	{
		return 1;
	}
}
// �ж�ˮ���Լ��������Ƿ������ˮ����
static int AllowChongMaTong(void)
{
	if((g_sensor.WuWuXiangZhongLiang>=WU_WU_XIANG_MAN)||(g_sensor.ShuiXiangLow==1)||(g_sensor.WuWuXiangGuan==0))
	{
		rt_kprintf("��ˮ����������������������䲻��λ�û���Ͱ��\r\n");
		return 0;
	}
 	else if(!IsOpen())
	{
		rt_kprintf("����δ��\r\n");
		return 0;
	}
	else
	{
		return 1;
	}
}

// ˮ��
static void DoShuiFeng(void)
{
	CHONG_XI_SHUI_FA_TO_MATONG(1);
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);
	rt_thread_delay(RT_TICK_PER_SECOND);	// 1 ��
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(0);
	CHONG_XI_SHUI_FA_TO_MATONG(0);
	ZHI_CHONG_SHUI_FA(0);
}

// ����ѹ�ó���գ��ų��ڲ�����
static void DoVacuumPump(u8 timeS)
{
	u16 i,cnt;
	u8 buff[7];
	u16 templiuliang=0;
	cnt=0;
	if((g_sensor.ShuiXiangLow==1)||(g_sensor.WuWuXiangGuan==0))
	{
		rt_kprintf("��ˮ�����������·δ����\r\n");
		return ;
	}	
	m_XSBDuty = 1000;
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������
	RE_SHUI_FANG_SHUI_FA(1);						// ��ˮ��ˮˮ����
	XIAO_SHUI_BENG(1);								// Сˮ�ô�
	
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

// ��ʼ����ת�����俿�� ������ʱ��������������������ִ��
// ��ѹСˮ�ô�ˮ�������ſ���
static void thread1_entry(void* parameter)
{
		rt_kprintf("rotate Dining\r\n");
	//�ȴ�������ư�3S  ���������ģʽ������ƽ����������ס��������Ҫ�ȱ�����һ�������
  rt_thread_delay(3*RT_TICK_PER_SECOND);
  
	rotateDiningCount = 10*TIME_ROTATE_DINING ; // ��ʼ�����ֵ
	rt_kprintf("Start �俿�� \r\n");
	rotateDining(1,0);		// �俿��

	rt_thread_delay(TIME_ROTATE_DINING*RT_TICK_PER_SECOND); // ����ʱ�� = TIME_ROTATE_DINING ��
	
	rt_kprintf("End ��ɿ���\r\n");
	rotateDiningCount=0;
	rotateDining(0,0);
	
	// ��ѹСˮ�ô�ˮ�������ſ���
	DoVacuumPump(25);	
}

// ��ϴ����  ˮ��=�趨ʱ��*0.33L/S ��9S=3Lˮ����ˮˮ�ñȽ�ˮ��1��  ��ˮ��1.2L,��ʱ10S
static void DoChongXiBianPeng(void)
{
	if(!AllowChongMaTong())
	{
		return;
	}
	g_sensor.FunctionWorkMaTong =1;
	rt_kprintf("��ʼ��ϴ����\r\n");
	if(g_sensor.MaTongFull == 1)	// ֱ�ӿ������۱�
	{
		PAI_WU_SUI_WU_BENG(1);				// �����۱�
		if(WaitTimeout(TIME_PAI_WU_BENG*RT_TICK_PER_SECOND/2, (1<<STOP)|(1<<ChongXiBianPeng)))
		{
			PAI_WU_SUI_WU_BENG(0);				
			g_sensor.FunctionWorkMaTong =0;
			return;
		}	
		PAI_WU_SUI_WU_BENG(0);				// �ٹ����۱�
		g_sensor.FunctionWorkMaTong =0;		
		return;		
	}

//	CHONG_XI_SHUI_FA_TO_MATONG(0);			// �򿪵���Ͱ��ˮ��
//	ZHI_CHONG_SHUI_FA(1);					// ����Ͱ��ϴ-ֱ�巧
	CHONG_XI_SHUI_FA_TO_MATONG(1);		// �򿪲�ڳ�ϴ
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);		// ����ѹˮ��
//	if(WaitTimeout(4*RT_TICK_PER_SECOND, (1<<STOP)|(1<<ChongXiBianPeng)))
//	{
//		QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 
//		PAI_WU_SUI_WU_BENG(0);				
//		CHONG_XI_SHUI_FA_TO_MATONG(0);			
//		ZHI_CHONG_SHUI_FA(0);					// �ر���Ͱ��ϴ-ֱ�巧
//		g_sensor.FunctionWorkMaTong =0;
//		return;
//	}
//	CHONG_XI_SHUI_FA_TO_MATONG(1);		// �򿪲�ڳ�ϴ
//	ZHI_CHONG_SHUI_FA(0);					// �ر���Ͱ��ϴ-ֱ�巧
	if(WaitTimeout((m_ActionCfg.BianPenShouDongQingJieShiJian+TIME_ADD_FLUSH)*RT_TICK_PER_SECOND, (1<<STOP)|(1<<ChongXiBianPeng)))
	{
		QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 
		PAI_WU_SUI_WU_BENG(0);				
		CHONG_XI_SHUI_FA_TO_MATONG(0);			
		ZHI_CHONG_SHUI_FA(0);					// �ر���Ͱ��ϴ-ֱ�巧
		g_sensor.FunctionWorkMaTong =0;
		return;
	}
	PAI_WU_SUI_WU_BENG(1);				// �����۱�
	if(WaitTimeout(TIME_PAI_WU_BENG*RT_TICK_PER_SECOND/2, (1<<STOP)|(1<<ChongXiBianPeng)))
	{
		QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 
		PAI_WU_SUI_WU_BENG(0);				
		CHONG_XI_SHUI_FA_TO_MATONG(0);			
		ZHI_CHONG_SHUI_FA(0);					// �ر���Ͱ��ϴ-ֱ�巧
		g_sensor.FunctionWorkMaTong =0;
		return;
	}
//	QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 	// �ȹ�ˮ��
//	CHONG_XI_SHUI_FA_TO_MATONG(0);			
//	rt_thread_delay(RT_TICK_PER_SECOND/2);	// 1/2 �� ��Ҫͬʱ�����������ʹ���
	PAI_WU_SUI_WU_BENG(0);				// �ٹ����۱�
	DoShuiFeng();						// ��ֹ��ζ
	g_sensor.FunctionWorkMaTong =0;

}

// ������ϴǿ��
//static void TiaoJieLiDu(void)
//{
//   if(Set_A_Config.ShuiLiu==1)	   //�ر����� ˮ�� 70%
//		{set_XSBDuty=700;flag_set_qiben=0;}
//   if(Set_A_Config.ShuiLiu==2)	   // ������   ˮ�� 100% 
//		{set_XSBDuty=1000;flag_set_qiben=0;}   
//   if(Set_A_Config.ShuiLiu==3)	   // ������ ˮ�� 100% 
//   		{set_XSBDuty=1000;flag_set_qiben=1;}
//}
static void TiaoJieLiDu(void)
{
   if(Set_MianBan_Config_QingXi==1)	   //�ر����� ˮ�� 70%
		{set_XSBDuty=700;flag_set_qiben=0;}
   if(Set_MianBan_Config_QingXi==2)	   // ������   ˮ�� 100% 
		{set_XSBDuty=1000;flag_set_qiben=0;}   
   if(Set_MianBan_Config_QingXi==3)	   // ������ ˮ�� 100% 
   		{set_XSBDuty=1000;flag_set_qiben=1;}
}


// ����ˮ��
//static u8 TiaoJieShuiWen(void)
//{
//	u8 set_tt;
//   if(Set_A_Config.ShuiWen==0)	   // ��ˮ
//	   set_tt=0;
//   if(Set_A_Config.ShuiWen==1)	   // 35��
//	   set_tt=35;
//   if(Set_A_Config.ShuiWen==2)	   // 38��
//	   set_tt=38; 	   
//   if(Set_A_Config.ShuiWen==3)	   // 41�� 
//	   set_tt=41; 		   
//	if(set_tt>42)
//		set_tt = 42;	// �趨�����
//	return set_tt;
//}
static u8 TiaoJieShuiWen(void)
{
	u8 set_tt;
   if(Set_MianBan_Config_ShuiWen==0)	   // ��ˮ
	   set_tt=0;
   if(Set_MianBan_Config_ShuiWen==1)	   // 35��
	   set_tt=35;
   if(Set_MianBan_Config_ShuiWen==2)	   // 38��
	   set_tt=38; 	   
   if(Set_MianBan_Config_ShuiWen==3)	   // 41�� 
	   set_tt=41; 		   
	if(set_tt>42)
		set_tt = 42;	// �趨�����
	return set_tt;
}

//static u8 TiaoJieNuanFen(void)
//{
//	u8 set_tt;
//	if(Set_A_Config.NuanFen==0) 	// ���
//		set_tt=0;
//	if(Set_A_Config.NuanFen==1) 	// 45��빦��
//		set_tt=40;
//	if(Set_A_Config.NuanFen==2) 	// 55��
//		set_tt=60;	
////		set_tt=80;		
//	if(Set_A_Config.NuanFen==3) 	// 65�� ȫ���ʿ�
//		set_tt=80;	
////	set_tt=100;				
//	return set_tt;
//}

static u8 TiaoJieNuanFen(void)
{
	u8 set_tt;
	if(Set_MianBan_Config_NuanFeng==0) 	// ���
		set_tt=0;
	if(Set_MianBan_Config_NuanFeng==1) 	// 45��빦��
		set_tt=45;
	if(Set_MianBan_Config_NuanFeng==2) 	// 55��
		set_tt=65;	
//		set_tt=80;		
	if(Set_MianBan_Config_NuanFeng==3) 	// 65�� ȫ���ʿ�
		set_tt=85;	
//		set_tt=100;				
	return set_tt;
}

// ����
static void DoGanZao(void)
{
	unsigned int i;
	u8 j,tmp;
	u8 d_set_tt;

	if(!IsOpen())
	{
		rt_kprintf("����δ��\r\n");
		return;
	}
	g_sensor.FunctionWorkNuanFeng=1;
	rt_kprintf("��ʼ��ɶ���\r\n");
	// �����̵�������ʼ���ȣ��������ȣ����ҿ�ʼ��ȡ�¶�ֵ������ʱ��Ϊ�趨ʱ��
	SHI_DIAN_ZONG_KAI_GUAN(1);		// �����е�����
	HONG_GAN_PI_GU_FENG_SHAN(1);	// ����ů�����
	for(i=0;i<(m_ActionCfg.HongGanShiJian);i++)   //  1Sһ������
	{
		d_set_tt= TiaoJieNuanFen();
		// 2 ��ȡ������¶�ֵ
		//g_sensor.ChuFengWenDu =	ReadWenDu();

	 // 3 �¶����ݶԱȵ���PWM
//		if(!d_set_tt)		// �ڷ����ģʽ��
//		{
//			tmp = (d_set_tt-50)/2+40;	// ת�¶��趨ֵ
//			// ����¶�ֵδ�ﵽ�趨ֵ������1S��δ���¶��ϵ������µ����趨ֵ
//			if(g_sensor.ChuFengWenDu<tmp)
//			{	
//				tmp=tmp-g_sensor.ChuFengWenDu;
//				if(tmp>=10)
//					d_set_tt+=4;
//				if((tmp>5)&&(tmp<10))
//					d_set_tt+=2;
//			}
//			// �¶ȹ��� ���µ�
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
		HONG_GAN_PI_GU_DIAN_RE_SI(1);	// ����ů�����˿������˿���Ʋ���ģ��PWM2��һ���ڣ�100mSһ����λ
		for(j=0;j<100;j++)	
		{
		 if(j>=d_set_tt)	//��������趨ֵ �رռ���
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


// ��ϴ�β�   ��ˮ�� 0.72L ��ʱ40S
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
	rt_kprintf("��ʼ��ϴ�β�\r\n");
	// 1 �ȷ���ˮ
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������
	RE_SHUI_FANG_SHUI_FA(1);
	m_XSBDuty=1000;
	XIAO_SHUI_BENG(1);
	SuReQiSet(1,38);	
	rt_thread_delay(RT_TICK_PER_SECOND*0.1f);
	SuReQiRead(buff); 
	m_SuReQiState=1;	// �����߳�static void ReShuiTimer(void *p)
	m_SuReQiLengShuiTimer=0;
	rt_thread_delay(50); 		// �������˴�ͨѶ�������200mS
	
	for(i=0;i<(m_ActionCfg.TunBuQingXiShiJian*2);i++)	// 30ML*40S=1.2Lˮmax
	{
		d_set_tt=TiaoJieShuiWen();
		SuReQiSet(1,d_set_tt);  // ������ÿ��������һ���¶�
		TiaoJieLiDu();
		if(WaitTimeout(RT_TICK_PER_SECOND*0.1f, (1<<STOP)|(1<<QingXiTunBu)))
		{
			m_SuReQiState=0;
			m_SuReQiLengShuiTimer=0;
			SuReQiSet(0,0);		// �������ر�			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_HOU_BU_SHUI_FA(0);	
			ZENG_YA_BENG_ZHU_SHUI_FA(0);     // ��������		
			g_sensor.FunctionWorkHouBu=0;			
			rt_kprintf("��\r\n");
			PAI_WU_SUI_WU_BENG(1);//	���۱ÿ���
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
			SuReQiSet(0,0);		// �������ر�			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_HOU_BU_SHUI_FA(0);	
				ZENG_YA_BENG_ZHU_SHUI_FA(0);     // ��������
			g_sensor.FunctionWorkHouBu=0;			
			rt_kprintf("��\r\n");
			PAI_WU_SUI_WU_BENG(1);//	���۱ÿ���
			rt_thread_delay(RT_TICK_PER_SECOND*1);
			PAI_WU_SUI_WU_BENG(0);			
			return;
		}
	}
	SuReQiSet(0,0); 	// �������ر�			
	XIAO_SHUI_BENG(0);
	m_SuReQiState=0;
	m_SuReQiLengShuiTimer=0;
	XIAO_QI_BENG(0);
	RE_SHUI_FANG_SHUI_FA(0);
	RE_SHUI_HOU_BU_SHUI_FA(0);
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // ��������
	
	CHONG_XI_SHUI_FA_TO_MATONG(1);		// ��ˮ
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);
	rt_thread_delay(RT_TICK_PER_SECOND*(m_ActionCfg.BianPenShouDongQingJieShiJian+2));		
	PAI_WU_SUI_WU_BENG(1);//	���۱ÿ���
	rt_thread_delay((TIME_PAI_WU_BENG-3)*RT_TICK_PER_SECOND/2);
	PAI_WU_SUI_WU_BENG(0);
	DoShuiFeng();
	g_sensor.FunctionWorkHouBu=0;
	flag_houbu=0;

}

// ��ϴ��ʿǰ��  ��ˮ�� 0.4L����ʱ 15S
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
	rt_kprintf("��ϴ��ʿǰ��\r\n");
	// 1 �ȷ���ˮ
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������
	RE_SHUI_FANG_SHUI_FA(1);
	m_XSBDuty=1000;
	XIAO_SHUI_BENG(1);
	SuReQiSet(1,38);
	rt_thread_delay(RT_TICK_PER_SECOND*0.1f);
	SuReQiRead(buff); 
	m_SuReQiState=1;	// �����߳�static void ReShuiTimer(void *p)
	m_SuReQiLengShuiTimer=0;
	rt_thread_delay(50); 		// �������˴�ͨѶ�������200mS
	
	for(i=0;i<((m_ActionCfg.QianBuYanShi+20)*2);i++)		// 20s*30mL=0.6L	+20s
	{
		d_set_tt=TiaoJieShuiWen();	
		SuReQiSet(1,d_set_tt);  // ������ÿ��������һ���¶�
		if(WaitTimeout(RT_TICK_PER_SECOND*0.1f, (1<<STOP)|(1<<QingXiQianBu)))
		{
			m_SuReQiState=0;
			m_SuReQiLengShuiTimer=0;
			SuReQiSet(0,0);		// �������ر�			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_QIAN_BU_SHUI_FA(0);	
		ZENG_YA_BENG_ZHU_SHUI_FA(0);     // ��������
			g_sensor.FunctionWorkQianBu=0;			
			rt_kprintf("��\r\n");
			return;
		}
		SuReQiRead(buff); 
		g_sensor.SuReQiWork=buff[0];
		g_sensor.SuReQiError=buff[1];
		g_sensor.LengShuiWenDu= buff[2];
		g_sensor.ReShuiWenDu = buff[3];
		templiuliang=(buff[4]<<8)+buff[5];

//		// ��������
//		if(i>30)	//15���Ժ�
//		{
//			if(g_sensor.ReShuiWenDu<d_set_tt)	// ����¶ȴﲻ���趨�¶�
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
			SuReQiSet(0,0);		// �������ر�			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_QIAN_BU_SHUI_FA(0);
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // ��������
			g_sensor.FunctionWorkQianBu=0;			
			rt_kprintf("��\r\n");
			return;
		}
	}
	SuReQiSet(0,0); 	// �������ر�			
	XIAO_SHUI_BENG(0);
	m_SuReQiState=0;
	m_SuReQiLengShuiTimer=0;
	XIAO_QI_BENG(0);
	RE_SHUI_FANG_SHUI_FA(0);
	RE_SHUI_QIAN_BU_SHUI_FA(0);	
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // ��������
	CHONG_XI_SHUI_FA_TO_MATONG(1);		// ��ˮ
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);
	rt_thread_delay(RT_TICK_PER_SECOND*(m_ActionCfg.BianPenShouDongQingJieShiJian+2));	
	PAI_WU_SUI_WU_BENG(1);//	���۱ÿ���
	rt_thread_delay((TIME_PAI_WU_BENG-3)*RT_TICK_PER_SECOND/2);
	PAI_WU_SUI_WU_BENG(0);
	DoShuiFeng();						// ��ֹ��ζ	
	g_sensor.FunctionWorkQianBu=0;
	flag_qianbu=0;
}


// ����ģʽ�º�̱��ģʽ�£��Զ���ɽ����̡�

static char zidongpaiwu_stack[512];
static struct rt_thread zidongpaiwu_thread;

// �Զ����۽��̣������ȼ�����
static void ZiDongPaiWuThreadEntry(void* parameter)
{
	rt_kprintf("�����Զ����۽���\r\n");
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
//			DoVacuumPump(10);		// ����ˮ������	
			if(g_sensor.DaBian)
			{
				rt_kprintf("��⵽��㣬��ʼ�Զ����۶���\r\n");
				g_sensor.FunctionWorkHouBu=1;
				CHONG_XI_SHUI_FA_TO_MATONG(0);			// �򿪵���Ͱ��ˮ��
				ZHI_CHONG_SHUI_FA(1);					// ����Ͱ��ϴ-ֱ�巧
				QING_SHUI_CHONG_XI_ZENG_YA_BENG(1); 	// ����ѹˮ��
				rt_thread_delay(RT_TICK_PER_SECOND*4);	// 7 �� ��Ҫͬʱ�����������ʹ���
				CHONG_XI_SHUI_FA_TO_MATONG(1);		// �򿪲�ڳ�ϴ
				ZHI_CHONG_SHUI_FA(0);					// �ر���Ͱ��ϴ-ֱ�巧
				rt_thread_delay(RT_TICK_PER_SECOND*4);	// 7 �� ��Ҫͬʱ�����������ʹ���
				PAI_WU_SUI_WU_BENG(1);				// �����۱�				
				rt_thread_delay(RT_TICK_PER_SECOND*(m_ActionCfg.BianPenShouDongQingJieShiJian-3));				
			}
			else 
			{
				rt_kprintf("��⵽С�㣬��ʼ�Զ����۶���\r\n");
				if(g_sensor.ManXiaoBian)
					g_sensor.FunctionWorkQianBu=1;		// ����
				if(g_sensor.WomenXiaoBian)
					g_sensor.FunctionWorkQianBu=1;		// ����					
				//	g_sensor.FunctionWorkHouBu=1;		// Ů��
				CHONG_XI_SHUI_FA_TO_MATONG(1);			// �򿪵���Ͱ��ˮ��
				QING_SHUI_CHONG_XI_ZENG_YA_BENG(1); 	// ����ѹˮ��
				rt_thread_delay(RT_TICK_PER_SECOND*3);	//��Ҫͬʱ�����������ʹ���
				PAI_WU_SUI_WU_BENG(1);				// �����۱�				
				rt_thread_delay(RT_TICK_PER_SECOND*1);
			}

			QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 	// �ȹ�ˮ��
			CHONG_XI_SHUI_FA_TO_MATONG(0);			
			rt_thread_delay(RT_TICK_PER_SECOND/2);	// 1/2 �� ��Ҫͬʱ�����������ʹ���
			PAI_WU_SUI_WU_BENG(0);				// �ٹ����۱�

			g_sensor.DaBian = 0;
			g_sensor.ManXiaoBian = 0;
			g_sensor.WomenXiaoBian = 0;
			rt_kprintf("��ʼ�Զ����۶������������µȴ���С���ź�\r\n");
			rt_thread_delay(RT_TICK_PER_SECOND*10);
		}
	}
}

// �Զ������߳����ñ�־
static u8 ZiDongPaiWuThreadRun = 0;

static void StartZiDongPaiWu(void)
{
	u32 i;
	if(!Set_B_Config.ZhiNengModel)   // ����ģʽδ��
		return;
	for(i=0;i<90;i++)
	{
		if(!g_bedSensor.BianMenKaiDaoDing)	// �ȴ����ſ�����
		{
			if(WaitTimeout(RT_TICK_PER_SECOND*0.5f, (1<<STOP)|(1<<ZuoBianKaiShi)|((1<<ZuoBianJieShu))))
			{
				return;
			}
		}
	}
	if(!g_bedSensor.BianMenKaiDaoDing)	// ʱ�䵽������δ��λ
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
	rt_kprintf("ֹͣ�Զ�����\r\n");
	rt_enter_critical();
	if(ZiDongPaiWuThreadRun)
	{
		rt_thread_detach(&zidongpaiwu_thread);
		DoStop();
	}
	ZiDongPaiWuThreadRun = 0;
	rt_exit_critical();
}

// �����������
static void DoZuoBianJieShu(void)
{
	u16 i;u8 j,buff[7];
	u8 d_set_tt=38;	
	u16 yanshi=0;

	if(!Set_B_Config.ZhiNengModel)   // ������ģʽ���˳�
		return;
	if(!AllowChongShui())
	{
		return;
	}
	rt_kprintf("��ʼ�����������\r\n");
	g_sensor.FunctionWorkAuto=1;

	// 1. ��ϴǰ��
		ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������
	RE_SHUI_FANG_SHUI_FA(1);
	m_XSBDuty=1000;
	XIAO_SHUI_BENG(1);	 
	SuReQiSet(1,d_set_tt);
	rt_thread_delay(RT_TICK_PER_SECOND*0.1f);
	SuReQiRead(buff); 	
	m_SuReQiState=1;	// �����߳�static void ReShuiTimer(void *p)
	m_SuReQiLengShuiTimer=0;
	rt_thread_delay(50); 		// �������˴�ͨѶ�������200mS

	if(g_sensor.FunctionWorkQianBu)
		yanshi=m_ActionCfg.QianBuYanShi + 20;   // ����20S

	if(g_sensor.FunctionWorkHouBu)
		yanshi=m_ActionCfg.TunBuQingXiShiJian;

	for(i=0;i<(yanshi*2);i++)	// 30ML*40S=1.2Lˮmax
	{
		if(g_sensor.FunctionWorkHouBu&&g_sensor.FunctionWorkQianBu)  // ǰ��һ���ϴ
		{	
			if(i>=m_ActionCfg.TunBuQingXiShiJian*2)  // ǰ����ϴʱ�䵽
			{	
				RE_SHUI_QIAN_BU_SHUI_FA(0);			 // ǰ��ˮ���ر� ��������
				g_sensor.FunctionWorkQianBu= 0;
			}
		}
		d_set_tt=TiaoJieShuiWen();	
		SuReQiSet(1,d_set_tt);  // ������ÿ��������һ���¶�
		if(WaitTimeout(RT_TICK_PER_SECOND*0.1f, (1<<STOP)|(1<<ZuoBianJieShu)))
		{
			m_SuReQiState=0;
			m_SuReQiLengShuiTimer=0;
			SuReQiSet(0,0);		// �������ر�			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_QIAN_BU_SHUI_FA(0);
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // ��������
			g_sensor.FunctionWorkAuto=0;
			g_sensor.FunctionWorkHouBu=0;
			g_sensor.FunctionWorkQianBu=0;
			rt_kprintf("��\r\n");
			return;
		}
		SuReQiRead(buff); 
		g_sensor.SuReQiWork=buff[0];
		g_sensor.SuReQiError=buff[1];
		g_sensor.LengShuiWenDu= buff[2];
		g_sensor.ReShuiWenDu = buff[3];

		// ��������
//		if(i>30)	//15���Ժ�
//		{
//			if(g_sensor.ReShuiWenDu<d_set_tt)	// ����¶ȴﲻ���趨�¶�
//				if((d_set_tt-g_sensor.ReShuiWenDu)>2)
//					d_set_tt+=1;
//			else
//				if(((g_sensor.ReShuiWenDu-d_set_tt)>2)&&(!d_set_tt))
//					d_set_tt-=1;
//		}
//		if(d_set_tt>42)
//			d_set_tt = 42;	// �趨�����		
		if(WaitTimeout(RT_TICK_PER_SECOND*0.3f, (1<<STOP)|(1<<ZuoBianJieShu)))
		{
			m_SuReQiState=0;
			m_SuReQiLengShuiTimer=0;
			SuReQiSet(0,0);		// �������ر�			
			XIAO_SHUI_BENG(0);
			XIAO_QI_BENG(0);
			RE_SHUI_FANG_SHUI_FA(0);
			RE_SHUI_HOU_BU_SHUI_FA(0);	
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // ��������			
			g_sensor.FunctionWorkAuto=0;
			g_sensor.FunctionWorkHouBu=0;
			g_sensor.FunctionWorkQianBu=0;			
			return;
		}
	}
	SuReQiSet(0,0); 	// �������ر�			
	XIAO_SHUI_BENG(0);
	m_SuReQiState=0;
	m_SuReQiLengShuiTimer=0;
	g_sensor.FunctionWorkHouBu=0;
	g_sensor.FunctionWorkQianBu=0;
	XIAO_QI_BENG(0);
	RE_SHUI_FANG_SHUI_FA(0);
	RE_SHUI_HOU_BU_SHUI_FA(0);	
	ZENG_YA_BENG_ZHU_SHUI_FA(0);     // ��������

	// 2. ��ϴ����
	CHONG_XI_SHUI_FA_TO_MATONG(1);			// �򿪵���Ͱ��ˮ��
	QING_SHUI_CHONG_XI_ZENG_YA_BENG(1);		// ����ѹˮ��
	if(WaitTimeout((m_ActionCfg.BianPenZiDongPaiWuYanShi+2+10)*RT_TICK_PER_SECOND, (1<<STOP)|(1<<ZuoBianJieShu)))
	{
		QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 
		PAI_WU_SUI_WU_BENG(0);				
		CHONG_XI_SHUI_FA_TO_MATONG(0);		
		g_sensor.FunctionWorkAuto=0;
		return;
	}
	PAI_WU_SUI_WU_BENG(1);//	���۱ÿ���
	rt_thread_delay(TIME_PAI_WU_BENG*RT_TICK_PER_SECOND/2);	// 2 �� ��Ҫͬʱ�����������ʹ���
	
//	QING_SHUI_CHONG_XI_ZENG_YA_BENG(0); 	// �ȹ�ˮ��
//	CHONG_XI_SHUI_FA_TO_MATONG(0);			
//	rt_thread_delay(RT_TICK_PER_SECOND/2);	// 1/2 �� ��Ҫͬʱ�����������ʹ���
	PAI_WU_SUI_WU_BENG(0);				// �ٹ����۱�
	DoShuiFeng();

	// 3 ����
	SHI_DIAN_ZONG_KAI_GUAN(1);		// �����е�����
	HONG_GAN_PI_GU_FENG_SHAN(1);	// ����ů�����
	for(i=0;i<(m_ActionCfg.HongGanShiJian);i++)   //  1Sһ������
	{
		d_set_tt = 80;

		HONG_GAN_PI_GU_DIAN_RE_SI(1);	// ����ů�����˿������˿���Ʋ���ģ��PWM2��һ���ڣ�100mSһ����λ
		for(j=0;j<100;j++)	
		{
		 if(j>=d_set_tt)	//��������趨ֵ �رռ���
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
// ���ڼ��ȹ��������Ҫ24V��5A���ҡ����ԣ��ڴ����豸����ʱ���ܼ��ȣ�
// ������κβ������ƣ���ͣ����ģʽ,���������󣬻ָ�����ģʽ��
// 1.ͣ�磬���ع���ʱ��2.�綯�Ƹ�����ʱ��3.����������ʱ��
// ���ڵ綯�Ƹ˵����У�û����ȡ���������ҹ��ʻ���<5A,��ʱ��������ͣ���ȹ�����
static int AllowJiaRePower(void)
{
	if(PowerWorkFlag)
	{	rt_kprintf("����������ʱ\r\n");
		return 0;}
	if(g_bedSensor.TingDian)
	{
		rt_kprintf("ͣ�磬���ع���ʱ\r\n");
		return 0;	
	}
	return 1;
}
static char chuangdianjiare_stack[512];
static struct rt_thread chuangdianjiare_thread;

static void JiaReThreadEntry(void )
{
	u8 c_set_tt,c_read_tt;
	rt_kprintf("������Ƚ���\r\n");
	while(1)
	{
		if(AllowJiaRePower())
		{	
//			c_read_tt=g_sensor.ChuanDianWenDu;		// ��ȡ�����¶�
			c_set_tt= Set_B_Config.ChuangDianJiaRe;	//�趨�¶�
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


// ��������߳����ñ�־
static u8 JiaReThreadRun = 0;
// �����Զ����ȴ���
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
	rt_kprintf("ֹͣ�������\r\n");
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

// ���յ�CAN�ص�����
extern rt_err_t CanRxInd(rt_device_t dev, rt_size_t size);
extern void CanAnalyzerInit(void);

static struct rt_timer reshui_timer;
// ��ˮ�ö�ʱ��ÿ�����Ӧ����
#define	RE_SHUI_BENG_TIMER_PER_SEC	2

// ������ˮ�ó�ˮ��ʱ��
// ÿ0.5 �����һ��
static void ReShuiTimer(void *p)
{	
	feedWD();	 // ι��
	if(!g_bedSensor.BianMenKaiDaoDing)			// ����δ��
	{
			return ;
	}
	if(m_SuReQiState == 1)   // ��ʼ��ˮ·Ϊ�м�λ
	{	
		if(++m_SuReQiLengShuiTimer/RE_SHUI_BENG_TIMER_PER_SEC > m_LSSC) //����10S
		{
			m_SuReQiLengShuiTimer = 0;
			m_SuReQiState = 2;
			if(g_sensor.FunctionWorkQianBu)
			{
				m_XSBDuty=QIAN_BU_CHONG_XI_LI_DU; 	
		ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������			
				XIAO_SHUI_BENG(1);
			}
			if(g_sensor.FunctionWorkHouBu)
			{	
				m_XSBDuty=set_XSBDuty;		
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������				
				XIAO_SHUI_BENG(1);
			}
		}	
		if(m_SuReQiLengShuiTimer==10) // ǰ5 �������ˮ ,5�����������ˮ
		{
			m_XSBDuty=QING_XI_PENG_TOU;		
		ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������		
			XIAO_SHUI_BENG(1);
			RE_SHUI_FANG_SHUI_FA(0);
			if(g_sensor.FunctionWorkHouBu )
			{	RE_SHUI_HOU_BU_SHUI_FA(1);}
			if(g_sensor.FunctionWorkQianBu)
			{	RE_SHUI_QIAN_BU_SHUI_FA(1);}
		}
		if(m_SuReQiLengShuiTimer>12) // ǰ5 �������ˮ �м�5������Ž�ͷ��·ˮ
		{
//				m_XSBDuty=QIAN_BU_CHONG_XI_LI_DU;
//				XIAO_SHUI_BENG(1);
			if(g_sensor.FunctionWorkHouBu)
			{
				m_XSBDuty+=100;
				if(m_XSBDuty>=set_XSBDuty)
					m_XSBDuty=set_XSBDuty;
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������				
				XIAO_SHUI_BENG(1);
			}
			if(g_sensor.FunctionWorkQianBu)
			{
				m_XSBDuty+=50;
				if(m_XSBDuty>=QIAN_BU_CHONG_XI_LI_DU)
					m_XSBDuty=QIAN_BU_CHONG_XI_LI_DU;
	ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������				
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
						ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������
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
	u8 timeS = 7;   //��ת����X��
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
	u8 timeS = 7;   //��ת����X��
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

// �����������
void ActionThreadEntry(void* parameter)
{
	rt_uint32_t cmd,evt;
	
	ActionInit();
	// �� CAN1 �豸
	CanAnalyzerInit();	
	rt_timer_init(&reshui_timer, "re shui", ReShuiTimer, RT_NULL, RT_TICK_PER_SECOND*0.5f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
	rt_timer_start(&reshui_timer);

//	SuReQiSet(0,Set_B_Config.ShuiWen);  // ��ʼ�������� ������
	
 /* �����߳�1  ������ת������ʼ��Ϊ����ģʽ*/	
	rt_thread_init(&thread_rotate,
							 "rotate",
							 thread1_entry, RT_NULL,
							 &thread_rotate_stack[0], sizeof(thread_rotate_stack),
							 12, 10);
	rt_thread_startup(&thread_rotate);
			
	rt_kprintf("init system end\r\n");
	while(1)
	{
		// �ȴ������ʼ�����
		rt_mb_recv(&m_mb, &cmd, RT_WAITING_FOREVER);	// �ȴ���������
		DoStop();
		rt_kprintf("���յ��������� %d\r\n",(int)cmd);
		rt_event_recv(&m_evt,0xffffffff , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, RT_WAITING_NO, &evt);	// ���¼�
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
				rt_kprintf("ǿ��ֹͣ���ж���\r\n");
				break;				
			default:
				DoStop();
				rt_kprintf("û���ҵ���Ӧ�Ķ���ָ��\r\n");
				break;
		}
		DoStop();
		rt_mb_recv(&m_mb, &cmd, RT_WAITING_NO);	// ������
		rt_event_recv(&m_evt,0xffffffff , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, RT_WAITING_NO, &evt);	// ���¼�
		rt_kprintf("��������\r\n");
	}
}

