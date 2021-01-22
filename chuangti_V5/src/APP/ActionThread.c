/**
�汾˵����
	  Yan01�����ã�Ӳ��V5B;���������V5;
		1�������Ƹ� -- ������λ�ã���ʼλ�ã���е���������� ������Լ5cm ������Ƹ˻��⡣
		2���Ȳ��Ƹ� -- ������λ�ã���ʼλ�ã��м�λ��ͣ122mmΪˮƽλ
    3�������Ƹ�- �Ͳ����Ƹ˻��⣬�������������һ���ҲҪ��ȥ  150mm
		4���һ����Ƹ�- �Ͳ����Ƹ˻��⣬�������������һ���ҲҪ��ȥ  150mm
		5�������Ƹ� -- ��ϲ�����ת���(�����������ư���)�������£���ת���ת������ʱ����ƽ�ţ���ȥ������俿����130mm
		�����������ϣ�����һ��Һ������ת���������ƽ��ʱ������䣬��ɲ����俿��������������
		6���������� -- ��ϱ���ƽ������������
		7������ƽ�� -- ��ϱ����������������������� ���λ�ã��յ�λ��
		8���������� -- �����������������ƽ�ƣ�����
�ṹ�� �����������𱳣��ǻ��������ͻ���ײ��������ɻ�е�ṹ�𻵣�������������������������Ҳ�γ����صĻ�е�ṹ�𻵣�
			   ���ԣ�������������Ҫ�뿪>=5cm;��������һ�룬�������뿪ʼ�½���
����1�������Ͳ���������̫����֧�ֵ㶯��һ��ʽ����ģʽ��ʲôʱ����Ҫ��������˶�����������������ô���أ�
����2��������̫�ࡣ����ȱ�������׽ţ����º��жϽš�ͷ�����ױ������õ���

�����Ƹ˲�����
    1.�Ƹ��г� 120mm ��ʱ 11s    10mm/s  1200N

�������˶������߼���
    1.������ģʽ��һ��ʽ����ģʽ�¿�����������������˶���ͬ������������ͬ���½� ������ģʽ�£��������ɶ�
		2.��ƽ��ģʽ�º�һ��ʽ���£�Ҫ�˶�һ��ʱ������俿��������ͬ������ƽ��ģʽ�£����������ɽ�
		3.�ڵ㶯ģʽ���������ó������ռ䣬ƽ������ʱ�������һ��ʱ�䣨10S����̧��>5cm,ͣ�£���ס����λ��   (�������Ż���)
		4.�㶯ģʽ�½�����������λ�ú󣬿����俿��һ��ʱ��(10S).
		5.����ģʽ�£��ջز�������ͬ���ף���һ��ʱ��(10S)��
		6.����ģʽ�£������һ��ʱ�䣨10S����̧��>5cm,ͣ�� (�������Ż���)
		7.�ϵ��ʼ��������λ�ã�����Ϊ0����ô�������������ȷ�ƽ�󣬲����俿�������������ͣ��������Ҫ10Sλ�ã���ͣ����
		8.�������������������俿������������λ�ã�δ��ʼ��������������������������������ͬ���½���λ��
��־��
	2021.1.21.  �رտ��Ź�����Ҫ�ǳ�ʼ�������λ��������һ�����գ���Ҫ�˹��ֶ���λ��ȫһ��
							�㶯ʱ����Ҫ�ó������ռ䣬�������̣�����ͻ��
							����ģʽ�£�������ת������Ͳ�����е�۵��������һ����·���ϣ���Ҫͬ������һ���쳣����ǳ����ء�
*/
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
#include "time1.h"
#include "Adc.h"
#include <rthw.h>
#include "board.h"

// ������ر���
static struct rt_mailbox m_mb;
static rt_uint32_t mbPool[1];
// �¼�
static struct rt_event m_evt;
// ��ʱ�������ڳ�ʱ
static struct rt_timer m_timer={0};

u8 m_TuiCmd=0;				// ��¼֮ǰ�Ȳ���������
u8 m_bottonrelease=0;		// �����ͷż�¼
u8 flag_relase=0;
extern bool flag_pingtang;	// ��¼ƽ�ɴ����������ζ�˫��ͣ
extern unsigned char flag_qidong;
unsigned char flagRelaseKey=0;

#define LeftGuardSetTime 15
#define RightGuardSetTime 15
static u8 m_rotateDiningCnt=0;				// ʱ��
static u8 m_rotateAbnorCnt=0;					// �쳣����
#define TIME_ROTATE_DINING	8			// 6��

static u16 LeftGuardCount = 0;
static u16 RightGuardCount = 0;
#define BianMenShengJiangYanShi 6 		// ����������ʱʱ��10S   �г�55mm,˿������5mm,ת��200-260r/min  4S
#define BianMenPingYiYanShi  18			// ����ƽ����ʱ       10S  λ��250mm,˿������5mm,ת��200-260r/min 0.06*L  50ת  12-15S
#define ZuoBianQiShengJiangYanShi 7 //�������Ϊ�Ƹ� ����������г�25mm,��е�г�158mm 7mm/s 1500N  15	// ������������ʱ 10S   ����λ��160mm,Ƥ������5mm,����20��5mm,+ƽ��X����210mm,ת�� 7.5-10r/min    L/100V    10S-13S
//static u8 BianMen_Sheng_Count=0;		// ���������Ƹ˵���ʱ������
//static u8 BianMen_Jiang_Count=0;		// ���������Ƹ˵���ʱ������
//static u8 ZuoBianQi_Sheng_Count=0;	// �����������Ƹ˵���ʱ������
//static u8 ZuoBianQi_Jiang_Count=0;	// �����������Ƹ˵���ʱ������
static u8 ZuobianqiCount = 0;
static u8 BianmenCount = 0;
#define BeiBuYanShi  30 //  mm/s 130mm
#define TuiBuYanShi  30//35  3.9mm/s 150mm
#define FanShenYanShi 20
#define AutoJieBianYanShi  100
//u16 ChuangTiYanShiCount=0; // ���巭ת��ʱ������
#define BeiBuTongBuLowValue   200//1400    // ��������ͱ����Ƹ�ͬ�����Ƶ�   ��λ  40����/mm
#define BeiBuTongBuHighValue  BEI_MAPAN_MAX//6100    // ��������ͱ����Ƹ�ͬ�����Ƶ�   ��λ
// ��������ģʽ��1��ʾ����ģʽ
#define DONG_ZUO_DEBUG	1
// ����������е���Ƹˣ���ΪTA16,����,4.7mm/s,110mm�г�,ʱ����25s�����г�55mm���и��м�λ�ô�����
#define TIME_RUN_DINING_MT 		21 // ʵ��ʱ��
//#define TIME_RUN_DINING_MT 		10// 13  �Ƹ˳ߴ�130�޸�λ100 //S ʵ������ʱ��11.1�� 10mm/s, 120mm  �Ƹ������ף���������Ƹ�����ʼλ�ã��俿��
#define TIME_DINING_CONFLICT_HUABEI   0 // ��е��������ͻ�ˣ�����//   1		// �뻬����ͻ��λ�� >5cm,��ʱ���
u16 DiningCount = 0;    // <255   100ms/pcs    0Ϊ���� TIME_RUN_DINING_MTΪ����
//static u8 flagDiningNormal = 0;		// �����������������Ϊ1�����������Ƹ˴�������⣻
#define HUABEI_CONFLICT_POSITION	200   //��ͣһ�£����ж�һ�²���ͷ���Ƿ���ܱ�����ѹ�� //BEI_MAPAN_MAX/2    // �뻬����ͻ��λ�� 70mm  �����Ƹ���Ҫʱ��   3.9mm/s 130mm
u8 DiningMode=0;     // ģʽѡ��  0 ������1���� 2�𱳽� 3���� 4���� 5ƽ��
typedef enum {
	DiningCmdNone = 0,		// �޶�����ֹͣ�������͸�λʱ
	DiningBeiSheng,		// ����
}DiningCmd;
typedef enum {
	DiningDirUp = 0,		// �俿��
	DiningDirDown 			// �����
}DiningDir;
typedef enum {
	GuardDirUp = 0,		// ��������
	GuardDirDown 			// �����½�
}GuardDir;

static u8 initMode=0;					// ��ʼ��ģʽ������������������ֻ�г�ʼ��ʹ��
static s16 ErrorWheelCnt_back=0; // �����г�λ�������ۼ�ֵ
static s16 ErrorWheelCnt_leg=0; // �Ȳ��г�λ�������ۼ�ֵ
static s16 prebackCnt = 0;       //ǰһ�̼���ֵ
static s16 prelegCnt = 0;
//���̶�ȡ�߳����ñ�־
//static u8 MaPanReadRun = 0;
//static char mapanread_stack[512];
//static struct rt_thread mapanread_thread;
static u8 FanShenMaPanFlag=0;

static void TuiBu(u8 work,u8 dir);
static void Dining(u8 work,u8 dir);   // 1 down����� 0 up�俿��
// �Ʊ����߻��� M5   P0.10 up; P0.11 Down;
void TuiBei(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(g_sensor.FangJia)
		work=0;
	if(work)
	{
		if(dir)
		{
			g_MT_work.huabei=1;
//			IO0SET = BIT10;
//			IO0CLR = BIT11;
		}
		else
		{
			g_MT_work.huabei=2;
	//		IO0CLR = BIT10;
	//		IO0SET= BIT11;
		}
	}
	else
	{
		g_MT_work.huabei=0;
//		IO0CLR=BIT10|BIT11;
	}
}
		// �����Ƹ˵ĵ���   1ms/pcs  �����޸�Ϊ100ms/pcs
//void adjustTable(void)
//{
//		static u8 count=0; 
//	if(g_MT_work.beibu==1)//������ۼ�
//	{
//			if(DiningMode == DiningBeiSheng )   // ������������ģʽ
//			{
//				if(DiningCount >= 10*TIME_DINING_CONFLICT_HUABEI)	   // ͣ�����������ж�
//				{
//					Dining(0,0);    // ֹͣ�˶�		
//					DiningMode = DiningCmdNone;
//					count = 0;
//				}else{
//						Dining(1,DiningDirDown);	// dining down  ��������						
//						count +=1;	
//						if(count >= 100)
//						{
//							count = 0;
//							if(DiningCount == 0xff)
//								DiningCount = 0xff;
//							else
//								DiningCount+=1;   // 100mS/pcs							
//						}							
//				}
//			}	
//	}
//	
//	if(g_MT_work.beibu>1)// ������ۼ�
//	{
//		if( DiningCount > 10*TIME_DINING_CONFLICT_HUABEI )  // ��ʾ�б����������Ҫʱ����
//		{
//				Dining(1,DiningDirUp);	// dining up  �俿��					
//				count +=1;	
//				if(count >= 100)
//				{
//					count = 0;
//					if(DiningCount == 0)
//						DiningCount = 0;
//					else
//						DiningCount-=1;   // 100mS/pcs							
//				}
//		}else{				//�鿴�����Ƿ񽵵�λ������ͻ
//			if(beibumapancount >  HUABEI_CONFLICT_POSITION )	 //��������������û�е�λ���ȴ���λ
//			{	
//				Dining(0,0);    // ֹͣ�˶�		
//				count = 0;							
//			}else{
//				Dining(1,DiningDirUp);	// dining up  �俿��					
//				count +=1;	
//				if(count >= 100)
//				{
//					count = 0;
//					if(DiningCount == 0)
//						DiningCount = 0;
//					else
//						DiningCount-=1;   // 100mS/pcs							
//				}					
//			}						
//		}
//	}
//}

// �ڲ�����е���˶������У��жϲ�����ת����Ƿ�����쳣
static u8 isDiningRotateAbnor(u8 dir)
{
	if((DiningCount > 9*TIME_RUN_DINING_MT && g_ZuobianqiSensor.rotateDiningCnt == 0)||(DiningCount < 2*TIME_RUN_DINING_MT && g_ZuobianqiSensor.rotateDiningCnt == 10*TIME_ROTATE_DINING))
	{
		return 1;
	}

	if(dir == DiningDirDown)		// ����� DiningCount +
	{
			if(DiningCount > 5*TIME_RUN_DINING_MT)		// 4*TIME_RUN_DINING_MTʱ�򣬲�����ת�������
			{
				if(g_ZuobianqiSensor.rotateDiningCnt != 0 )		// �����ڿ���λ��˵����ʼ�˶��� ��ת����˶�ʱ�� 8S < ������е��һ���ʱ�� 10S
				{
					if(g_ZuobianqiSensor.rotateDiningCnt != 10*TIME_ROTATE_DINING)   // δ���� 
					{
						if(m_rotateDiningCnt == g_ZuobianqiSensor.rotateDiningCnt)		// ˵��û��������
						{
								if(m_rotateAbnorCnt>3)			//2 ������ת���������ֹͣ�ˣ�˵���쳣������·������
								{
									m_rotateAbnorCnt = 0;
									return 1;						
								}
								m_rotateAbnorCnt +=1;
						}else{
							m_rotateAbnorCnt =0;
						}
					}
				}else{					//1 ʱ�䵽������ת�����δ����˵���쳣
					return 1;
				}	
			}
			m_rotateDiningCnt = g_ZuobianqiSensor.rotateDiningCnt;			
	}else{											//	�俿�� DiningCount -
			if(DiningCount < 8*TIME_RUN_DINING_MT)		// 9*TIME_RUN_DINING_MTʱ�򣬲�����ת�������
			{
				if(g_ZuobianqiSensor.rotateDiningCnt != 10*TIME_ROTATE_DINING )	// δ���� 	˵����ʼ�˶��� ��ת����˶�ʱ�� 8S < ������е��һ���ʱ�� 10S
				{
					if(g_ZuobianqiSensor.rotateDiningCnt != 0)   // �����ڿ���λ��
					{
						if(m_rotateDiningCnt == g_ZuobianqiSensor.rotateDiningCnt)		// ˵��û��������
						{
								if(m_rotateAbnorCnt>3)			//2 ������ת���������ֹͣ�ˣ�˵���쳣������·������
								{
									m_rotateAbnorCnt = 0;
									return 1;						
								}
								m_rotateAbnorCnt +=1;
						}else{
							m_rotateAbnorCnt =0;
						}
					}
				}else{					//1 ʱ�䵽������ת�����δ����˵���쳣
					return 1;
				}	
			}
			m_rotateDiningCnt = g_ZuobianqiSensor.rotateDiningCnt;		
	}
	
	return 0;
}

// ���ڲ�����е���汳���Ƹ��Զ�����
// 1.�ڱ���ģʽ��������ģʽ���´�����2.������ƽ��ģʽ�����α������´��� 
void adjustTable(void)       // 100mS/pcs  �����Ƹ˵���
{
	if(g_MT_work.beibu == 1)//������ۼ�
	{
			if(DiningMode == DiningBeiSheng )   // ������������ģʽ
			{
				if(DiningCount >= 10*TIME_DINING_CONFLICT_HUABEI)	   // ͣ�����������ж� ȫ�г�
				{
					Dining(0,0);    // ֹͣ�˶�		
					DiningMode = DiningCmdNone;
				}else{
						Dining(1,DiningDirDown);	// dining down  ��������	ֻ�����ڷ�ֹ����������ײ				
						DiningCount+=1;   // 100mS/pcs																			
				}
			}	
	}else if(g_MT_work.beibu > 1)// ������ۼ�
	{
		if( DiningCount > 4*TIME_RUN_DINING_MT )  // ��ʾ�б��������> 0.5�г�
		{
				Dining(1,DiningDirUp);	// dining up  �俿��					
				DiningCount-=1;   // 100mS/pcs
				if(isDiningRotateAbnor(DiningDirUp) != 0)	//ʱ�̼���쳣
				{
					Dining(0,0);    // ֹͣ�˶�		
				}
		}else{				//�鿴�����Ƿ񽵵�λ������ͻ    ��ȫ�߶Ƚ���
			if(beibumapancount >=  HUABEI_CONFLICT_POSITION )	 //��ͣ���� �жϲ���ͷ���Ƿ���ܱ�ѹ���ȴ���λ
			{			
				Dining(0,0);    // ֹͣ�˶�								
			}else{
				Dining(1,DiningDirUp);	// dining up  �俿��					
				if(DiningCount == 0)
				{	
					Dining(0,0);
				}else{
					DiningCount-=1;   // 100mS/pcs			
//  ���������е�� �� 4*TIME_RUN_DINING_MT ������ת�����û��ת�ã�˵��������	
					if(g_ZuobianqiSensor.rotateDiningCnt > 4*TIME_ROTATE_DINING)
					{
							Dining(0,0);
					}
				}
			}						
		}
	}	
	g_MT_work.diningCnt = (u8)DiningCount;
}

static void ReadMaPan(void)
{
	static u8 count=0;    
	u8	c_bei_hall,c_tui_hall;
	u8	c_zuofan_hall,c_youfan_hall;
//	 c_bei_hall = (IO1PIN&BIT19);
//	 c_tui_hall = (IO1PIN&BIT30);
	 c_bei_hall = ((IO1PIN&0X80000)>0);	 
	 c_tui_hall = ((IO1PIN&0X40000000)>0);

	 if(c_bei_hall!=m_bei_hall)//����������
	 {	 
	   if(g_MT_work.beibu==1)//������ۼ�
		 {
				beibumapancount+=1;
//				if(beibumapancount>BeiBuTongBuLowValue)	 //����������һ���߶�
//				{	
//					TuiBei(1,1);	
//				}	 			 
	   }
	   if(g_MT_work.beibu>1)// ������ۼ�
	   {
				if(beibumapancount==0)
						beibumapancount=0;
				else
						beibumapancount-=1;
	//		if(beibumapancount<BeiBuTongBuHighValue)	 //�������͵�һ���߶�
	//				TuiBei(1,0);		 											
	   }
	 }	
//	adjustTable();	   // ��������  1ms/pcs
	 if(g_sensor.BeiBuFangPing)
		 beibumapancount=0;

	 if(c_tui_hall!=m_tui_hall)//����������
	 {	 
	   if(g_MT_work.tuibu==1)//������ۼ�
				tuibumapancount+=1;
	   if(g_MT_work.tuibu>1)// ������ۼ�
	   {
			 if(initMode)
			 {
					ErrorWheelCnt_leg -=1;
			 }else{
				 if(tuibumapancount==0)
						tuibumapancount=0;
					else
						tuibumapancount-=1;			 
			 }
	   }
	 }
	 m_TuiBuStatus=c_TuiBuStatus;	 // ˢ����һ��״̬				 
//	 c_TuiBuStatus=(0==(IO1PIN&BIT20));
	 c_TuiBuStatus=((IO1PIN&0X100000)>0);

	 if(g_MT_work.tuibu==1)//������ۼ�
	 {
		 if((m_TuiBuStatus==0)&&(c_TuiBuStatus==1))//��ʾ����������
		 {
	//		 rt_kprintf("\t����: %d \r\n",tuibumapancount);
			 tuibumapancount=m_tuibu_middle;
		 }
	 }
	
	 if(g_MT_work.tuibu>1)// ������ۼ�
	 {
		 if((m_TuiBuStatus==1)&&(c_TuiBuStatus==0))//��ʾ�����½���
		 {
		//	 rt_kprintf("\t�Ƚ�: %d \r\n",tuibumapancount);
			 tuibumapancount=m_tuibu_middle;
		 }
	 }
	 	 

	 m_bei_hall=c_bei_hall;
	 m_tui_hall=c_tui_hall;

	// ��������
	 if(FanShenMaPanFlag)
	 {
		 c_youfan_hall = ((IO1PIN&0X200000)>0);   //p1.21
		 c_zuofan_hall = ((IO1PIN&0X10000000)>0);	// p1.28
		 
		 if(c_zuofan_hall!=m_zuofan_hall)//����������
		 {	 
		   if(g_MT_work.zuofanshen==1)//������ۼ�
			 zuofanmapancount+=1;
		   if(g_MT_work.zuofanshen>1)// ������ۼ�
		   {
			 if(zuofanmapancount==0)
				 zuofanmapancount=0;
			 else
				 zuofanmapancount-=1;
		   }
		 }
		 if(g_sensor.ZuoFanFangPing)
			 zuofanmapancount=0;
		 
		 if(c_youfan_hall!=m_youfan_hall)//����������
		 {	 
		   if(g_MT_work.youfanshen==1)//������ۼ�
			 youfanmapancount+=1;
		   if(g_MT_work.youfanshen>1)// ������ۼ�
		   {
			 if(youfanmapancount==0)
				 youfanmapancount=0;
			 else
				 youfanmapancount-=1;
		   }
		 }		 
		 if(g_sensor.YouFanFangPing)
			 youfanmapancount=0;

		 m_zuofan_hall=c_zuofan_hall;
		 m_youfan_hall=c_youfan_hall;

	 }
	
}



/*
// ���̶�ȡ���̣������ȼ�����
static void MaPanReadThreadEntry(void* parameter)
{
	rt_kprintf("�������̶�ȡ\r\n");
	while(1)
	{
	   ReadMaPan();
//	   rt_thread_delay(1);
	}
}


static void StartReadMaPan(void)
{
	rt_enter_critical();
	if(!MaPanReadRun)
	{
	    rt_thread_init(&mapanread_thread,
	                   "mapanread",
	                   MaPanReadThreadEntry, RT_NULL,
	                   &mapanread_stack[0], sizeof(mapanread_stack),
	                   25, 1);
	    rt_thread_startup(&mapanread_thread);
	}
	MaPanReadRun = 1;
	rt_exit_critical();
}

static void StopReadMaPan(void)
{
	rt_kprintf("ֹͣ���̶�ȡ\r\n");
	rt_enter_critical();
	if(MaPanReadRun)
	{
		rt_thread_detach(&mapanread_thread);
	}
	MaPanReadRun = 0;
	rt_exit_critical();
}
*/
void rt_hw_timer1_handler(int vector)
{
  ReadMaPan();

	/* clear interrupt flag */
	T1IR |= 0x01;

	/* acknowledge Interrupt */
	VICVectAddr = 0;
}

void rt_hw_timer1_init(void)
{
	PCONP|=(1<<2);  //timer1��Դ	
	/* prescaler = 0*/
   	T1PR = 0;
   	T1PC = 0;

	/* reset and enable MR0 interrupt */
	T1MCR = 0x3;
	T1MR0 = PCLK / 1000; // 1ms ����һ�� 
//	T1MR0 = PCLK ; // 1s ����һ�� 		
	/* enable timer 0 */
	T1TCR = 1;
	
	/* install timer handler */
	rt_hw_interrupt_install(TIMER1_INT, rt_hw_timer1_handler, RT_NULL);
	rt_hw_interrupt_umask(TIMER1_INT);

}


//---------------------����Ϊ�Ƹ˿���----------------//
// ���е�����Ǹߵ�ƽ�����͵�ƽ����
// work ��ʹ�ܣ�dir�Ƿ���1=�����0=����

// ���� M1  P0.19 up; P0.20 Down;
void BeiBu(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(g_sensor.FangJia)
		work=0;
	if(work)
	{
	//	StartReadMaPan();
		if(dir)
		{
			g_MT_work.beibu=1;
			IO0SET = BIT19;
			IO0CLR = BIT20;
		}
		else
		{
			g_MT_work.beibu=2;
			IO0CLR = BIT19;
			IO0SET= BIT20;		
		}
	}
	else
	{
		rt_enter_critical();
		g_MT_work.beibu=0;
		IO0CLR=BIT19|BIT20;
		TuiBei(0,0);
	//	if(!g_MT_work.tuibu)
	//		StopReadMaPan();	
        /* unlock scheduler */
		rt_exit_critical();	
	}
}

// �Ȳ� M2   P0.18 up; P0.24 Down;
static void TuiBu(u8 work,u8 dir)
{	
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(work)
	{
//		timer1_start();
//		StartReadMaPan();
		if(dir)
		{
			g_MT_work.tuibu=1;
			IO0SET = BIT18;
			IO0CLR = BIT24;
		}
		else
		{
			g_MT_work.tuibu=2;		
			IO0CLR = BIT18;
			IO0SET= BIT24;
		}
	}
	else
	{
		g_MT_work.tuibu=0;
		IO0CLR=BIT18|BIT24;
//		timer1_stop();			// �����ǵ������
//		if(!g_MT_work.beibu)
//			StopReadMaPan();

	}
}


// ���� M3   P0.17 up; P0.23 Down;
static void ZuoFanShen(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
/*	if(work)
	{
	//	timer1_start();
		FanShenMaPanFlag=1;
		if(dir)
		{
			g_MT_work.zuofanshen=1;
			IO0SET = BIT17;
			IO0CLR = BIT23;
//			g_sensor.ZuoFanFangPing = 0;
		}
		else
		{
			g_MT_work.zuofanshen=2;
			IO0CLR = BIT17;
			IO0SET= BIT23;
//			if(0==(IO1PIN&BIT29))  // ����
//				g_sensor.ZuoFanFangPing = 1;
		}
	}
	else
	{
		g_MT_work.zuofanshen=0;
		IO0CLR=BIT17|BIT23;
		if(0==g_MT_work.youfanshen)	// �����Ƹ�ȫ����λ��
			FanShenMaPanFlag=0;
	//	timer1_stop();			// �����ǵ������		
	}
	*/
}

// �ҷ��� M4  P0.16 up; P0.22 Down;
static void YouFanShen(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
/*	if(work)
	{
	//	timer1_start();
	FanShenMaPanFlag=1;
		if(dir)
		{
			g_MT_work.youfanshen=1;
			IO0SET = BIT16;
			IO0CLR = BIT22;
//			g_sensor.YouFanFangPing = 0;
		}
		else
		{
			g_MT_work.youfanshen=2;
			IO0CLR = BIT16;
			IO0SET= BIT22;
//			if(0==(IO1PIN&BIT22))  // ����
//				g_sensor.YouFanFangPing = 1;
		}
	}
	else
	{
		g_MT_work.youfanshen=0;
		IO0CLR=BIT16|BIT22;
		if(0==g_MT_work.zuofanshen)	// �����Ƹ�ȫ����λ��
			FanShenMaPanFlag=0;
	//	timer1_stop();			// �����ǵ������		
	}
	*/
}


// �������� M6  P0.6 up; P0.12 Down;
static void BianMenShengJiang(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(work)
	{
		if(dir)
		{
			g_MT_work.bianmenshengjiang=1;
//			IO0SET = BIT6;
	//		IO0CLR = BIT12;
			IO0CLR = BIT6;
			IO0SET= BIT12;			
		}
		else
		{
			g_MT_work.bianmenshengjiang=2;
//			IO0CLR = BIT6;
//			IO0SET= BIT12;
			IO0SET = BIT6;
			IO0CLR = BIT12;			
		}
	}
	else
	{
		g_MT_work.bianmenshengjiang=0;
		IO0CLR=BIT6|BIT12;
	}
}

// ����ƽ�� M7  P0.5 ����; P0.13 �ƻ�;
static void BianMenPingYi(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(work)
	{
		if(dir)
		{
			g_MT_work.bianmenpingyi=1;
			IO0CLR = BIT5;			// �����ת180�㰲װ����Ҫ��������ת˳��
			IO0SET= BIT13;
		}
		else
		{
			g_MT_work.bianmenpingyi=2;
			IO0SET = BIT5;
			IO0CLR = BIT13;
		}
	}
	else
	{
		g_MT_work.bianmenpingyi=0;
		IO0CLR=BIT5|BIT13;
	}
}

// ���������� M8  P0.4 up; P0.15 Down;
static void ZuoBianQiShengJiang(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(work)
	{
		if(dir)
		{
			g_MT_work.zuobianqishengjiang=1;
			IO0SET = BIT4;
			IO0CLR = BIT15;
		}
		else
		{
			g_MT_work.zuobianqishengjiang=2;
			IO0CLR = BIT4;
			IO0SET= BIT15;
		}
	}
	else
	{
		g_MT_work.zuobianqishengjiang=0;
		IO0CLR=BIT4|BIT15;
	}
}


// ���� M3   P0.17 up; P0.23 Down;
static void LeftGuard(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(work)
	{
		if(dir)
		{
			g_MT_work.leftG=1;
			IO0CLR = BIT17;
			IO0SET= BIT23;
		}
		else
		{
			g_MT_work.leftG=2;
			IO0SET = BIT17;
			IO0CLR = BIT23;		
		}
	}
	else
	{
		g_MT_work.leftG=0;
		IO0CLR=BIT17|BIT23;
	}
}

// �һ���M4
static void RightGuard(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(work)
	{
		if(dir)
		{
			g_MT_work.rightG=1;
			IO0CLR = BIT16;
			IO0SET= BIT22;
		}
		else
		{
			g_MT_work.rightG=2;
			IO0SET = BIT16;
			IO0CLR = BIT22;
		}
	}
	else
	{
		g_MT_work.rightG=0;
		IO0CLR=BIT16|BIT22;
	}
}
// ���������Ƹ�M5   P0.10 up; P0.11 Down;
void Dining(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(g_sensor.FangJia)
		work=0;
	if(work)
	{
		if(dir)  //1 ����� 
		{
			g_MT_work.dining=1;
			IO0SET = BIT10;
			IO0CLR = BIT11;		
		}
		else     //0 �俿��
		{
			g_MT_work.dining=2;		
			IO0CLR = BIT10;
			IO0SET= BIT11;	
		}
	}
	else
	{
		g_MT_work.dining=0;
		IO0CLR=BIT10|BIT11;
	}
}

// ���ڵ㶯�ſ����Զ�ֹͣ
void DoStop(void)
{
	BeiBu(0, 0);
	TuiBu(0, 0);
	ZuoFanShen(0,0);
	YouFanShen(0,0);
	TuiBei(0, 0);
	LeftGuard(0,0);
	RightGuard(0,0);
	Dining(0,0);
	DiningMode = DiningCmdNone;
}

// ���������������ֹͣ
void DoAllStop(void)
{
	BianMenShengJiang(0, 0);
	BianMenPingYi(0, 0);			
	ZuoBianQiShengJiang(0, 0);
	BeiBu(0, 0);
	TuiBu(0, 0);
	ZuoFanShen(0,0);
	YouFanShen(0,0);
	TuiBei(0, 0);	
	LeftGuard(0,0);
	RightGuard(0,0);
	Dining(0,0);
	DiningMode = DiningCmdNone;
}

// ������ʱ
void ActionTimeout(void* parameter)
{
	BeiBu(0, 0);
	TuiBu(0, 0);
	ZuoFanShen(0,0);
	YouFanShen(0,0);
	TuiBei(0, 0);
	LeftGuard(0,0);
	RightGuard(0,0);
	Dining(0,0);	
	DiningMode = DiningCmdNone;
}

// ������
BOOL DoLeftGuardUp(void)
{
	u8 i;
	rt_kprintf("LeftGuardUp\r\n");
	if(DiningCount > 5*TIME_RUN_DINING_MT)   // ���������ˣ��ͻ�����ͻ > 0.5�г�
	{
		rt_kprintf("�����Ƹ˳�ͻ\r\n");
		return TRUE;
	}
	LeftGuard(1,GuardDirUp);
	for(i=LeftGuardCount;i<(10*LeftGuardSetTime);i++)
	{
		if(WaitTimeout(20, (1<<LeftGuardUp)|(1<<STOP)))		
		{
			LeftGuardCount=i;
			LeftGuard(0,0);
			return TRUE;
		}		
	}
	LeftGuardCount = 10*LeftGuardSetTime;   
	LeftGuard(0,0);
	g_FlagCmd.Anything=0;
	rt_kprintf("LeftGuardUp  End\r\n");
	return FALSE;
}

// ������
BOOL DoLeftGuardDown(void)
{
	u8 i;
	rt_kprintf("DoLeftGuardDown\r\n");
	LeftGuard(1,GuardDirDown);
	for(i=0;i<LeftGuardCount;i++)
	{
		if(WaitTimeout(20, (1<<LeftGuardDown)|(1<<STOP)))		
		{
			LeftGuardCount-=i;
			LeftGuard(0,0);
			return TRUE;
		}		
	}
	LeftGuardCount = 0;   
	LeftGuard(0,0);
	g_FlagCmd.Anything=0;
	rt_kprintf("DoLeftGuardDown  End\r\n");
	return FALSE;
}

// �һ�����
BOOL DoRightGuardUp(void)
{
	u8 i;
	rt_kprintf("DoRightGuardUp\r\n");
	if(DiningCount > 5*TIME_RUN_DINING_MT)   // ���������ˣ��ͻ�����ͻ > 0.5�г�
	{
		rt_kprintf("�����Ƹ˳�ͻ\r\n");
		return TRUE;
	}
	RightGuard(1,GuardDirUp);
	for(i=RightGuardCount;i<(10*RightGuardSetTime);i++)
	{
		if(WaitTimeout(20, (1<<RightGuardUp)|(1<<STOP)))		
		{
			RightGuardCount=i;
			RightGuard(0,0);
			return TRUE;
		}		
	}
	RightGuardCount = 10*RightGuardSetTime;   
	RightGuard(0,0);
	g_FlagCmd.Anything=0;
	rt_kprintf("DoRightGuardUp  End\r\n");
	return FALSE;
}

// �һ�����
BOOL DoRightGuardDown(void)
{
	u8 i;
	rt_kprintf("DoRightGuardDown\r\n");
	RightGuard(1,GuardDirDown);
	for(i=0;i<RightGuardCount;i++)
	{
		if(WaitTimeout(20, (1<<RightGuardDown)|(1<<STOP)))		
		{
			RightGuardCount-=i;
			RightGuard(0,0);
			return TRUE;
		}		
	}
	RightGuardCount = 0;   
	RightGuard(0,0);
	g_FlagCmd.Anything=0;
	rt_kprintf("DoRightGuardDown  End\r\n");
	return FALSE;
}

// ���һ�����
BOOL DoGuardUp(void)
{
	u8 i,tmp;
	rt_kprintf("DoGuardUp\r\n");
	if(DiningCount > 5*TIME_RUN_DINING_MT)   // ���������ˣ��ͻ�����ͻ > 0.5�г�
	{
		rt_kprintf("�����Ƹ˳�ͻ\r\n");
		return TRUE;
	}	
	LeftGuard(1,GuardDirUp);
	RightGuard(1,GuardDirUp);
	tmp =  (LeftGuardCount<RightGuardCount)?LeftGuardCount:RightGuardCount;
	for(i=0;i<(10*RightGuardSetTime)-tmp;i++)
	{
		if(WaitTimeout(20, (1<<GuardUp)|(1<<STOP)))		
		{
			RightGuardCount += i;
			if(RightGuardCount>10*RightGuardSetTime)
				RightGuardCount = 10*RightGuardSetTime;
			LeftGuardCount += i;
			if(LeftGuardCount>10*LeftGuardSetTime)
				LeftGuardCount = 10*LeftGuardSetTime;			
			LeftGuard(0,0);			
			RightGuard(0,0);
			return TRUE;
		}		
	}
	RightGuardCount = 10*RightGuardSetTime;
	LeftGuardCount = 10*LeftGuardSetTime;	
	LeftGuard(0,0);
	RightGuard(0,0);
	g_FlagCmd.Anything=0;
	rt_kprintf("DoGuardUp  End\r\n");
	return FALSE;
}

// ���һ�����
BOOL DoGuardDown(void)
{
	u8 i,tmp;
	rt_kprintf("DoGuardDown\r\n");
	LeftGuard(1,GuardDirDown);
	RightGuard(1,GuardDirDown);
	tmp =  (LeftGuardCount>RightGuardCount)?LeftGuardCount:RightGuardCount;
	for(i=0;i<tmp;i++)
	{
		if(WaitTimeout(20, (1<<GuardDown)|(1<<STOP)))		
		{
			if(RightGuardCount < i)
				RightGuardCount = 0;
			else
				RightGuardCount -= i;
			if(LeftGuardCount < i)
				LeftGuardCount = 0;		
			else
				LeftGuardCount -= i;	
			LeftGuard(0,0);			
			RightGuard(0,0);
			return TRUE;
		}		
	}
	RightGuardCount = 0;
	LeftGuardCount = 0;	
	LeftGuard(0,0);
	RightGuard(0,0);
	g_FlagCmd.Anything=0;
	rt_kprintf("DoGuardDown  End\r\n");
	return FALSE;
}

// ��������  �俿��
// 		8.�������������������俿������������λ�ã�δ��ʼ��������������������������������ͬ���½���λ��
static rt_uint8_t DoDiningUp(void)
{
	u8 i,tmp;
	rt_kprintf("DoDiningUp start\r\n");

	rt_kprintf("back:%d,DiningCount:%d\r\n",beibumapancount,DiningCount);
	// �������δ��ƽ�������俿�������ܵ��ײ�����ƽ���˶�����
	// ���������Ѿ�����ͻ
//	if(beibumapancount < HUABEI_CONFLICT_POSITION)   // �����Ƹ˲����˶�
//	{
			Dining(1,DiningDirUp);
		  tmp = DiningCount;
			for(i=0;i<tmp;i++)
			{
				if(WaitTimeout(20, (1<<DiningUp)|(1<<STOP)))		
				{
					Dining(0,0);				
					return TRUE;
				}
				DiningCount -=1;
				
				if(isDiningRotateAbnor(DiningDirUp) != 0)	//ʱ�̼���쳣
				{
					Dining(0,0);    // ֹͣ�˶�		
				}
			}	
			rt_kprintf("Dining:%d\r\n",DiningCount);
			DiningCount = 0;
			Dining(0,0);
//	}else{     //  ����δ��ƽ�����������ڣ�
//	   rt_kprintf("Dining:%d\r\n",DiningCount);
//		 Dining(1,DiningDirUp);
//			if(DiningCount > 2*TIME_RUN_DINING_MT)	         // ������е���ڵ�λ������ͻ��������������0.2�г�
//			{
//				tmp = (DiningCount - 2*TIME_RUN_DINING_MT);
//			  for(i=0;i<tmp;i++)
//				{							
//					if(WaitTimeout(20, (1<<DiningUp)|(1<<STOP)))		
//					{
//						Dining(0,0);
//						return TRUE;
//					}	
//					DiningCount -=1;
//				}
//				rt_kprintf("Dining:%d\r\n",DiningCount);
//				Dining(0,0);
//			}else{
//				Dining(0,0);					
//			}
//	}
	g_FlagCmd.Anything=0;
	rt_kprintf("DoDiningUp  End\r\n");
	return FALSE;
}

// ��������
// 		8.�������������������俿������������λ�ã�δ��ʼ��������������������������������ͬ���½���λ��
static rt_uint8_t DoDiningDown(void)
{
	u8 i,tmp;
	rt_kprintf("DoDiningDown start\r\n");
	if(beibumapancount <= BEI_MAPAN_MAX-m_wucha)    // ��������������������Ų�ƽ��û����
	{
		rt_kprintf("need back max\r\n");
		return TRUE;	
	}
	LeftGuard(1,GuardDirDown);   //������
	RightGuard(1,GuardDirDown);
	Dining(1,DiningDirDown);
	rt_kprintf("back:%d,DiningCount:%d\r\n",beibumapancount,DiningCount);
	tmp = DiningCount;		
	for(i=tmp;i<(10*TIME_RUN_DINING_MT);i++)
	{
		if(WaitTimeout(20, (1<<DiningUp)|(1<<STOP)))		
		{
			if(RightGuardCount < i-tmp )
				RightGuardCount = 0;
			else
				RightGuardCount -= i-tmp;
			if(LeftGuardCount < i-tmp)
				LeftGuardCount = 0;		
			else
				LeftGuardCount -= i-tmp;	
			LeftGuard(0,0);			
			RightGuard(0,0);
			DiningCount =i;
			Dining(0,0);			
			return TRUE;
		}
		DiningCount +=1;   // 100ms/pcs

		if(isDiningRotateAbnor(DiningDirDown) != 0)   // �쳣����
		{
				Dining(0,0);
				return TRUE;
		}		
	
	}
	DiningCount = 10*TIME_RUN_DINING_MT;  
	Dining(0,0);
	RightGuardCount = 0;
	LeftGuardCount = 0;	
	LeftGuard(0,0);
	RightGuard(0,0);
	
	g_FlagCmd.Anything=0;
	rt_kprintf("DoRightGuardDown  End\r\n");
	return FALSE;
}

// ���Ŵ���������
BOOL DoBianmenkai(void)
{
	u16 i=0;
	rt_kprintf("open WC\r\n");
	if(!g_sensor.ZuoFanFangPing||!g_sensor.YouFanFangPing)
	{
		return TRUE;
	}
	g_sensor.BianMenGuanDaoDi=0;
	// 1.���Ž���������  ��
	if(g_sensor.BianMenPingYiDaoDi)	// ��ʾ����ƽ��δ��ʼ,���ڵײ�λ��
	{
		BianMenShengJiang(1, 0);		// ��ʱN ��,���Ž���λ
		for(i=0;i<BianmenCount;i++)
		{
			if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		// ���Źرռ���ֹͣ������
			{
				if(BianmenCount < i)
					BianmenCount = 0;
				else
					BianmenCount -= i;
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);
				return TRUE;
			}
			rt_kprintf(".");			
		}
		BianmenCount = 0;  // �����Ƹ˽����ײ�
	}
	BianMenShengJiang(0, 0);
	DbgPrintf("1.WC-Cover Down End\r\n");

	if(g_sensor.BianMenPingYiDaoDing)  // 2 �������ߵ�λ����
	{
		BianMenPingYi(0, 0);
	}
	else
	{
		BianMenPingYi(1, 1);
	}

	if(!g_sensor.BianMenPingYiDaoDing)  // 2 �������ߵ�λ����
	{		
		for(i=0;i<(10*BianMenPingYiYanShi);i++)
		{
			g_sensor.BianMenPingYiDaoDing= (0==(IO1PIN&BIT25));
			if(g_sensor.BianMenPingYiDaoDing)
				break;
			else		
			{ 
				if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		// ���Źرռ���ֹͣ������
				{
					BianMenShengJiang(0, 0);
					BianMenPingYi(0, 0);			
					ZuoBianQiShengJiang(0, 0);
					return TRUE;
				}
			}
		}
	}
	BianMenPingYi(0, 0);
	DbgPrintf("2.WC-MOVE UP end\r\n");
	if(!g_sensor.BianMenPingYiDaoDing)  //������Ż�δ������˵���������ˡ���Ҫֹͣ���ж���
	{
		BianMenShengJiang(0, 0);
		BianMenPingYi(0, 0);			
		ZuoBianQiShengJiang(0, 0);
		return TRUE;		
	}
	// ��ʱN�룬����������λ
	ZuoBianQiShengJiang(1,1);

	for(i=0;i<(10*ZuoBianQiShengJiangYanShi)-ZuobianqiCount;i++)
	{
		if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		
		{
			ZuobianqiCount += i;
			if(ZuobianqiCount>10*ZuoBianQiShengJiangYanShi)
				ZuobianqiCount = 10*ZuoBianQiShengJiangYanShi;	
			BianMenShengJiang(0, 0);
			BianMenPingYi(0, 0);			
			ZuoBianQiShengJiang(0, 0);
			return TRUE;
		}		
			rt_kprintf(".");			
	}
	ZuobianqiCount = 10*ZuoBianQiShengJiangYanShi;
	ZuoBianQiShengJiang(0, 0);
	DbgPrintf("3.WC-Toilet UP END\r\n");	

	BianMenShengJiang(0, 0);
	BianMenPingYi(0, 0);			
	ZuoBianQiShengJiang(0, 0);
	g_sensor.BianMenKaiDaoDing=1;
	g_FlagCmd.Anything=0;
	return FALSE;
}

/*
BOOL DoBianmenkai(void)
{
u16 i=0;
rt_kprintf("open WC\r\n");

	if(g_sensor.ZuoFanFangPing&&g_sensor.YouFanFangPing)
	{
		g_sensor.BianMenGuanDaoDi=0;
		if(g_sensor.BianMenPingYiDaoDi)	// ��ʾ����ƽ��δ��ʼ,���ڵײ�λ��
		{
		// ��ʱN ��,���Ž���λ
			BianMenShengJiang(1, 0);
			for(i=BianMen_Jiang_Count;i<(10*BianMenShengJiangYanShi);i++)
	//		for(i=YanShiCount;i<(10*BianMenShengJiangYanShi);i++)
			{
				if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		// ���Źرռ���ֹͣ������
				{
		//			YanShiCount = i;
					BianMen_Jiang_Count=i;
					BianMen_Sheng_Count=10*BianMenShengJiangYanShi-i;
					BianMenShengJiang(0, 0);
					BianMenPingYi(0, 0);			
					ZuoBianQiShengJiang(0, 0);
					return TRUE;
				}
			}
	//		YanShiCount = 0;	//	Ϊ��һ�����¼���	
			BianMen_Jiang_Count = 10*BianMenShengJiangYanShi;   // �����Ƹ˽����ײ�
			BianMen_Sheng_Count = 0;
		}else{
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);		
			return TRUE;
		}
		BianMenShengJiang(0, 0);
		DbgPrintf("1.WC-Cover Down End\r\n");
	
		if(g_sensor.BianMenPingYiDaoDing)  // 2 �������ߵ�λ����
		{
			BianMenPingYi(0, 0);
		}
		else
		{
			BianMenPingYi(1, 1);
		}

		if(!g_sensor.BianMenPingYiDaoDing)  // 2 �������ߵ�λ����
		{		
			for(i=0;i<(10*BianMenPingYiYanShi);i++)
			{
				if(g_sensor.BianMenPingYiDaoDing)
					break;
				else		
				{ 
				  if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		// ���Źرռ���ֹͣ������
				  {
					BianMenShengJiang(0, 0);
					BianMenPingYi(0, 0);			
					ZuoBianQiShengJiang(0, 0);
					return TRUE;
				  }
				}
			}
		}
		BianMenPingYi(0, 0);
		DbgPrintf("2.WC-MOVE UP end\r\n");
		if(!g_sensor.BianMenPingYiDaoDing)  //������Ż�δ������˵���������ˡ���Ҫֹͣ���ж���
		{
			BianMenShengJiang(0, 0);
			BianMenPingYi(0, 0);			
			ZuoBianQiShengJiang(0, 0);
			return TRUE;		
		}
		// ��ʱN�룬����������λ
		ZuoBianQiShengJiang(1,1);

		for(i=0;i<(10*ZuoBianQiShengJiangYanShi)-ZuobianqiCount;i++)
		{
			if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		
			{
				ZuobianqiCount += i;
				if(ZuobianqiCount>10*ZuoBianQiShengJiangYanShi)
					ZuobianqiCount = 10*ZuoBianQiShengJiangYanShi;	
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);
				return TRUE;
			}		
		}
		ZuobianqiCount = 10*ZuoBianQiShengJiangYanShi;
		for(i=ZuoBianQi_Sheng_Count;i<(10*ZuoBianQiShengJiangYanShi);i++)
//		for(i=YanShiCount;i<(10*ZuoBianQiShengJiangYanShi);i++)
		{
			if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		// ���Źرռ���ֹͣ������
			{
		//		YanShiCount = i;
				ZuoBianQi_Sheng_Count=i;
				ZuoBianQi_Jiang_Count=10*ZuoBianQiShengJiangYanShi-i;
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);
				return TRUE;
			}
		}

	//	YanShiCount = 0;	//	Ϊ��һ�����¼���	
		ZuoBianQi_Sheng_Count=10*ZuoBianQiShengJiangYanShi;		// �����������Ƹ�������
		ZuoBianQi_Jiang_Count=0;		
		ZuoBianQiShengJiang(0, 0);
		DbgPrintf("3.WC-Toilet UP END\r\n");	

		BianMenShengJiang(0, 0);
		BianMenPingYi(0, 0);			
		ZuoBianQiShengJiang(0, 0);
		g_sensor.BianMenKaiDaoDing=1;
		g_FlagCmd.Anything=0;
	}
	return FALSE;
}
*/

// ���Źر���������
static void DoBianmenguan(void)
{
	u16 i=0;
	rt_kprintf("WC CLOSE\r\n");
	g_sensor.BianMenKaiDaoDing=0;		
	if(g_sensor.BianMenPingYiDaoDing)  //   ����ƽ�ƻ������ڶ���λ�ã�δ����
	{	
		// ��ʱN�룬�������������Ƹ��½���λ
		ZuoBianQiShengJiang(1,0);
		
		for(i=0;i<ZuobianqiCount;i++)
		{
			if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP)))		
			{
				if(ZuobianqiCount < i)
					ZuobianqiCount = 0;
				else
					ZuobianqiCount -= i;
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);
				return;
			}	
			rt_kprintf(".");			
		}
		ZuobianqiCount = 0;// �����������Ƹ˽���λ
	}
	ZuoBianQiShengJiang(0, 0);
	DbgPrintf("3.WC-Toilet Down End\r\n");

	if(g_sensor.BianMenPingYiDaoDi)  // 2 �����ƻص��ײ�
	{
		BianMenPingYi(0, 0);
	}
	else
	{
		BianMenPingYi(1, 0);
	}

	if(!g_sensor.BianMenPingYiDaoDi)  // 2 �����ƻص��ײ�
	{		
		for(i=0;i<(10*BianMenPingYiYanShi);i++)
		{
			if(g_sensor.BianMenPingYiDaoDi)
				break;
			else
			{
			  if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP))) 	// ���Źرռ���ֹͣ������
			  {
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);
				return;
			  }
			}
		}
	}
	BianMenPingYi(0, 0);
	DbgPrintf("2.WC-Move Down END\r\n");
	if(!g_sensor.BianMenPingYiDaoDi)	//������Ż�δ�����ײ�˵���������ˡ���Ҫֹͣ���ж���
	{
		BianMenShengJiang(0, 0);
		BianMenPingYi(0, 0);			
		ZuoBianQiShengJiang(0, 0);
		return ;		
	}

	// ��ʱN ��,��������λ
		BianMenShengJiang(1, 1);
	for(i=0;i<(10*BianMenShengJiangYanShi)-BianmenCount;i++)
	{
		if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP)))		
		{
			BianmenCount += i;
			if(BianmenCount>10*BianMenShengJiangYanShi)
				BianmenCount = 10*BianMenShengJiangYanShi;	
			BianMenShengJiang(0, 0);
			BianMenPingYi(0, 0);			
			ZuoBianQiShengJiang(0, 0);
			return;
		}		
			rt_kprintf(".");			
	}	
	BianmenCount = 10*BianMenShengJiangYanShi;	
	BianMenShengJiang(0, 0);
	DbgPrintf("1.WC-Cover Up End\r\n");

	BianMenShengJiang(0, 0);
	BianMenPingYi(0, 0);			
	ZuoBianQiShengJiang(0, 0);
	g_sensor.BianMenGuanDaoDi=1;
	g_FlagCmd.Anything=0;
	return;
}

/*
static void DoBianmenguan(void)
{
	u16 i=0;
	rt_kprintf("WC CLOSE\r\n");
	g_sensor.BianMenKaiDaoDing=0;		
	if(g_sensor.BianMenPingYiDaoDing)  //   ����ƽ�ƻ������ڶ���λ�ã�δ����
	{	
		// ��ʱN�룬�������������Ƹ��½���λ
		ZuoBianQiShengJiang(1,0);
		
		for(i=0;i<ZuobianqiCount;i++)
		{
			if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP)))		
			{
				if(ZuobianqiCount < i)
					ZuobianqiCount = 0;
				else
					ZuobianqiCount -= i;
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);
				return;
			}		
		}
		ZuobianqiCount = 0;
		
/*		for(i=ZuoBianQi_Jiang_Count;i<(10*ZuoBianQiShengJiangYanShi);i++)
//		for(i=YanShiCount;i<(10*ZuoBianQiShengJiangYanShi);i++)
		{
			if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP))) 	// ���Źرռ���ֹͣ������
			{
			//	YanShiCount = i;
				ZuoBianQi_Jiang_Count=i;
				ZuoBianQi_Sheng_Count=10*ZuoBianQiShengJiangYanShi-i;				
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);
				return;
			}
		}
		
	//	YanShiCount = 0;	//	Ϊ��һ�����¼���	
		ZuoBianQi_Jiang_Count=10*ZuoBianQiShengJiangYanShi;			// �����������Ƹ˽���λ
		ZuoBianQi_Sheng_Count=0;
	}else{
		BianMenShengJiang(0, 0);
		BianMenPingYi(0, 0);			
		ZuoBianQiShengJiang(0, 0);
		return;
	}
	ZuoBianQiShengJiang(0, 0);
	DbgPrintf("3.WC-Toilet Down End\r\n");

	if(g_sensor.BianMenPingYiDaoDi)  // 2 �����ƻص��ײ�
	{
		BianMenPingYi(0, 0);
	}
	else
	{
		BianMenPingYi(1, 0);
	}

	if(!g_sensor.BianMenPingYiDaoDi)  // 2 �����ƻص��ײ�
	{		
		for(i=0;i<(10*BianMenPingYiYanShi);i++)
		{
			if(g_sensor.BianMenPingYiDaoDi)
				break;
			else
			{
			  if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP))) 	// ���Źرռ���ֹͣ������
			  {
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);
				return;
			  }
			}
		}
	}
	BianMenPingYi(0, 0);
	DbgPrintf("2.WC-Move Down END\r\n");
	if(!g_sensor.BianMenPingYiDaoDi)	//������Ż�δ�����ײ�˵���������ˡ���Ҫֹͣ���ж���
	{
		BianMenShengJiang(0, 0);
		BianMenPingYi(0, 0);			
		ZuoBianQiShengJiang(0, 0);
		return ;		
	}

	// ��ʱN ��,��������λ
		BianMenShengJiang(1, 1);
		for(i=BianMen_Sheng_Count;i<(10*BianMenShengJiangYanShi);i++)
//		for(i=YanShiCount;i<(10*BianMenShengJiangYanShi);i++)
		{
			if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP))) 	// ���Źرռ���ֹͣ������
			{
//				YanShiCount = i;
				BianMen_Sheng_Count=i;
				BianMen_Jiang_Count=10*BianMenShengJiangYanShi-i;
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);
				return;
			}
		}
//		YanShiCount = 0;	//	Ϊ��һ�����¼���		
		BianMen_Sheng_Count=10*BianMenShengJiangYanShi;		// ���������Ƹ�������
		BianMen_Jiang_Count=0;

	BianMenShengJiang(0, 0);
	DbgPrintf("1.WC-Cover Up End\r\n");

	BianMenShengJiang(0, 0);
	BianMenPingYi(0, 0);			
	ZuoBianQiShengJiang(0, 0);
	g_sensor.BianMenGuanDaoDi=1;
	g_FlagCmd.Anything=0;
		
}
*/
BOOL AllowFuYuan(void)
{
	u16 i;
	for(i=0;i<AutoJieBianYanShi;i++)
	{
		if(WaitTimeout(RT_TICK_PER_SECOND, (1<<FuYuan)|(1<<STOP)))		// ���Źرռ���ֹͣ������
		{
			return FALSE;
		}
		if(!g_ZuobianqiSensor.FunctionWorkAuto)		// �Զ�������
			return TRUE;
	}
	return FALSE;
}




// ��ƽ����
BOOL DoFangPing(void)
{
	u16 i;
	u8 tmp;
    rt_kprintf("ƽ�� \r\n");
	if(!g_sensor.TuiBuFangPing)
	{
		rt_kprintf("�Ȳ����� \r\n");
		if(tuibumapancount>(m_tuibu_middle+m_wucha)) // ��ʾΪ������λ״̬
			TuiBu(1,0);
		if(tuibumapancount<(m_tuibu_middle-m_wucha)) // ��ʾ����״̬
			TuiBu(1,1);
	}
	if(!g_sensor.BeiBuFangPing)
	{
		rt_kprintf("�������� \r\n");
		BeiBu(1,0);
//		TuiBei(1,0);
	}
	LeftGuard(1,GuardDirUp);    // ������
	RightGuard(1,GuardDirUp);
	Dining(1,DiningDirUp);  // �俿��
/*	for(i=0;((i<(10*BeiBuYanShi))||((g_sensor.BeiBuFangPing==1)||(g_sensor.TuiBuFangPing==1)));i++)
	{
		if(WaitTimeout(20, (1<<STOP))) 	//ֹͣ������
		{
			DoStop();
			return TRUE;
		}
	}	*/
	for(i=0;i<(10*BeiBuYanShi);i++)
	{
		if(g_sensor.BeiBuFangPing)
		{	rt_thread_delay(2);
			BeiBu(0,0);
			TuiBei(0,0);}
		if(g_sensor.TuiBuFangPing)
		{
			if(g_MT_work.tuibu ==1)
				rt_thread_delay(100);
			TuiBu(0,0);
		}
		if(g_sensor.FangJia)
		{	DoStop();
			return TRUE;}			
		if(g_sensor.BeiBuFangPing&&g_sensor.TuiBuFangPing) // �����Ȳ�ͬʱ��ƽ
			break;
		else
		{
			if(WaitTimeout(20, (1<<PingTang)|(1<<STOP))) 	//ֹͣ������
			{
				DoStop();				
				RightGuardCount += i;
				if(RightGuardCount>10*RightGuardSetTime)
					RightGuardCount = 10*RightGuardSetTime;
				LeftGuardCount += i;
				if(LeftGuardCount>10*LeftGuardSetTime)
					LeftGuardCount = 10*LeftGuardSetTime;	
				LeftGuard(0,0);			
				RightGuard(0,0);								
				return TRUE;
			}
		}
	}
	RightGuardCount = 10*RightGuardSetTime;
	LeftGuardCount = 10*LeftGuardSetTime;	
	LeftGuard(0,0);
	RightGuard(0,0);	
	if(DiningCount != 0)		// �����Ƹ˻�δ����
	{
		if(g_ZuobianqiSensor.rotateDiningCnt > 0)		// ˵���쳣�����ܱ俿��
		{
			return TRUE;
		}
		rt_kprintf("�����俿��δ����\r\n");
		Dining(1,DiningDirUp);  //�俿��
		tmp = DiningCount;
		for(i=0;i<tmp;i++)
		{
			if(WaitTimeout(20, (1<<PingTang)|(1<<STOP)))
			{
				Dining(0,0);
				return TRUE;			
			}
			DiningCount -=1;
		}
		Dining(0,0);
		DiningCount = 0;
		rt_kprintf("�俿������\r\n");		
	}
	DoStop();
	if(!g_sensor.TuiBuFangPing)
		tuibumapancount = m_tuibu_max;
	else
		rt_kprintf("�����Ȳ��ѷ�ƽ \r\n");

	
	if(g_sensor.BeiBuFangPing&&g_sensor.TuiBuFangPing)
	{
		if(!g_sensor.ZuoFanFangPing)
		{
			ZuoFanShen(1,0);
		}
		if(!g_sensor.YouFanFangPing)
			YouFanShen(1,0);
	}
	for(i=0;i<(10*FanShenYanShi);i++)
	{
		if(g_sensor.YouFanFangPing)
			{rt_thread_delay(2);YouFanShen(0,0);}
		if(g_sensor.ZuoFanFangPing)
			{rt_thread_delay(2);ZuoFanShen(0,0);}
		if(g_sensor.YouFanFangPing&&g_sensor.ZuoFanFangPing) // �����Ȳ�ͬʱ��ƽ
			break;
		else
		{if(WaitTimeout(20, (1<<PingTang)|(1<<STOP))) 	//ֹͣ������
		{	DoStop();
			return TRUE;
		}}
	}	
	DoStop();
	g_FlagCmd.Anything=0;
	return FALSE;
}

// ����ģʽ
// ���һ����½��������Ƹ˱����
static void DoZuoYi(void)
{
	u16 i;
	u8 tmp;
    rt_kprintf("����\r\n");
	g_sensor.FunctionWorkZuoYi=1;
	if(g_sensor.ZuoFanFangPing && g_sensor.YouFanFangPing)
	{
		
		BeiBu(1, 1);
//		TuiBei(1,1);		
		TuiBu(1, 0);
	
		LeftGuard(1,GuardDirDown);   // ������
		RightGuard(1,GuardDirDown);
		rt_kprintf("����:%d,�Ȳ�:%d",beibumapancount,tuibumapancount);
		
		for(i=0;i<(10*BeiBuYanShi);i++)
		{
			if(beibumapancount >= 3000)	
			{
					Dining(1,DiningDirDown);			// �����
			}
			if((beibumapancount>=m_beibu_max))
			{	BeiBu(0,0);TuiBei(0,0);}
			if((tuibumapancount<=m_wucha))
			{	 
				rt_thread_delay(20);
				TuiBu(0,0);		//	��ʼ�����һ������������
			}
			if((beibumapancount>=m_beibu_max - m_wucha)&&(tuibumapancount<=m_wucha)) // �����Ȳ��ൽλ���˳�
			{	
				rt_thread_delay(20);
				break;
			}else{
				if(WaitTimeout(20, (1<<QiZuo)|(1<<STOP))) 	//ֹͣ������
				{
					DoStop();
					g_sensor.FunctionWorkZuoYi=0;		
					
					if(RightGuardCount < i )
						RightGuardCount = 0;
					else
						RightGuardCount -= i;
					if(LeftGuardCount < i)
						LeftGuardCount = 0;		
					else
						LeftGuardCount -= i;	
					LeftGuard(0,0);			
					RightGuard(0,0);	
					Dining(0,0);					
					return;
				}
				if(DiningCount >= 10*TIME_RUN_DINING_MT)  // �Ƿ��ѱ����
				{
					Dining(0,0);
				}else{
					if(g_MT_work.dining== 1)    // δ���������ۼӣ�
						DiningCount +=1;  //100ms/pcs						
				}
				// �����⵽������ת��������ˣ�������һ�벻������Ҫ����ֹͣ������е���˶�������������
				if(isDiningRotateAbnor(DiningDirDown) != 0)
				{
						Dining(0,0);
				}			
			}
		}
		rt_kprintf("����:%d,�Ȳ�:%d",beibumapancount,tuibumapancount);
		beibumapancount = m_beibu_max;
		tuibumapancount = 0;
		LeftGuardCount = 0;
		RightGuardCount=0;
		LeftGuard(0,0);			
		RightGuard(0,0);
		Dining(0,0);		
	}

	DoStop();
	if(LeftGuardCount != 0 || RightGuardCount != 0)   // ����δ��ƽ�������ţ�Ϊ��ֹ������Ƹ˳�ͻ����������
	{
		tmp =  (LeftGuardCount<RightGuardCount)?LeftGuardCount:RightGuardCount;	
		rt_kprintf("����δ��λ\r\n");
		rt_kprintf("left:%d	right:%d\r\n",LeftGuardCount,RightGuardCount);
		LeftGuard(1,GuardDirDown);   // ������
		RightGuard(1,GuardDirDown);	
		for(i=0;i<tmp;i++)
		{
			if(WaitTimeout(20, (1<<QiZuo)|(1<<STOP)))
			{
				DoStop();
			  g_sensor.FunctionWorkZuoYi=0;	
					if(RightGuardCount < i )
						RightGuardCount = 0;
					else
						RightGuardCount -= i;
					if(LeftGuardCount < i)
						LeftGuardCount = 0;		
					else
						LeftGuardCount -= i;	
					LeftGuard(0,0);			
					RightGuard(0,0);						
				return;			
			}
		}
		RightGuardCount = 0;
		LeftGuardCount = 0;	
		LeftGuard(0,0);
		RightGuard(0,0);		
	}
	
	if(DiningCount != 10*TIME_RUN_DINING_MT)		// ���α����ʱ��� ��е��δ��λ
	{
		rt_kprintf("�����δ��λ\r\n");
		Dining(1,DiningDirDown);  //�����
		for(i=DiningCount;i<10*TIME_RUN_DINING_MT;i++)
		{
			if(WaitTimeout(20, (1<<QiZuo)|(1<<STOP)))
			{
				DoStop();
			  g_sensor.FunctionWorkZuoYi=0;	
				DiningCount = i;
				return;			
			}
			DiningCount+=1;
			// �����⵽������ת��������ˣ�������һ�벻������Ҫ����ֹͣ������е���˶�������������
			if(isDiningRotateAbnor(DiningDirDown) != 0)
			{
					Dining(0,0);
					g_sensor.FunctionWorkZuoYi=0;	
					return;
			}
		}
		Dining(0,0);
		DiningCount = 10*TIME_RUN_DINING_MT;
		rt_kprintf("�������λ\r\n");		
	}
	RightGuardCount = 0;
	LeftGuardCount = 0;	
	LeftGuard(0,0);
	RightGuard(0,0);
	Dining(0,0);
	DiningCount = 10*TIME_RUN_DINING_MT;
	
	g_sensor.FunctionWorkZuoYi=0;	
	g_FlagCmd.Anything=0;

}

// ������λ��������ģʽ
// ����ģʽ�£����һ����Ͳ�����е����ô���
// ����ģʽ�£��ǳԷ������ԣ�������е�۲��˶�����ת����̨�治�˶���ע�����һ����Ƿ��������е�ۻ���ײ����ƽ�ɵ�����δ��ײ���Ͳ��ÿ���
// ���ε����Σ�������е���յ�������ͻλ��
static void DoTangYi(void)
{
	u16 i;
    rt_kprintf("������λ  ����\r\n");
	g_sensor.FunctionWorkTangYi=1;
	if(g_sensor.ZuoFanFangPing && g_sensor.YouFanFangPing)
	{
		if(beibumapancount>(m_beibu_middle+m_wucha))	// ����Ϊ�м�̬
		{   
			BeiBu(1, 0);
		//	TuiBei(1,0);
		}
		else
		{
			BeiBu(1, 1);
		//	TuiBei(1,1);
		}
		if(tuibumapancount<m_tuibu_max)	// �Ȳ�Ϊ���
			TuiBu(1, 1);
		// ��̧�ȵ�ʱ����������ڣ��Ȳ�ѹ����������Ҫ�Ѳ����յ�
		if(DiningCount > 0)	         // ������е���ڵ�λ������ͻ��������������0.2�г�
		{
				Dining(1,DiningDirUp);  //�俿��	
		}			
	
	 	for(i=0;(i<(10*TuiBuYanShi));i++)
	 	{
			if((beibumapancount<=(m_beibu_middle+m_wucha))&&(beibumapancount>=(m_beibu_middle-m_wucha)))
			 {	BeiBu(0,0);TuiBei(0,0);}
			if(tuibumapancount>=m_tuibu_max )
			{	TuiBu(0,0);}
			if((beibumapancount<=(m_beibu_middle+m_wucha))&&(beibumapancount>=(m_beibu_middle-m_wucha))&&(tuibumapancount>=m_tuibu_max - m_wucha)) // �����Ȳ��ൽλ���˳�
			{
				rt_thread_delay(20);
				break;
			}else{
				if(WaitTimeout(20, (1<<XinZangTangWei)|(1<<STOP))) 	//ֹͣ������
				{
					DoStop();
					Dining(0,0);
					g_sensor.FunctionWorkTangYi=0;
					return;
				}
			}
			if(DiningCount > 5*TIME_RUN_DINING_MT)
			{
					if(g_ZuobianqiSensor.rotateDiningCnt > 5*TIME_ROTATE_DINING)		// ��е���߹�һ�룬��δ��ʼ������ת��
					{
						Dining(0,0);			
					}
			}
			if(g_MT_work.dining == 2)
			{
				DiningCount -= 1;
				if(isDiningRotateAbnor(DiningDirUp) != 0)  //�쳣����
				{
						Dining(0,0);
				}
			}
	 	}	
		tuibumapancount= m_tuibu_max;			
	}		
	DoStop();
	
	if(DiningCount > 0)
	{
			Dining(1,DiningDirUp);  //�俿��	
			for(i=0;i < DiningCount;i++)
			{							
				if(WaitTimeout(20, (1<<DiningUp)|(1<<STOP)))		
				{
					Dining(0,0);
				}	
				DiningCount -=1;
				if(isDiningRotateAbnor(DiningDirUp) != 0)  //�쳣����
				{
						Dining(0,0);
				}				
			}
	}
	Dining(0,0);
	
	g_sensor.FunctionWorkTangYi=0;	
	g_FlagCmd.Anything=0;
}



// ���Ź���ʼ��
// 2880000 ,1��
void WDInit(void)
{
//	WDTC = 0x00DBBA00;	// ���ÿ��Ź���ʱ���Ĺ̶�װ��ֵ : 5 ��
//	WDTC = 0x04F1A000;	// ���ÿ��Ź���ʱ���Ĺ̶�װ��ֵ : 30 ��
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

// ��ʼ��������
void ActionStartCmd(ActionCmd cmd)
{
	u8 c_TuiCmd=0;
#if 0
	if(cmd != ActionCmdNone)
	{
		rt_kprintf("ActionStartCmd= %d \r\n",cmd);
	}
#endif
	switch(cmd)
	{
	case BeiSheng	:
	//	DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.ZuoFanFangPing&&g_sensor.YouFanFangPing&&!g_sensor.FangJia)
#endif
		{
			BeiBu(1, 1);
		//	TuiBei(1,1);
		}
		DiningMode = DiningBeiSheng;
		flagRelaseKey=1;
		RestartTimer();
		break;
	case BeiJiang:
	//	DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.ZuoFanFangPing&&g_sensor.YouFanFangPing&&!g_sensor.FangJia)
#endif
		{
			BeiBu(1,0);
		//	TuiBei(1,0);
		}	
		
		flagRelaseKey=1;
		RestartTimer();
		break;

	case TuiSheng	:
		DoStop();		
#if !DONG_ZUO_DEBUG
		if(g_sensor.ZuoFanFangPing&&g_sensor.YouFanFangPing)
#endif
		{
//			if(g_sensor.TuiBuFangPing&&(m_TuiCmd>60))	// ���������ҷ�ƽ��λ
			if(g_sensor.TuiBuFangPing)
			{			
				if(m_bottonrelease)		// ����δ�ͷ�
					{if(!flag_relase)
						TuiBu(0, 0);
					 else 
					 	TuiBu(1, 1); }
				else		// �����ͷŹ�
					{TuiBu(1, 1);flag_relase=1;}
			}
			else				// ���� ���ڷ�ƽλ�ã��������ڷ�ƽλ����
			{	TuiBu(1, 1);flag_relase=0;
			}
		}
		m_bottonrelease=1;
		m_TuiCmd+=1;
		if(m_TuiCmd>250)
			m_TuiCmd=250;
		flagRelaseKey=1;
		RestartTimer();
		break;

	case TuiJiang	:
		DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.ZuoFanFangPing&&g_sensor.YouFanFangPing)
#endif
		{
	//			if(g_sensor.TuiBuFangPing&&(m_TuiCmd>60))	// ���������ҷ�ƽ��λ
	//				TuiBu(0, 0);
	//			else
	//				TuiBu(1, 0);
	//		}
			if(g_sensor.TuiBuFangPing)
			{			
				if(m_bottonrelease) 	// ����δ�ͷ�
					{if(!flag_relase)
						TuiBu(0, 0);
					 else 
						TuiBu(1, 0); }
				else		// �����ͷŹ�
					{TuiBu(1, 0);flag_relase=1;}
			}
			else				// ���� ���ڷ�ƽλ�ã��������ڷ�ƽλ����
			{	TuiBu(1, 0);flag_relase=0;
			}
		}
		m_bottonrelease=1;
		m_TuiCmd+=1;
		if(m_TuiCmd>250)
			m_TuiCmd=250;	
		flagRelaseKey=1;
		RestartTimer();
		break;

	case ZuoFanSheng:
		DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.BeiBuFangPing&&g_sensor.TuiBuFangPing&&g_sensor.BianMenPingYiDaoDi)
#endif
		{
			if(g_sensor.YouFanFangPing)   // ����ʱ�������ҷ���
				ZuoFanShen(1,1);
		}
		flagRelaseKey=1;
		RestartTimer();
		break;
	case ZuoFanJiang:
		DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.BeiBuFangPing&&g_sensor.TuiBuFangPing&&g_sensor.BianMenPingYiDaoDi)
#endif
		{
			ZuoFanShen(1,0);
		}	
		flagRelaseKey=1;
		RestartTimer();
		break;

	case YouFanSheng:
		DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.BeiBuFangPing&&g_sensor.TuiBuFangPing&&g_sensor.BianMenPingYiDaoDi)
#endif
		{
			if(g_sensor.ZuoFanFangPing)    // �ҷ���ʱ����������
				YouFanShen(1,1);
		}
		flagRelaseKey=1;
		RestartTimer();
		break;
	case YouFanJiang:
		DoStop();
#if !DONG_ZUO_DEBUG
		if(g_sensor.BeiBuFangPing&&g_sensor.TuiBuFangPing&&g_sensor.BianMenPingYiDaoDi)
#endif
		{
			YouFanShen(1,0);
		}	
		flagRelaseKey=1;
		RestartTimer();
		break;
		
	case ActionCmdNone:
		m_bottonrelease=0;
		if(flagRelaseKey)
		{
			ActionTimeout(NULL);
			flagRelaseKey=0;
		}	
		flag_qidong=0;		
		m_TuiCmd=0;
		if(g_sensor.DianJiGuoLiuBao)		// �����������������ֹͣ����
			 ActionStopCmd(STOP);
//		DoStop();		
//		ActionTimeout(NULL);
//		RestartTimer();
		break;
	default:
		DbgPrintf("������Ϣ����������\r\n");
		rt_mb_send(&m_mb, cmd);
		break;
//		case ActionCmdNone:
//			m_FanShenEnable = 1;

//		DoStop();			
//			ActionTimeout(NULL);
//			RestartTimer();
//			ActionStopCmd(ActionCmdNone);
//			break;
	}
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


static void ActionStatusInit(void)
{
	g_sensor.TuiBuFangPing=((IO1PIN&BIT20)>0);	 // �����Ȳ�ˮƽλΪ������λ
	if(g_sensor.TuiBuFangPing)
		tuibumapancount = m_tuibu_middle;
	BeiBu(1,0);
	ZuoFanShen(1,0);
	YouFanShen(1,0);
	rt_thread_delay(RT_TICK_PER_SECOND*25/1000);	
	g_sensor.BeiBuFangPing=(0==(IO1PIN&BIT27));
	g_sensor.ZuoFanFangPing=(0==(IO1PIN&BIT29));
	g_sensor.YouFanFangPing=(0==(IO1PIN&BIT22));
	BeiBu(0,0);
	ZuoFanShen(0,0);
	YouFanShen(0,0);	
	g_sensor.BianMenPingYiDaoDi= (0==(IO1PIN&BIT26));
	g_sensor.BianMenPingYiDaoDing= (0==(IO1PIN&BIT25));

	g_sensor.BianMenGuanDaoDi=g_sensor.BianMenPingYiDaoDi;
	g_sensor.BianMenKaiDaoDing=g_sensor.BianMenPingYiDaoDing;

//	BianMen_Sheng_Count=0;
//	BianMen_Jiang_Count=0;
//	ZuoBianQi_Sheng_Count=0;
//	ZuoBianQi_Jiang_Count=0;

	if(g_sensor.BianMenPingYiDaoDing) // ������������
	{
		ZuobianqiCount = 10*ZuoBianQiShengJiangYanShi;
		BianmenCount = 0;
	}
	if(g_sensor.BianMenPingYiDaoDi)    //  ������Ź�
	{
		BianmenCount = 10*BianMenShengJiangYanShi;
		ZuobianqiCount = 0;	
	}

	DoAllStop();

	// �����Ȳ���ʼ��
	TuiBu(1,1);		// ���� ���� �ٻ�ԭλ����ȡ��Ϣ
	rt_thread_delay(RT_TICK_PER_SECOND/2);	// 500mS
	TuiBu(0,0);
	rt_thread_delay(RT_TICK_PER_SECOND*25/1000);	// 25ms
	TuiBu(1,0);
	rt_thread_delay(RT_TICK_PER_SECOND/2);	// 500mS
	TuiBu(0,0);

}

static u8 initBianMen(u16 i)
{
	static u16 tickBianMen=0;
	static u8 BianMenStatus=0;
	rt_kprintf("tick=%d,status=%d\r\n",i,BianMenStatus);
	g_sensor.BianMenPingYiDaoDi= (0==(IO1PIN&BIT26));
	g_sensor.BianMenPingYiDaoDing= (0==(IO1PIN&BIT25));
	switch(BianMenStatus)
	{
		case 0: 
					if(g_sensor.BianMenPingYiDaoDing)  //   ����ƽ�ƻ������ڶ���λ�ã�δ����,�����ſ���
					{	
						//1 �������������Ƹ��½���λ
						ZuoBianQiShengJiang(1,0);
						BianMenStatus = 1;
						tickBianMen = i;
					}else{
						 BianMenStatus = 2;			// ˵���������ڵײ�������ƽ�ƻ������м�λ��
					}
					break;
		case 1:
					if(i-tickBianMen > 10*ZuoBianQiShengJiangYanShi)
					{
						ZuoBianQiShengJiang(0, 0);
						ZuobianqiCount = 0;// �����������Ƹ˽���λ
						BianMenStatus = 2;
					}	
					break;
		case 2:
					if(g_sensor.BianMenPingYiDaoDi)  // 2 �����ƻص��ײ�
					{
						BianMenPingYi(0, 0);							
						BianMenStatus = 4;
					}
					else
					{
						BianMenPingYi(1, 0);
						BianMenStatus = 3;
					}		
					break;
		case 3:
					if(g_sensor.BianMenPingYiDaoDi)
					{
						BianMenPingYi(0, 0);	
						BianMenStatus = 4;
					}					
					break;				
		case 4:
					if(g_sensor.BianMenPingYiDaoDi)
					{
						BianMenShengJiang(1, 1);				// ��������
						BianMenStatus = 5;
						tickBianMen = i;
					}
					break;
		case 5:
					rt_kprintf("tickBianMen =%d",tickBianMen);
					if(i - tickBianMen > 10*BianMenShengJiangYanShi)
					{
						BianMenShengJiang(0, 0);
						BianmenCount = 10*BianMenShengJiangYanShi;	
						g_sensor.BianMenGuanDaoDi=1;			
						BianMenStatus = 6;											// ����
					}
					break;
		default:
					ZuoBianQiShengJiang(0, 0);
					BianMenPingYi(0, 0);	
					BianMenShengJiang(0, 0);
					break;
	}				
	return BianMenStatus;			
}

// ��ҪΪǿ�Ƴ�ʼ��ƽ��ģʽ�����е��λ�ù��㣬���ڽ�������Ƹ�λ�ó�ͻ
// ��ʼ�����̣�
// 1 ���� �Ȳ� ��ƽ��2 �����Ƹ˱俿��   3 ���һ���������  4 ���Ź�
// ������ʱ 30s  �Ȳ���ʱ30S(��������˷�ƽ����60S)  �����Ƹ���ʱ13s   ���һ�����ʱ 15s  ���Ź���3�����������15+18+6 <= 39S  
void initAllPosition(void)
{
	u16 i;
	rt_kprintf("��ʼ��ʼ��\r\n");		

	ErrorWheelCnt_back = 0;
	ErrorWheelCnt_leg = 0;
	
	
//	g_sensor.BianMenPingYiDaoDi= (0==(IO1PIN&BIT26));
//	g_sensor.BianMenPingYiDaoDing= (0==(IO1PIN&BIT25));

//	g_sensor.BianMenGuanDaoDi=g_sensor.BianMenPingYiDaoDi;
//	g_sensor.BianMenKaiDaoDing=g_sensor.BianMenPingYiDaoDing;

//	if(g_sensor.BianMenPingYiDaoDing) // ������������
//	{
//		ZuobianqiCount = 10*ZuoBianQiShengJiangYanShi;
//		BianmenCount = 0;
//	}
//	if(g_sensor.BianMenPingYiDaoDi)    //  ������Ź�
//	{
//		BianmenCount = 10*BianMenShengJiangYanShi;
//		ZuobianqiCount = 0;	
//	}
	
//	BeiBu(1,0);       
//	g_sensor.BeiBuFangPing=(0==(IO1PIN&BIT27));   // ֻ��ͨ����ܶ�ȡ	

	// �Ȳ���ʼ��   ����ʡ������Ȳ���ˮƽλ��һ�½��ͼ�ⲻ��ˮƽ����������һ��㡣
	TuiBu(1,1);		// ���� ���� �ٻ�ԭλ����ȡ��Ϣ
	for(i=0;i<10;i++)
	{
		rt_thread_delay(10);
		if((IO1PIN&BIT20)>0)
		{
			rt_kprintf("tuibu(1,1)\r\n");
			break;
		}
	}
	TuiBu(0,0);
	rt_thread_delay(100);
	g_sensor.TuiBuFangPing=((IO1PIN&BIT20)>0);
	if(g_sensor.TuiBuFangPing)
	{
		tuibumapancount = m_tuibu_middle;
	}else{
		TuiBu(1,0);				//�½�
	}
	
	BeiBu(1,0);       //������ʼ���½�
	initMode = 1;    // �Ȳ���ʼ��ģʽ	
	TuiBu(1,0);				// �Ȳ��½�
	LeftGuard(1,GuardDirDown);    // ������    ���������ǰ������������Ȳ����Ƹ��뿪�󣬲�������
	RightGuard(1,GuardDirDown);		// �һ�����    ���������ǰ������������Ȳ����Ƹ��뿪�󣬲�������
//	Dining(1,DiningDirUp);			// �����Ƹ˱俿��  ��ǰ�����������뱳����ƽ�����У�����ͻ���������ͻ
	g_sensor.BianMenPingYiDaoDi= (0==(IO1PIN&BIT26));
	g_sensor.BianMenPingYiDaoDing= (0==(IO1PIN&BIT25));
	if(g_sensor.BianMenPingYiDaoDing)  //   ����ƽ�ƻ������ڶ���λ�ã�δ����,�����ſ���
	{	
		// �������������Ƹ��½���λ
		ZuoBianQiShengJiang(1,0);
	}		
	for(i=0;i<60*50;i++)  // 60S
	{
		rt_kprintf("#");
		g_sensor.BeiBuFangPing=(0==(IO1PIN&BIT27));   // ��λ���أ�ֻ��ͨ����ܶ�ȡ	
	  if(g_sensor.BeiBuFangPing)
		{
			rt_kprintf("������ƽ��λ\r\n");
				BeiBu(0,0);
				Dining(1,DiningDirUp);		// �����俿��
		}
		
		g_sensor.TuiBuFangPing=((IO1PIN&BIT20)>0);
		if(g_sensor.TuiBuFangPing)
		{
			//rt_kprintf("�Ȳ���ƽ��λ\r\n");
			//rt_thread_delay(200);    // 150-168mm ���ܼ�⵽������ֵ,���������˶�ʱ��⵽λ��ҲҪ��ʱͣ
			tuibumapancount = m_tuibu_middle;
			TuiBu(0,0);
		}
		
		if(!g_MT_work.beibu && !g_MT_work.tuibu && i>10*LeftGuardSetTime && (initBianMen(i) == 6))   //ѭ������� �������Ȳ�����಻����
		{
			rt_kprintf("��ʼ�����е�����\r\n");
				Dining(0,0);
				DiningCount = 0;
				break;	
		}
		
		if(i > 10*LeftGuardSetTime)   // 15s
		{
				LeftGuard(0,0);
			  RightGuard(0,0);
				LeftGuardCount = 0;
				RightGuardCount = 0;
		}		
		
		rt_thread_delay(20);   //����ʱ100mS���ж�
		// �Ȳ��Ƹ���ˮƽλ�����£�Ϊ�˼��ٶ�ʱʱ�䣬�ڵ��������ʱ�򣬶�ȡ�����Ƿ��ж�����û����������˵�����ͣ��
		if(g_MT_work.tuibu == 2)   //�������½�ʱ
		{
			rt_kprintf("^");
			if(prelegCnt == ErrorWheelCnt_leg)	// �Ȳ��Ƹ˵���
			{
					tuibumapancount = 0;
					TuiBu(0,0);							
					initMode = 0;    // ������ʼ��ģʽ	
					rt_thread_delay(20);				
					TuiBu(1,1);											// ��ʼ��
			}
			prelegCnt = ErrorWheelCnt_leg;
		}
	}
	// ���������һ�����ʼ��   ���һ����������������俿��
//	LeftGuard(1,GuardDirUp);
//	RightGuard(1,GuardDirUp);
//	Dining(1,DiningDirUp);
//	rt_kprintf("��������������\r\n");
//	rt_thread_delay(RT_TICK_PER_SECOND*LeftGuardSetTime);
//	LeftGuard(0,0);
//	RightGuard(0,0);
//	Dining(0,0);
//	DiningCount = 0;
//	LeftGuardCount = 10*LeftGuardSetTime;
//	RightGuardCount = 10*RightGuardSetTime;	
//	rt_kprintf("��������-%d,�Ȳ�����-%d,",beibumapancount,tuibumapancount);
//	rt_kprintf("������ʱ-%d,����-%d,�һ���-%d\r\n",DiningCount,LeftGuardCount,RightGuardCount);
	
			DoAllStop();
			DiningCount = 0;	
			LeftGuardCount = 0;
			RightGuardCount = 0;
			if(g_sensor.BianMenPingYiDaoDi)    //  ������Ź�
			{
				BianmenCount = 10*BianMenShengJiangYanShi;
				ZuobianqiCount = 0;	
			}
	rt_kprintf("��ʼ��ʱ��:%d s\r\n",i/10);
	rt_kprintf("��ʼ��������ϣ�\r\n");
}



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
	rt_timer_init(&m_timer, "act", ActionTimeout, RT_NULL, 0.35f*RT_TICK_PER_SECOND, RT_TIMER_FLAG_ONE_SHOT);
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

	rt_kprintf("start \r\n");
	// ��ʱ��1�����ڱ����̲���
//	timer1_init();
//	timer1_clear();
	// �������̶�ȡ
//	StartReadMaPan();
	rt_hw_timer1_init();
	
	
	// ���״̬��ʼ��
//	rt_thread_delay(RT_TICK_PER_SECOND*10/1000);   // 10mS
//	ActionStatusInit();
		initAllPosition();
	
	// ��ȡ�Ƿ�������
//	IO1SET = BIT17; 	// �����ؼ̵���
//	rt_thread_delay(RT_TICK_PER_SECOND*3000/1000); // 3s
	g_diancidianliangAd = GetAdcSmoothly(3);
	if(g_diancidianliangAd<399)			// ��ȡ��ѹ<12V
		g_sensor.DianChiZhuOn=0;		// �����ڵ��
	else
		g_sensor.DianChiZhuOn=1;		// ���ڵ��
//	IO1CLR = BIT17;		// �ر�����
	rt_kprintf("\t��ص�ѹ: %d \r\n",g_diancidianliangAd);
	if(g_sensor.DianChiZhuOn)
		rt_kprintf("��ش��ڲ���\r\n");
	else
		rt_kprintf("����鲻���ڻ��߹ر�\r\n");
	// �� CAN1 �豸
	CanAnalyzerInit();
	rt_thread_delay(RT_TICK_PER_SECOND*10/1000);   // 10mS
	
	while(1)
	{	
		// �ȴ������ʼ�����
		rt_mb_recv(&m_mb, &cmd, RT_WAITING_FOREVER);	// �ȴ���������
		DoStop();
		rt_kprintf("���յ��������� %d\r\n",(int)cmd);
		rt_event_recv(&m_evt,0xffffffff , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, RT_WAITING_NO, &evt);	// ���¼�
		switch(cmd)    // Ϊ�������Զ������������
		{
			case LeftGuardUp:
				DoLeftGuardUp();
				break;
			case LeftGuardDown:
				DoLeftGuardDown();
				break;
			case RightGuardUp:
				DoRightGuardUp();
				break;
			case RightGuardDown:
				DoRightGuardDown();
				break;	
			case GuardUp:
				DoGuardUp();
				break;
			case GuardDown:
				DoGuardDown();
				break;
			case DiningUp:
				DoDiningUp();
			  break;
			case DiningDown:
				DoDiningDown();
			  break;
			
			
			case BianMenQuanKai :	
				DoAllStop();
				DoBianmenkai();
				break;

			case BianMenQuanGuan :
				DoAllStop();
				DoBianmenguan();
				break;				
				
			case PingTang  :
				DoAllStop();
				DoFangPing();
				break;	
		
			case QiZuo	:
				DoStop();
				DoZuoYi();
				break;
		
			case XinZangTangWei :
				DoStop();
				DoTangYi();
				break;

			case ZuoBian  :	
				DoAllStop();
				if(!DoBianmenkai())	// ˵��û�а�������
					DoZuoYi();
				break;	

			case FuYuan  :	//��ͣ��Ҫ����2�������������
				DoAllStop();
				if(AllowFuYuan())		// �Զ�������
				{if(!DoFangPing()) 	// ˵��û�а�������
				  {if(!g_sensor.BianMenGuanDaoDi)  // ����û�йرյ�λ
					   DoBianmenguan();}}
				break;
	
			default:
				DoAllStop();
				rt_kprintf("û���ҵ���Ӧ�Ķ���ָ��\r\n");
				break;
		}
		DoStop();
		rt_mb_recv(&m_mb, &cmd, RT_WAITING_NO);	// ������
		rt_event_recv(&m_evt,0xffffffff , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, RT_WAITING_NO, &evt);	// ���¼�
		rt_kprintf("��������\r\n");

	}
}

