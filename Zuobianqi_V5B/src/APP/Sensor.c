#include "Sensor.h"
#include "Adc.h"
extern void rotateDining(u8 work,u8 dir);
extern u8 rotateDiningCount;
extern u8 rotateDiningDir;
// ������״̬ȫ�ֱ���
volatile s_Sensor g_sensor = {0};		
// ���崫����״̬ȫ�ֱ���
volatile s_BedSensor g_bedSensor = {0};
volatile s_BedSensor_MT_work g_bedsensor_MT_work={0};
// ����������е���Ƹˣ���ΪTA16,4.7mm/s,110mm�г�,ʱ����25s�����г�55mm���и��м�λ�ô�����
#define TIME_RUN_DINING_MT 		21		// ʵ��ʱ��
//#define TIME_RUN_DINING_MT 		10  //13  �޸����Ƹ˳ߴ�130��Ϊ100mm //S �Ƹ�����ʱ�� ��Ҫͬ������ư�Ĳ���һ�£�Ϊ�����Ƹ�����ʱ��
// ����һ�δ�����״̬�������˲�
static s_Sensor m_lastSensorStatus;
static u8 m_lastBianMen = 0;   // ���Ŵ򿪼�¼
// עˮ ˮ�� 5L/min;
static u8 m_lastzhushui=0;
static u8 g_actionzhushui=0;
#define ZHU_SHUI_TIME 80//100   // 10S���� ����10S��ֹͣ
#define QI_DONG_TIME 200    // 20S ����עˮʱ���ڽ�ֹ���������
#define ZENG_YA_FA_TIME 200   // 20S
static u8 g_zhushui_time=0;
static u8 g_qidong_time=0;		// עˮ����������ֱ��עˮֹͣ
// �Զ���ϴ������
static u8 g_actionzuobianquan=0;
static u8 m_lastbianmenguanbi=1;
//#define ZUO_BIAN_QUAN_TIME 100  // 10S*0.33L/S=3.3L
static u8 g_zuobianquan_time=0;
static u16 DianJiDianLiuAD1=0;
#define Pai_Wu_Beng_Guoliu 100	// ������������ 7A
static u8 DaDianJiGuoliuCount=0;
static u8 m_waterIO = 0;
static u8 m_flag_waterIO = 0;
static u16 water_cnt =0;
// ����
#define CHENG_ZHONG_TIME  20      // 2s1�ζ�ȡ�� 100ms/�μ���
static u32 g_chengzhong_time = 0;  // ���������� 
static u16 TuoJiaZhongLiang=(230+610);//230;	// �мܺ����̵�����g
#define WU_WU_XIANG_KONG  	1313// 500  	// ������յ����� g
#define WU_WU_XIANG_BU_ZAI_WEI  1000    // �����䲻��λ��
#define WU_WU_XIANG_BORDER 8000	// �������ٽ�ֵ g
#define Cheng_Zhong_Work(on)	if(on){IO1CLR = BIT26;} else {IO1SET = BIT26;} 
unsigned long ChengZhongReg=0;

#define WenDuJianCe_TIME  100      // 1 MIN 60s 100ms/��
static u32 g_wendu_time = 0;  //�¶ȼ���ʱ

u8 flag_rotate = 0;    // ��ʾ��������ת�������

#ifdef RT_USING_FINSH

#include "CAN/IncludeCan.h"
// ��ʾ��������Ϣ
static void SensorList(void * parameter)
{
	s32 i;
	rt_kprintf("��ǰGPIO����:\r\n");
	rt_kprintf("\tPINSEL0=%04X    PINSEL1=%04X    PINSEL2=%04X    IO0DIR=%04X    IO1DIR=%04X    IO0PIN=%04X    IO1PIN=%04X    \r\n",
		PINSEL0,PINSEL1,PINSEL2,IO0DIR,IO1DIR,IO0PIN,IO1PIN);
	rt_kprintf("��ǰCAN����:\r\n");
	rt_kprintf("\tCAN0MOD=%04X	CAN0ICR=%04X	CAN0SR=%04X\r\n",CANMOD(0).Word,CANICR(0).Word,CANSR(0).Word);
	
	rt_kprintf("��ǰ������״̬:\r\n");
	rt_kprintf("\t���ſ�: %s\r\n",g_bedSensor.BianMenKaiDaoDing?"Y":"");
	rt_kprintf("\t���Ź�: %s\r\n",g_bedSensor.BianMenGuanDaoDi?"Y":"");
	rt_kprintf("\t���: %s\r\n",g_sensor.DaBian?"Y":"");
	rt_kprintf("\t��ʿС��: %s\r\n",g_sensor.ManXiaoBian?"Y":"");
	rt_kprintf("\tŮʿС��: %s\r\n",g_sensor.WomenXiaoBian?"Y":"");
	rt_kprintf("\t�������·����״̬: %s\r\n",g_sensor.WuWuXiangGuan?"Yes":"No");	
	rt_kprintf("\tˮ���·����״̬: %s\r\n",g_sensor.ShuiXiangLow?"No":"Yes");		
	
	rt_kprintf("\tˮ����: %s\r\n",g_sensor.ShuiXiangHigh?"Y":"");	
	rt_kprintf("\tˮ���: %s\r\n",g_sensor.ShuiXiangLow?"Y":"");	
	rt_kprintf("\t�Զ�עˮˮ�ù���: %s\r\n",g_actionzhushui?"Y":"");
	rt_kprintf("\t����������: %s\r\n",g_sensor.SuReQiWork?"Y":"");
	rt_kprintf("\tͣ��: %s\r\n",g_bedSensor.TingDian?"Y":"");
	rt_kprintf("\t��������: %s\r\n",g_bedSensor.DianLiangBuZu?"Y":"");
	rt_kprintf("\t��������������: %d\r\n",(u32)g_sensor.SuReQiError);
	rt_kprintf("\t����������: %d\r\n",(u32)g_sensor.WuWuXiangZhongLiang);
	rt_kprintf("\t���ؼĴ���ֵ: %d\r\n",(u32)ChengZhongReg);
	rt_kprintf("\t������ת���: %d\r\n",g_sensor.rotateDiningCnt);
	rt_kprintf("\trotate: %d\r\n",rotateDiningCount);
//	rt_kprintf("\t�����¶�: %d\r\n",(u32)g_sensor.ChuanDianWenDu);
	rt_kprintf("\tg_sensor= %X, %X\r\n",(u32)(*((u8*)(&g_sensor))),(u32)(*(((u8*)(&g_sensor))+1)));
}
FINSH_FUNCTION_EXPORT(SensorList, "��ʾ��ǰ������״̬");


// �Զ����
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
FINSH_FUNCTION_EXPORT(AutoSensor, "�Զ���ش�����,����[0]=0��ʾֹͣ�������ʾ��ʼ");


#endif

BOOL Read_Cheng_Zhong_Dout(void)
{
	if(0==(IO1PIN&BIT16))
		return 0;
	else
		return 1;
}

// 24λ�������
static unsigned long ChengZhongReadCount(void)
{
	unsigned long Count,j;
	unsigned char i;
	Cheng_Zhong_Work(1); //ʹ��AD��PD_SCK �õͣ�
	Count=0;
//	while(Read_Cheng_Zhong_Dout()); //ADת��δ������ȴ�������ʼ��ȡ
	for(j=0;j<10000;j++)
	{
		if(j==9999)
			return 0xffffff;		//	ͨѶ�쳣
		if(Read_Cheng_Zhong_Dout())
			DelayXus(100);	//��ʱ 100uS
		else
			break;			// �Ѿ�������ͨѶ��ʼ		
	}
//	rt_kprintf("\tj: %d\r\n",(u32)j);	
	
	for (i=0;i<24;i++)
	{
		Cheng_Zhong_Work(0); //PD_SCK �øߣ��������壩
		Count=Count<<1; //�½�����ʱ����Count����һλ���Ҳಹ��
		DelayXus(2);	//��ʱ 2uS
		Cheng_Zhong_Work(1); //PD_SCK �õ�
		if((IO1PIN&BIT16)>0) Count+=1;
		DelayXus(2);	//��ʱ 2uS			
	}
	Cheng_Zhong_Work(0);
//	Count=Count^0x800000;//��25�������½�����ʱ��ת������
	if((Count& 0x800000) == 0x800000)
	{
		Count= (~(Count - 1)&0xffffff);		   //�������ɲ�������������λ��ԭ��ļ��㡣
	}	  		
	DelayXus(2);	//��ʱ 2uS	
	Cheng_Zhong_Work(1);
	DelayXus(2);	//��ʱ 2uS	
	return(Count);
}

// �ѳ���ֵΪ�Ĵ���ֵת��Ϊ������λg  �������������ֵΪ20KG
// 4.3V ���磬20kg�������� ������ 2mV/V �൱����������������ѹ8.6mV,��С�ֱ�� 2.86g
// hx711 �Ŵ�128�����൱����������������ѹ 1100.8mV(1.1V),24λת����Reg=4294967,
// ��������ֵΪA kg,��õ�ADֵΪReg
// AD��ѹ=A*8.6mV/20kg  128���Ŵ���תΪ24bit�����ź�=(A*8.6mV/20Kg)*128*2^24/4.3V
// �൱��Reg = 214748.36*A, ��kgתΪg�󣬵õ�Reg = 214.75*A,
// ���ԣ��������A=Reg/214.75 (g) ��������
static unsigned int ChengZhongRegToG(unsigned long Reg)
{
	unsigned int Weight_Shiwu;
	Weight_Shiwu = (unsigned int)((float)Reg/210.00+0.05);		// ԭʼֵ 214.75
	if(Weight_Shiwu>TuoJiaZhongLiang)
		Weight_Shiwu = Weight_Shiwu - TuoJiaZhongLiang;	// ��ȥ�����мܵ�����
	else
		Weight_Shiwu = 0 ;
	if(0xffffff==Reg)	// ��ʾͨѶ�쳣
		Weight_Shiwu = 0xffff;
	return Weight_Shiwu;
}

// ��ʱ�������������Ե��ô�������⺯��
static struct rt_timer sensorTimer;

// Ӳ����ʼ��
static void SensorHwInit(void)
{
	Cheng_Zhong_Work(0);
	IO1DIR &= ~(BIT16);		// ����ֵ
	IO0DIR &= ~(BIT30);		// ˮ��ˮλ ��
	IO0DIR &= ~(BIT29);     // ˮ��ˮλ ��
//	IO0DIR &= ~(BIT2);		// ��·���ӿ���
	IO0DIR &= ~(BIT3);      // עˮ��������
	IO0DIR &= ~(BIT4);	    // עˮˮ������
	IO1DIR &= ~(BIT17);		// �����
	IO1DIR &= ~(BIT18);     // ŮʿС����
	IO1DIR &= ~(BIT19);     // ��ʿС����
	IO0DIR &= ~(BIT23);     // ��Ͱ��Һλ�����
	
	AdcInit();  // ad1
}

static u8 flag_zhushuibutton=0;
static u8 m_shuiwei=0;
static u8 g_zengyafa_time=0;
static u8 flag_zengyafa=0;
static u8 m_lastdabian=0;
// ��������⣬��ʱ�����Ը���ȫ�ֱ���
static void SensorCheck(void * parameter)
{
//	unsigned long ChengZhongReg=0;
	u8 currzhushui,shuiliukaiguan;

//	g_sensor.WuWuXiangGuan		= ((IO0PIN&BIT2)>0);	// �������·����״̬
	g_sensor.WuWuXiangGuan = ((IO0PIN&BIT2)>0);	// �������·����״̬ 0��δ����
	g_sensor.ShuiXiangHigh 		= ((IO0PIN&BIT29)>0);	// ˮ����״̬
//	g_sensor.ShuiXiangLow 		= (0==(IO0PIN&BIT30));	// ˮ���״̬
	g_sensor.ShuiXiangLow = ((IO0PIN&BIT5)==0);   // ˮ���·����״̬  1��δ����
	g_sensor.DaBian 			= (0==(IO1PIN&BIT17));
	g_sensor.WomenXiaoBian		= (0==(IO1PIN&BIT18));
	g_sensor.ManXiaoBian		= (0==(IO1PIN&BIT19));
	g_sensor.MaTongFull			= (0==(IO0PIN&BIT23));	// ��Ͱ���ź�
	// ������¶�ֵ ƽʱ��ȡ 1min/pcs
	if(g_wendu_time>WenDuJianCe_TIME)
	{
		//g_sensor.ChuanDianWenDu=ReadTemperature();
		g_wendu_time=0;
	}
	else
		g_wendu_time+=1;
	
	// ����
			g_sensor.WuWuXiangZhongLiang	= 2000;

	
	// ���Ŵ򿪻�ر� ������ر���Ͱ��ҹ�ӵƣ�����1��
	if(m_lastBianMen != g_bedSensor.BianMenKaiDaoDing)
	{			if(g_bedSensor.BianMenKaiDaoDing)
				{	TOILET_LAMP_WORK(1);}
				else
				{	TOILET_LAMP_WORK(0);}	
	}				
	m_lastBianMen = g_bedSensor.BianMenKaiDaoDing;
	
	// �����Ƹ��շ�ʱ��������ת���ٵ��ͬ������
	// λ��1������ 10*TIME_RUN_DINING_MT��λ��2���뿪����λ 2*TIME_RUN_DINING_MT �� λ��3���м�λ 5*TIME_RUN_DINING_MT
	// ��ת������Ҫʱ�� 5s, ��ת��е����Ҫʱ��TIME_RUN_DINING_MT =25s ֻҪ1/5ʱ��=2*TIME_RUN_DINING_MT
	if(rotateDiningDir == 1)		// �����
	{
		if(rotateDiningCount >= 10*TIME_ROTATE_DINING)
		{
			rotateDiningCount = 10*TIME_ROTATE_DINING;
			rotateDining(0,0);
		}else{
			rotateDiningCount += 1;
		}
	}else if(rotateDiningDir == 2)		//�俿��
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
	
	if(g_bedsensor_MT_work.dining == 1)   // ����ģʽ�£��������diningCnt ���� ��Ҫ��ת���������
	{
		if(g_bedsensor_MT_work.diningCnt > 4*TIME_RUN_DINING_MT)		// 0.1Sһ�β��� max = 10*TIME_RUN_DINING_MT   6*TIME_RUN_DINING_MTΪ�𱳺ͻ�е��ͬʱ����ʱ�����ݣ�
		{
				rt_kprintf("+");
				rotateDining(1,1);   // �����    �����·����������߶����������岻����Σ��
				flag_rotate = 1;
		}	
	}else if(g_bedsensor_MT_work.dining == 2)    // �俿�� ��diningCnt ����
	{		
		if(g_bedsensor_MT_work.diningCnt < 9*TIME_RUN_DINING_MT)     // ȷ����е������(̧��)һС�ξ��룬��ת��������ײ����
		{
			rt_kprintf("-");
			rotateDining(1,0);   // �俿��			�����·����������߶����������岻����Σ��
			flag_rotate = 1;
		}
	}else{									// �������ˣ���ͣ
		if(flag_rotate)
		{
			flag_rotate = 0;
			rotateDining(0,0);
		}			
	}
	
		
	// ÿ��ˮ�ܽ��룬��Ҫ�ſ���һ��,�������·����
	if(!g_sensor.ShuiXiangLow && m_waterIO )   // ���ش���  1-->0
	{
		m_flag_waterIO = 1;
	}
	if(m_flag_waterIO && g_sensor.WuWuXiangGuan)   // ��������ʱ����
	{	
		if(water_cnt == 10*5 )   // 5S�� ����
		{
			m_XSBDuty = 1000;
			ZENG_YA_BENG_ZHU_SHUI_FA(1);     // ��������
			RE_SHUI_FANG_SHUI_FA(1);						// ��ˮ��ˮˮ����
			XIAO_SHUI_BENG(1);								// Сˮ�ô�		
			rt_kprintf("Start Pump\r\n");			
		}
		if(water_cnt > 10*(5+18))     // ��ʱ18s		
		{
			XIAO_SHUI_BENG(0);	
			RE_SHUI_FANG_SHUI_FA(0);
			ZENG_YA_BENG_ZHU_SHUI_FA(0);
			m_flag_waterIO = 0;	
			water_cnt = 0;
			rt_kprintf("End Pump\r\n");
		}
		water_cnt += 1;
	}else{				// 5S���ʱ���ڷ������������¼���
			water_cnt = 0;
	}
	m_waterIO = g_sensor.ShuiXiangLow;  // ˢ��ǰһ��״̬��Ϣ 1:δ��
	
	DianJiDianLiuAD1 = (u16)GetAdcSmoothly(0);	// 10λAD������������
	// 1S��������ȡ�������ź��൱��10�β���
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


// ��������ʼ��
void SensorInit(void)
{
	SensorHwInit();
	memset((void*)&g_sensor,0,sizeof(g_sensor));
	memset((void*)&g_bedSensor,0,sizeof(g_bedSensor));

	m_lastSensorStatus =  g_sensor;
	
	m_waterIO = g_sensor.ShuiXiangLow = ((IO0PIN&BIT5)==0);

	rt_timer_init(&sensorTimer , "sensor", SensorCheck, RT_NULL, RT_TICK_PER_SECOND*0.1f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);		// �����Ե���
	rt_timer_start(&sensorTimer);		// ��ʼ����

	AutoSensor(0);
}

