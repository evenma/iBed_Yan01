/**
版本说明：
	  Yan01版配置，硬件V5B;软件基础版V5;
		1：背部推杆 -- 编码盘位置，起始位置，机械联动滑背， 升出大约5cm 与餐桌推杆互斥。
		2：腿部推杆 -- 编码盘位置，起始位置，中间位置停122mm为水平位
    3：左护栏推杆- 和餐桌推杆互斥，餐桌下来，左右护栏也要下去  150mm
		4：右护栏推杆- 和餐桌推杆互斥，餐桌下来，左右护栏也要下去  150mm
		5：餐桌推杆 -- 配合餐桌旋转电机(在坐便器控制板上)，餐桌下，旋转电机转，下来时餐桌平放，上去后餐桌变靠背。130mm
		（坐便器板上，还有一个液晶屏旋转电机，餐桌平方时，随意变，变成餐桌变靠背后不能竖起来）
		6：座便升降 -- 配合便门平移升降，联动
		7：便门平移 -- 配合便门升降，座便升降，联动 起点位置，终点位置
		8：便门升降 -- 配合座便升降，便门平移，联动
结构： 餐桌不动，起背，那滑背机构就会碰撞到餐桌造成机械结构损坏；餐桌下来，护栏不动，两者也形成严重的机械结构损坏，
			   所以，起背升，餐桌需要离开>=5cm;餐桌升到一半，护栏必须开始下降；
问题1：滑背和餐桌关联性太大，起背支持点动和一键式两种模式，什么时候需要餐桌配合运动？餐桌独立控制怎么来控？
问题2：剪刀口太多。护栏缺口容易套脚，降下后切断脚。头部容易被餐桌敲到。

餐桌推杆参数：
    1.推杆行程 120mm 用时 11s    10mm/s  1200N

餐桌的运动控制逻辑：
    1.在座椅模式和一键式升起模式下开启，变餐桌，开启运动，同步启动。护栏同步下降 ，起坐模式下，护栏不可动
		2.在平躺模式下和一键式放下，要运动一定时间后开启变靠背；护栏同步升，平躺模式下，护栏可升可降
		3.在点动模式下升背，让出滑背空间，平躺起来时，变餐桌一定时间（10S），抬起>5cm,停下；记住背部位置   (不会碰着护栏)
		4.点动模式下降背，到背部位置后，开启变靠背一定时间(10S).
		5.座椅模式下，收回餐桌，不同到底，少一定时间(10S)，
		6.躺椅模式下，变餐桌一定时间（10S），抬起>5cm,停下 (不会碰着护栏)
		7.上电初始化，任意位置，码盘为0，怎么操作。降背，等放平后，餐桌变靠背；升背，如果停，餐桌需要10S位置，不停到底
		8.单独操作餐桌，餐桌变靠背，条件是起背位置，未初始化不允许操作；靠背变餐桌，护栏必须同步下降到位，
日志：
	2021.1.21.  关闭看门狗，主要是初始化电机复位操作存在一定风险，需要人工手动复位安全一点
							点动时不需要让出滑背空间，滑背缩短，不冲突。
							座椅模式下，餐桌旋转电机，和餐桌机械臂电机，不在一个电路板上，需要同步处理，一旦异常后果非常严重。
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

// 邮箱相关变量
static struct rt_mailbox m_mb;
static rt_uint32_t mbPool[1];
// 事件
static struct rt_event m_evt;
// 定时器，用于超时
static struct rt_timer m_timer={0};

u8 m_TuiCmd=0;				// 记录之前腿部动作命令
u8 m_bottonrelease=0;		// 按键释放记录
u8 flag_relase=0;
extern bool flag_pingtang;	// 记录平躺触发次数单次动双次停
extern unsigned char flag_qidong;
unsigned char flagRelaseKey=0;

#define LeftGuardSetTime 15
#define RightGuardSetTime 15
static u8 m_rotateDiningCnt=0;				// 时间
static u8 m_rotateAbnorCnt=0;					// 异常次数
#define TIME_ROTATE_DINING	8			// 6秒

static u16 LeftGuardCount = 0;
static u16 RightGuardCount = 0;
#define BianMenShengJiangYanShi 6 		// 便门升降延时时间10S   行程55mm,丝杆牙距5mm,转速200-260r/min  4S
#define BianMenPingYiYanShi  18			// 便门平移延时       10S  位移250mm,丝杆牙距5mm,转速200-260r/min 0.06*L  50转  12-15S
#define ZuoBianQiShengJiangYanShi 7 //更换电机为推杆 参数：电机行程25mm,机械行程158mm 7mm/s 1500N  15	// 座便器升降延时 10S   滚轮位移160mm,皮带牙距5mm,滚轮20齿5mm,+平行X升降210mm,转速 7.5-10r/min    L/100V    10S-13S
//static u8 BianMen_Sheng_Count=0;		// 便门升降推杆的延时计数器
//static u8 BianMen_Jiang_Count=0;		// 便门升降推杆的延时计数器
//static u8 ZuoBianQi_Sheng_Count=0;	// 座便器升降推杆的延时计数器
//static u8 ZuoBianQi_Jiang_Count=0;	// 座便器升降推杆的延时计数器
static u8 ZuobianqiCount = 0;
static u8 BianmenCount = 0;
#define BeiBuYanShi  30 //  mm/s 130mm
#define TuiBuYanShi  30//35  3.9mm/s 150mm
#define FanShenYanShi 20
#define AutoJieBianYanShi  100
//u16 ChuangTiYanShiCount=0; // 床体翻转延时计数器
#define BeiBuTongBuLowValue   200//1400    // 滑背电机和背部推杆同步控制点   低位  40脉冲/mm
#define BeiBuTongBuHighValue  BEI_MAPAN_MAX//6100    // 滑背电机和背部推杆同步控制点   低位
// 动作调试模式，1表示调试模式
#define DONG_ZUO_DEBUG	1
// 更换餐桌机械臂推杆，改为TA16,静音,4.7mm/s,110mm行程,时长在25s。在行程55mm处有个中间位置传感器
#define TIME_RUN_DINING_MT 		21 // 实际时间
//#define TIME_RUN_DINING_MT 		10// 13  推杆尺寸130修改位100 //S 实际运行时间11.1秒 10mm/s, 120mm  推杆升到底，变餐桌；推杆在起始位置，变靠背
#define TIME_DINING_CONFLICT_HUABEI   0 // 机械调整不冲突了，保留//   1		// 与滑背冲突的位置 >5cm,的时间点
u16 DiningCount = 0;    // <255   100ms/pcs    0为靠背 TIME_RUN_DINING_MT为餐桌
//static u8 flagDiningNormal = 0;		// 数据正常化后此数据为1，依赖背部推杆传感器检测；
#define HUABEI_CONFLICT_POSITION	200   //暂停一下，可判断一下病人头部是否可能被餐桌压倒 //BEI_MAPAN_MAX/2    // 与滑背冲突的位置 70mm  背部推杆需要时间   3.9mm/s 130mm
u8 DiningMode=0;     // 模式选择  0 不动，1起背升 2起背降 3座椅 4躺椅 5平躺
typedef enum {
	DiningCmdNone = 0,		// 无动作，停止动作发送该位时
	DiningBeiSheng,		// 背升
}DiningCmd;
typedef enum {
	DiningDirUp = 0,		// 变靠背
	DiningDirDown 			// 变餐桌
}DiningDir;
typedef enum {
	GuardDirUp = 0,		// 护栏上升
	GuardDirDown 			// 护栏下降
}GuardDir;

static u8 initMode=0;					// 初始化模式，下面两个计数码盘只有初始化使用
static s16 ErrorWheelCnt_back=0; // 背部行程位置码盘累计值
static s16 ErrorWheelCnt_leg=0; // 腿部行程位置码盘累计值
static s16 prebackCnt = 0;       //前一刻计数值
static s16 prelegCnt = 0;
//码盘读取线程启用标志
//static u8 MaPanReadRun = 0;
//static char mapanread_stack[512];
//static struct rt_thread mapanread_thread;
static u8 FanShenMaPanFlag=0;

static void TuiBu(u8 work,u8 dir);
static void Dining(u8 work,u8 dir);   // 1 down变餐桌 0 up变靠背
// 推背或者滑背 M5   P0.10 up; P0.11 Down;
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
		// 餐桌推杆的调整   1ms/pcs  可以修改为100ms/pcs
//void adjustTable(void)
//{
//		static u8 count=0; 
//	if(g_MT_work.beibu==1)//电机升累加
//	{
//			if(DiningMode == DiningBeiSheng )   // 用于区别座椅模式
//			{
//				if(DiningCount >= 10*TIME_DINING_CONFLICT_HUABEI)	   // 停过再启动，判断
//				{
//					Dining(0,0);    // 停止运动		
//					DiningMode = DiningCmdNone;
//					count = 0;
//				}else{
//						Dining(1,DiningDirDown);	// dining down  餐桌放下						
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
//	if(g_MT_work.beibu>1)// 电机降累减
//	{
//		if( DiningCount > 10*TIME_DINING_CONFLICT_HUABEI )  // 表示有变餐桌过，需要时长大。
//		{
//				Dining(1,DiningDirUp);	// dining up  变靠背					
//				count +=1;	
//				if(count >= 100)
//				{
//					count = 0;
//					if(DiningCount == 0)
//						DiningCount = 0;
//					else
//						DiningCount-=1;   // 100mS/pcs							
//				}
//		}else{				//查看背部是否降到位，不冲突
//			if(beibumapancount >  HUABEI_CONFLICT_POSITION )	 //背部滑背机构还没有到位，等待到位
//			{	
//				Dining(0,0);    // 停止运动		
//				count = 0;							
//			}else{
//				Dining(1,DiningDirUp);	// dining up  变靠背					
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

// 在餐桌机械臂运动过程中，判断餐桌旋转电机是否出现异常
static u8 isDiningRotateAbnor(u8 dir)
{
	if((DiningCount > 9*TIME_RUN_DINING_MT && g_ZuobianqiSensor.rotateDiningCnt == 0)||(DiningCount < 2*TIME_RUN_DINING_MT && g_ZuobianqiSensor.rotateDiningCnt == 10*TIME_ROTATE_DINING))
	{
		return 1;
	}

	if(dir == DiningDirDown)		// 变餐桌 DiningCount +
	{
			if(DiningCount > 5*TIME_RUN_DINING_MT)		// 4*TIME_RUN_DINING_MT时候，餐桌旋转电机启动
			{
				if(g_ZuobianqiSensor.rotateDiningCnt != 0 )		// 不是在靠背位，说明开始运动了 旋转电机运动时长 8S < 餐桌机械臂一半的时长 10S
				{
					if(g_ZuobianqiSensor.rotateDiningCnt != 10*TIME_ROTATE_DINING)   // 未到顶 
					{
						if(m_rotateDiningCnt == g_ZuobianqiSensor.rotateDiningCnt)		// 说明没动！！！
						{
								if(m_rotateAbnorCnt>3)			//2 餐桌旋转电机启动后停止了，说明异常可能线路板死机
								{
									m_rotateAbnorCnt = 0;
									return 1;						
								}
								m_rotateAbnorCnt +=1;
						}else{
							m_rotateAbnorCnt =0;
						}
					}
				}else{					//1 时间到餐桌旋转电机还未启动说明异常
					return 1;
				}	
			}
			m_rotateDiningCnt = g_ZuobianqiSensor.rotateDiningCnt;			
	}else{											//	变靠背 DiningCount -
			if(DiningCount < 8*TIME_RUN_DINING_MT)		// 9*TIME_RUN_DINING_MT时候，餐桌旋转电机启动
			{
				if(g_ZuobianqiSensor.rotateDiningCnt != 10*TIME_ROTATE_DINING )	// 未到顶 	说明开始运动了 旋转电机运动时长 8S < 餐桌机械臂一半的时长 10S
				{
					if(g_ZuobianqiSensor.rotateDiningCnt != 0)   // 不是在靠背位，
					{
						if(m_rotateDiningCnt == g_ZuobianqiSensor.rotateDiningCnt)		// 说明没动！！！
						{
								if(m_rotateAbnorCnt>3)			//2 餐桌旋转电机启动后停止了，说明异常可能线路板死机
								{
									m_rotateAbnorCnt = 0;
									return 1;						
								}
								m_rotateAbnorCnt +=1;
						}else{
							m_rotateAbnorCnt =0;
						}
					}
				}else{					//1 时间到餐桌旋转电机还未启动说明异常
					return 1;
				}	
			}
			m_rotateDiningCnt = g_ZuobianqiSensor.rotateDiningCnt;		
	}
	
	return 0;
}

// 用于餐桌机械臂随背部推杆自动调整
// 1.在背升模式（非座椅模式）下触发，2.背降和平躺模式、座椅变躺椅下触发 
void adjustTable(void)       // 100mS/pcs  餐桌推杆调整
{
	if(g_MT_work.beibu == 1)//电机升累加
	{
			if(DiningMode == DiningBeiSheng )   // 用于区别座椅模式
			{
				if(DiningCount >= 10*TIME_DINING_CONFLICT_HUABEI)	   // 停过再启动，判断 全行程
				{
					Dining(0,0);    // 停止运动		
					DiningMode = DiningCmdNone;
				}else{
						Dining(1,DiningDirDown);	// dining down  餐桌放下	只是用于防止滑背机构碰撞				
						DiningCount+=1;   // 100mS/pcs																			
				}
			}	
	}else if(g_MT_work.beibu > 1)// 电机降累减
	{
		if( DiningCount > 4*TIME_RUN_DINING_MT )  // 表示有变餐桌过，> 0.5行程
		{
				Dining(1,DiningDirUp);	// dining up  变靠背					
				DiningCount-=1;   // 100mS/pcs
				if(isDiningRotateAbnor(DiningDirUp) != 0)	//时刻监测异常
				{
					Dining(0,0);    // 停止运动		
				}
		}else{				//查看背部是否降到位，不冲突    安全高度界限
			if(beibumapancount >=  HUABEI_CONFLICT_POSITION )	 //暂停几秒 判断病人头部是否可能被压，等待到位
			{			
				Dining(0,0);    // 停止运动								
			}else{
				Dining(1,DiningDirUp);	// dining up  变靠背					
				if(DiningCount == 0)
				{	
					Dining(0,0);
				}else{
					DiningCount-=1;   // 100mS/pcs			
//  如果餐桌机械臂 到 4*TIME_RUN_DINING_MT 餐桌旋转电机还没有转好，说明有问题	
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

	 if(c_bei_hall!=m_bei_hall)//有脉冲跳变
	 {	 
	   if(g_MT_work.beibu==1)//电机升累加
		 {
				beibumapancount+=1;
//				if(beibumapancount>BeiBuTongBuLowValue)	 //背部上升到一定高度
//				{	
//					TuiBei(1,1);	
//				}	 			 
	   }
	   if(g_MT_work.beibu>1)// 电机降累减
	   {
				if(beibumapancount==0)
						beibumapancount=0;
				else
						beibumapancount-=1;
	//		if(beibumapancount<BeiBuTongBuHighValue)	 //背部降低到一定高度
	//				TuiBei(1,0);		 											
	   }
	 }	
//	adjustTable();	   // 调整餐桌  1ms/pcs
	 if(g_sensor.BeiBuFangPing)
		 beibumapancount=0;

	 if(c_tui_hall!=m_tui_hall)//有脉冲跳变
	 {	 
	   if(g_MT_work.tuibu==1)//电机升累加
				tuibumapancount+=1;
	   if(g_MT_work.tuibu>1)// 电机降累减
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
	 m_TuiBuStatus=c_TuiBuStatus;	 // 刷新下一个状态				 
//	 c_TuiBuStatus=(0==(IO1PIN&BIT20));
	 c_TuiBuStatus=((IO1PIN&0X100000)>0);

	 if(g_MT_work.tuibu==1)//电机升累加
	 {
		 if((m_TuiBuStatus==0)&&(c_TuiBuStatus==1))//表示发生上升沿
		 {
	//		 rt_kprintf("\t腿升: %d \r\n",tuibumapancount);
			 tuibumapancount=m_tuibu_middle;
		 }
	 }
	
	 if(g_MT_work.tuibu>1)// 电机降累减
	 {
		 if((m_TuiBuStatus==1)&&(c_TuiBuStatus==0))//表示发生下降沿
		 {
		//	 rt_kprintf("\t腿降: %d \r\n",tuibumapancount);
			 tuibumapancount=m_tuibu_middle;
		 }
	 }
	 	 

	 m_bei_hall=c_bei_hall;
	 m_tui_hall=c_tui_hall;

	// 翻身启动
	 if(FanShenMaPanFlag)
	 {
		 c_youfan_hall = ((IO1PIN&0X200000)>0);   //p1.21
		 c_zuofan_hall = ((IO1PIN&0X10000000)>0);	// p1.28
		 
		 if(c_zuofan_hall!=m_zuofan_hall)//有脉冲跳变
		 {	 
		   if(g_MT_work.zuofanshen==1)//电机升累加
			 zuofanmapancount+=1;
		   if(g_MT_work.zuofanshen>1)// 电机降累减
		   {
			 if(zuofanmapancount==0)
				 zuofanmapancount=0;
			 else
				 zuofanmapancount-=1;
		   }
		 }
		 if(g_sensor.ZuoFanFangPing)
			 zuofanmapancount=0;
		 
		 if(c_youfan_hall!=m_youfan_hall)//有脉冲跳变
		 {	 
		   if(g_MT_work.youfanshen==1)//电机升累加
			 youfanmapancount+=1;
		   if(g_MT_work.youfanshen>1)// 电机降累减
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
// 码盘读取进程，低优先级进程
static void MaPanReadThreadEntry(void* parameter)
{
	rt_kprintf("进入码盘读取\r\n");
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
	rt_kprintf("停止码盘读取\r\n");
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
	PCONP|=(1<<2);  //timer1电源	
	/* prescaler = 0*/
   	T1PR = 0;
   	T1PC = 0;

	/* reset and enable MR0 interrupt */
	T1MCR = 0x3;
	T1MR0 = PCLK / 1000; // 1ms 采样一次 
//	T1MR0 = PCLK ; // 1s 采样一次 		
	/* enable timer 0 */
	T1TCR = 1;
	
	/* install timer handler */
	rt_hw_interrupt_install(TIMER1_INT, rt_hw_timer1_handler, RT_NULL);
	rt_hw_interrupt_umask(TIMER1_INT);

}


//---------------------以下为推杆控制----------------//
// 所有电机都是高电平动，低电平不动
// work 是使能，dir是方向，1=伸出，0=缩回

// 背部 M1  P0.19 up; P0.20 Down;
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

// 腿部 M2   P0.18 up; P0.24 Down;
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
//		timer1_stop();			// 不考虑电机惯性
//		if(!g_MT_work.beibu)
//			StopReadMaPan();

	}
}


// 左翻身 M3   P0.17 up; P0.23 Down;
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
//			if(0==(IO1PIN&BIT29))  // 到底
//				g_sensor.ZuoFanFangPing = 1;
		}
	}
	else
	{
		g_MT_work.zuofanshen=0;
		IO0CLR=BIT17|BIT23;
		if(0==g_MT_work.youfanshen)	// 左右推杆全部到位后
			FanShenMaPanFlag=0;
	//	timer1_stop();			// 不考虑电机惯性		
	}
	*/
}

// 右翻身 M4  P0.16 up; P0.22 Down;
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
//			if(0==(IO1PIN&BIT22))  // 到底
//				g_sensor.YouFanFangPing = 1;
		}
	}
	else
	{
		g_MT_work.youfanshen=0;
		IO0CLR=BIT16|BIT22;
		if(0==g_MT_work.zuofanshen)	// 左右推杆全部到位后
			FanShenMaPanFlag=0;
	//	timer1_stop();			// 不考虑电机惯性		
	}
	*/
}


// 便门升降 M6  P0.6 up; P0.12 Down;
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

// 便门平移 M7  P0.5 移走; P0.13 移回;
static void BianMenPingYi(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(work)
	{
		if(dir)
		{
			g_MT_work.bianmenpingyi=1;
			IO0CLR = BIT5;			// 电机旋转180°安装后，需要调整正反转顺序
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

// 座便器升降 M8  P0.4 up; P0.15 Down;
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


// 左护栏 M3   P0.17 up; P0.23 Down;
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

// 右护栏M4
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
// 餐桌升降推杆M5   P0.10 up; P0.11 Down;
void Dining(u8 work,u8 dir)
{
	if(g_sensor.DianJiGuoLiuBao)
		 work=0;
	if(g_sensor.FangJia)
		work=0;
	if(work)
	{
		if(dir)  //1 变餐桌 
		{
			g_MT_work.dining=1;
			IO0SET = BIT10;
			IO0CLR = BIT11;		
		}
		else     //0 变靠背
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

// 用于点动放开后自动停止
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

// 用于连续动作电机停止
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

// 动作超时
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

// 左护栏升
BOOL DoLeftGuardUp(void)
{
	u8 i;
	rt_kprintf("LeftGuardUp\r\n");
	if(DiningCount > 5*TIME_RUN_DINING_MT)   // 餐桌下来了，和护栏冲突 > 0.5行程
	{
		rt_kprintf("餐桌推杆冲突\r\n");
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

// 左护栏降
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

// 右护栏升
BOOL DoRightGuardUp(void)
{
	u8 i;
	rt_kprintf("DoRightGuardUp\r\n");
	if(DiningCount > 5*TIME_RUN_DINING_MT)   // 餐桌下来了，和护栏冲突 > 0.5行程
	{
		rt_kprintf("餐桌推杆冲突\r\n");
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

// 右护栏降
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

// 左右护栏升
BOOL DoGuardUp(void)
{
	u8 i,tmp;
	rt_kprintf("DoGuardUp\r\n");
	if(DiningCount > 5*TIME_RUN_DINING_MT)   // 餐桌下来了，和护栏冲突 > 0.5行程
	{
		rt_kprintf("餐桌推杆冲突\r\n");
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

// 左右护栏降
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

// 餐桌收起  变靠背
// 		8.单独操作餐桌，餐桌变靠背，条件是起背位置，未初始化不允许操作；靠背变餐桌，护栏必须同步下降到位，
static rt_uint8_t DoDiningUp(void)
{
	u8 i,tmp;
	rt_kprintf("DoDiningUp start\r\n");

	rt_kprintf("back:%d,DiningCount:%d\r\n",beibumapancount,DiningCount);
	// 如果床体未放平，餐桌变靠背，不能到底部，放平，运动到底
	// 滑背机构已经不冲突
//	if(beibumapancount < HUABEI_CONFLICT_POSITION)   // 背部推杆不再运动
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
				
				if(isDiningRotateAbnor(DiningDirUp) != 0)	//时刻监测异常
				{
					Dining(0,0);    // 停止运动		
				}
			}	
			rt_kprintf("Dining:%d\r\n",DiningCount);
			DiningCount = 0;
			Dining(0,0);
//	}else{     //  背部未放平，滑背机构在，
//	   rt_kprintf("Dining:%d\r\n",DiningCount);
//		 Dining(1,DiningDirUp);
//			if(DiningCount > 2*TIME_RUN_DINING_MT)	         // 餐桌机械臂在低位，不冲突滑背机构。定义0.2行程
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

// 餐桌放下
// 		8.单独操作餐桌，餐桌变靠背，条件是起背位置，未初始化不允许操作；靠背变餐桌，护栏必须同步下降到位，
static rt_uint8_t DoDiningDown(void)
{
	u8 i,tmp;
	rt_kprintf("DoDiningDown start\r\n");
	if(beibumapancount <= BEI_MAPAN_MAX-m_wucha)    // 如果背部不起来，餐桌放不平，没意义
	{
		rt_kprintf("need back max\r\n");
		return TRUE;	
	}
	LeftGuard(1,GuardDirDown);   //护栏降
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

		if(isDiningRotateAbnor(DiningDirDown) != 0)   // 异常处理
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

// 便门打开连续动作
BOOL DoBianmenkai(void)
{
	u16 i=0;
	rt_kprintf("open WC\r\n");
	if(!g_sensor.ZuoFanFangPing||!g_sensor.YouFanFangPing)
	{
		return TRUE;
	}
	g_sensor.BianMenGuanDaoDi=0;
	// 1.便门降的条件是  无
	if(g_sensor.BianMenPingYiDaoDi)	// 表示便门平移未开始,还在底部位置
	{
		BianMenShengJiang(1, 0);		// 延时N 秒,便门降到位
		for(i=0;i<BianmenCount;i++)
		{
			if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		// 便门关闭键和停止键跳出
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
		BianmenCount = 0;  // 便门推杆降到底部
	}
	BianMenShengJiang(0, 0);
	DbgPrintf("1.WC-Cover Down End\r\n");

	if(g_sensor.BianMenPingYiDaoDing)  // 2 便门移走到位顶部
	{
		BianMenPingYi(0, 0);
	}
	else
	{
		BianMenPingYi(1, 1);
	}

	if(!g_sensor.BianMenPingYiDaoDing)  // 2 便门移走到位顶部
	{		
		for(i=0;i<(10*BianMenPingYiYanShi);i++)
		{
			g_sensor.BianMenPingYiDaoDing= (0==(IO1PIN&BIT25));
			if(g_sensor.BianMenPingYiDaoDing)
				break;
			else		
			{ 
				if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		// 便门关闭键和停止键跳出
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
	if(!g_sensor.BianMenPingYiDaoDing)  //如果便门还未开到顶说明出问题了。需要停止所有动作
	{
		BianMenShengJiang(0, 0);
		BianMenPingYi(0, 0);			
		ZuoBianQiShengJiang(0, 0);
		return TRUE;		
	}
	// 延时N秒，座便器升到位
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
		if(g_sensor.BianMenPingYiDaoDi)	// 表示便门平移未开始,还在底部位置
		{
		// 延时N 秒,便门降到位
			BianMenShengJiang(1, 0);
			for(i=BianMen_Jiang_Count;i<(10*BianMenShengJiangYanShi);i++)
	//		for(i=YanShiCount;i<(10*BianMenShengJiangYanShi);i++)
			{
				if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		// 便门关闭键和停止键跳出
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
	//		YanShiCount = 0;	//	为下一个重新计数	
			BianMen_Jiang_Count = 10*BianMenShengJiangYanShi;   // 便门推杆降到底部
			BianMen_Sheng_Count = 0;
		}else{
				BianMenShengJiang(0, 0);
				BianMenPingYi(0, 0);			
				ZuoBianQiShengJiang(0, 0);		
			return TRUE;
		}
		BianMenShengJiang(0, 0);
		DbgPrintf("1.WC-Cover Down End\r\n");
	
		if(g_sensor.BianMenPingYiDaoDing)  // 2 便门移走到位顶部
		{
			BianMenPingYi(0, 0);
		}
		else
		{
			BianMenPingYi(1, 1);
		}

		if(!g_sensor.BianMenPingYiDaoDing)  // 2 便门移走到位顶部
		{		
			for(i=0;i<(10*BianMenPingYiYanShi);i++)
			{
				if(g_sensor.BianMenPingYiDaoDing)
					break;
				else		
				{ 
				  if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		// 便门关闭键和停止键跳出
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
		if(!g_sensor.BianMenPingYiDaoDing)  //如果便门还未开到顶说明出问题了。需要停止所有动作
		{
			BianMenShengJiang(0, 0);
			BianMenPingYi(0, 0);			
			ZuoBianQiShengJiang(0, 0);
			return TRUE;		
		}
		// 延时N秒，座便器升到位
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
			if(WaitTimeout(20, (1<<BianMenQuanKai)|(1<<STOP)))		// 便门关闭键和停止键跳出
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

	//	YanShiCount = 0;	//	为下一个重新计数	
		ZuoBianQi_Sheng_Count=10*ZuoBianQiShengJiangYanShi;		// 座便器升降推杆升到顶
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

// 便门关闭连续动作
static void DoBianmenguan(void)
{
	u16 i=0;
	rt_kprintf("WC CLOSE\r\n");
	g_sensor.BianMenKaiDaoDing=0;		
	if(g_sensor.BianMenPingYiDaoDing)  //   便门平移机构处于顶部位置，未移走
	{	
		// 延时N秒，先座便器升降推杆下降到位
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
		ZuobianqiCount = 0;// 座便器升降推杆降到位
	}
	ZuoBianQiShengJiang(0, 0);
	DbgPrintf("3.WC-Toilet Down End\r\n");

	if(g_sensor.BianMenPingYiDaoDi)  // 2 便门移回到底部
	{
		BianMenPingYi(0, 0);
	}
	else
	{
		BianMenPingYi(1, 0);
	}

	if(!g_sensor.BianMenPingYiDaoDi)  // 2 便门移回到底部
	{		
		for(i=0;i<(10*BianMenPingYiYanShi);i++)
		{
			if(g_sensor.BianMenPingYiDaoDi)
				break;
			else
			{
			  if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP))) 	// 便门关闭键和停止键跳出
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
	if(!g_sensor.BianMenPingYiDaoDi)	//如果便门还未开到底部说明出问题了。需要停止所有动作
	{
		BianMenShengJiang(0, 0);
		BianMenPingYi(0, 0);			
		ZuoBianQiShengJiang(0, 0);
		return ;		
	}

	// 延时N 秒,便门升到位
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
	if(g_sensor.BianMenPingYiDaoDing)  //   便门平移机构处于顶部位置，未移走
	{	
		// 延时N秒，先座便器升降推杆下降到位
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
			if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP))) 	// 便门关闭键和停止键跳出
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
		
	//	YanShiCount = 0;	//	为下一个重新计数	
		ZuoBianQi_Jiang_Count=10*ZuoBianQiShengJiangYanShi;			// 座便器升降推杆降到位
		ZuoBianQi_Sheng_Count=0;
	}else{
		BianMenShengJiang(0, 0);
		BianMenPingYi(0, 0);			
		ZuoBianQiShengJiang(0, 0);
		return;
	}
	ZuoBianQiShengJiang(0, 0);
	DbgPrintf("3.WC-Toilet Down End\r\n");

	if(g_sensor.BianMenPingYiDaoDi)  // 2 便门移回到底部
	{
		BianMenPingYi(0, 0);
	}
	else
	{
		BianMenPingYi(1, 0);
	}

	if(!g_sensor.BianMenPingYiDaoDi)  // 2 便门移回到底部
	{		
		for(i=0;i<(10*BianMenPingYiYanShi);i++)
		{
			if(g_sensor.BianMenPingYiDaoDi)
				break;
			else
			{
			  if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP))) 	// 便门关闭键和停止键跳出
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
	if(!g_sensor.BianMenPingYiDaoDi)	//如果便门还未开到底部说明出问题了。需要停止所有动作
	{
		BianMenShengJiang(0, 0);
		BianMenPingYi(0, 0);			
		ZuoBianQiShengJiang(0, 0);
		return ;		
	}

	// 延时N 秒,便门升到位
		BianMenShengJiang(1, 1);
		for(i=BianMen_Sheng_Count;i<(10*BianMenShengJiangYanShi);i++)
//		for(i=YanShiCount;i<(10*BianMenShengJiangYanShi);i++)
		{
			if(WaitTimeout(20, (1<<BianMenQuanGuan)|(1<<STOP))) 	// 便门关闭键和停止键跳出
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
//		YanShiCount = 0;	//	为下一个重新计数		
		BianMen_Sheng_Count=10*BianMenShengJiangYanShi;		// 便门升降推杆升到顶
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
		if(WaitTimeout(RT_TICK_PER_SECOND, (1<<FuYuan)|(1<<STOP)))		// 便门关闭键和停止键跳出
		{
			return FALSE;
		}
		if(!g_ZuobianqiSensor.FunctionWorkAuto)		// 自动洁便结束
			return TRUE;
	}
	return FALSE;
}




// 放平动作
BOOL DoFangPing(void)
{
	u16 i;
	u8 tmp;
    rt_kprintf("平躺 \r\n");
	if(!g_sensor.TuiBuFangPing)
	{
		rt_kprintf("腿部调整 \r\n");
		if(tuibumapancount>(m_tuibu_middle+m_wucha)) // 表示为心脏躺位状态
			TuiBu(1,0);
		if(tuibumapancount<(m_tuibu_middle-m_wucha)) // 表示座椅状态
			TuiBu(1,1);
	}
	if(!g_sensor.BeiBuFangPing)
	{
		rt_kprintf("背部调整 \r\n");
		BeiBu(1,0);
//		TuiBei(1,0);
	}
	LeftGuard(1,GuardDirUp);    // 护栏升
	RightGuard(1,GuardDirUp);
	Dining(1,DiningDirUp);  // 变靠背
/*	for(i=0;((i<(10*BeiBuYanShi))||((g_sensor.BeiBuFangPing==1)||(g_sensor.TuiBuFangPing==1)));i++)
	{
		if(WaitTimeout(20, (1<<STOP))) 	//停止键跳出
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
		if(g_sensor.BeiBuFangPing&&g_sensor.TuiBuFangPing) // 背部腿部同时放平
			break;
		else
		{
			if(WaitTimeout(20, (1<<PingTang)|(1<<STOP))) 	//停止键跳出
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
	if(DiningCount != 0)		// 餐桌推杆还未到底
	{
		if(g_ZuobianqiSensor.rotateDiningCnt > 0)		// 说明异常，不能变靠背
		{
			return TRUE;
		}
		rt_kprintf("餐桌变靠背未到底\r\n");
		Dining(1,DiningDirUp);  //变靠背
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
		rt_kprintf("变靠背到底\r\n");		
	}
	DoStop();
	if(!g_sensor.TuiBuFangPing)
		tuibumapancount = m_tuibu_max;
	else
		rt_kprintf("背部腿部已放平 \r\n");

	
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
		if(g_sensor.YouFanFangPing&&g_sensor.ZuoFanFangPing) // 背部腿部同时放平
			break;
		else
		{if(WaitTimeout(20, (1<<PingTang)|(1<<STOP))) 	//停止键跳出
		{	DoStop();
			return TRUE;
		}}
	}	
	DoStop();
	g_FlagCmd.Anything=0;
	return FALSE;
}

// 座椅模式
// 左右护栏下降，餐桌推杆变餐桌
static void DoZuoYi(void)
{
	u16 i;
	u8 tmp;
    rt_kprintf("座椅\r\n");
	g_sensor.FunctionWorkZuoYi=1;
	if(g_sensor.ZuoFanFangPing && g_sensor.YouFanFangPing)
	{
		
		BeiBu(1, 1);
//		TuiBei(1,1);		
		TuiBu(1, 0);
	
		LeftGuard(1,GuardDirDown);   // 护栏降
		RightGuard(1,GuardDirDown);
		rt_kprintf("背部:%d,腿部:%d",beibumapancount,tuibumapancount);
		
		for(i=0;i<(10*BeiBuYanShi);i++)
		{
			if(beibumapancount >= 3000)	
			{
					Dining(1,DiningDirDown);			// 变餐桌
			}
			if((beibumapancount>=m_beibu_max))
			{	BeiBu(0,0);TuiBei(0,0);}
			if((tuibumapancount<=m_wucha))
			{	 
				rt_thread_delay(20);
				TuiBu(0,0);		//	初始化后第一次无码盘数据
			}
			if((beibumapancount>=m_beibu_max - m_wucha)&&(tuibumapancount<=m_wucha)) // 背部腿部多到位了退出
			{	
				rt_thread_delay(20);
				break;
			}else{
				if(WaitTimeout(20, (1<<QiZuo)|(1<<STOP))) 	//停止键跳出
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
				if(DiningCount >= 10*TIME_RUN_DINING_MT)  // 是否已变餐桌
				{
					Dining(0,0);
				}else{
					if(g_MT_work.dining== 1)    // 未启动不能累加，
						DiningCount +=1;  //100ms/pcs						
				}
				// 如果检测到餐桌旋转电机启动了，并且走一半不动。需要马上停止餐桌机械臂运动，否则后果严重
				if(isDiningRotateAbnor(DiningDirDown) != 0)
				{
						Dining(0,0);
				}			
			}
		}
		rt_kprintf("背部:%d,腿部:%d",beibumapancount,tuibumapancount);
		beibumapancount = m_beibu_max;
		tuibumapancount = 0;
		LeftGuardCount = 0;
		RightGuardCount=0;
		LeftGuard(0,0);			
		RightGuard(0,0);
		Dining(0,0);		
	}

	DoStop();
	if(LeftGuardCount != 0 || RightGuardCount != 0)   // 护栏未放平，继续放，为防止与餐桌推杆冲突，单独工作
	{
		tmp =  (LeftGuardCount<RightGuardCount)?LeftGuardCount:RightGuardCount;	
		rt_kprintf("护栏未到位\r\n");
		rt_kprintf("left:%d	right:%d\r\n",LeftGuardCount,RightGuardCount);
		LeftGuard(1,GuardDirDown);   // 护栏降
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
	
	if(DiningCount != 10*TIME_RUN_DINING_MT)		// 躺椅变餐桌时间短 机械臂未到位
	{
		rt_kprintf("变餐桌未到位\r\n");
		Dining(1,DiningDirDown);  //变餐桌
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
			// 如果检测到餐桌旋转电机启动了，并且走一半不动。需要马上停止餐桌机械臂运动，否则后果严重
			if(isDiningRotateAbnor(DiningDirDown) != 0)
			{
					Dining(0,0);
					g_sensor.FunctionWorkZuoYi=0;	
					return;
			}
		}
		Dining(0,0);
		DiningCount = 10*TIME_RUN_DINING_MT;
		rt_kprintf("变餐桌到位\r\n");		
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

// 心脏躺位或者躺椅模式
// 躺椅模式下，左右护栏和餐桌机械臂怎么配合
// 躺椅模式下，非吃饭，所以，餐桌机械臂不运动，旋转餐桌台面不运动，注意左右护栏是否与餐桌机械臂会碰撞，如平躺到躺椅未碰撞，就不用控制
// 座椅到躺椅，餐桌机械臂收到滑背冲突位置
static void DoTangYi(void)
{
	u16 i;
    rt_kprintf("心脏躺位  躺椅\r\n");
	g_sensor.FunctionWorkTangYi=1;
	if(g_sensor.ZuoFanFangPing && g_sensor.YouFanFangPing)
	{
		if(beibumapancount>(m_beibu_middle+m_wucha))	// 背部为中间态
		{   
			BeiBu(1, 0);
		//	TuiBei(1,0);
		}
		else
		{
			BeiBu(1, 1);
		//	TuiBei(1,1);
		}
		if(tuibumapancount<m_tuibu_max)	// 腿部为最顶端
			TuiBu(1, 1);
		// 上抬腿的时候，如果餐桌在，腿部压倒餐桌，需要把餐桌收掉
		if(DiningCount > 0)	         // 餐桌机械臂在低位，不冲突滑背机构。定义0.2行程
		{
				Dining(1,DiningDirUp);  //变靠背	
		}			
	
	 	for(i=0;(i<(10*TuiBuYanShi));i++)
	 	{
			if((beibumapancount<=(m_beibu_middle+m_wucha))&&(beibumapancount>=(m_beibu_middle-m_wucha)))
			 {	BeiBu(0,0);TuiBei(0,0);}
			if(tuibumapancount>=m_tuibu_max )
			{	TuiBu(0,0);}
			if((beibumapancount<=(m_beibu_middle+m_wucha))&&(beibumapancount>=(m_beibu_middle-m_wucha))&&(tuibumapancount>=m_tuibu_max - m_wucha)) // 背部腿部多到位了退出
			{
				rt_thread_delay(20);
				break;
			}else{
				if(WaitTimeout(20, (1<<XinZangTangWei)|(1<<STOP))) 	//停止键跳出
				{
					DoStop();
					Dining(0,0);
					g_sensor.FunctionWorkTangYi=0;
					return;
				}
			}
			if(DiningCount > 5*TIME_RUN_DINING_MT)
			{
					if(g_ZuobianqiSensor.rotateDiningCnt > 5*TIME_ROTATE_DINING)		// 机械臂走过一半，还未开始餐桌旋转，
					{
						Dining(0,0);			
					}
			}
			if(g_MT_work.dining == 2)
			{
				DiningCount -= 1;
				if(isDiningRotateAbnor(DiningDirUp) != 0)  //异常处理
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
			Dining(1,DiningDirUp);  //变靠背	
			for(i=0;i < DiningCount;i++)
			{							
				if(WaitTimeout(20, (1<<DiningUp)|(1<<STOP)))		
				{
					Dining(0,0);
				}	
				DiningCount -=1;
				if(isDiningRotateAbnor(DiningDirUp) != 0)  //异常处理
				{
						Dining(0,0);
				}				
			}
	}
	Dining(0,0);
	
	g_sensor.FunctionWorkTangYi=0;	
	g_FlagCmd.Anything=0;
}



// 看门狗初始化
// 2880000 ,1秒
void WDInit(void)
{
//	WDTC = 0x00DBBA00;	// 设置看门狗定时器的固定装载值 : 5 秒
//	WDTC = 0x04F1A000;	// 设置看门狗定时器的固定装载值 : 30 秒
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

// 开始动作命令
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
//			if(g_sensor.TuiBuFangPing&&(m_TuiCmd>60))	// 持续按着且放平到位
			if(g_sensor.TuiBuFangPing)
			{			
				if(m_bottonrelease)		// 按键未释放
					{if(!flag_relase)
						TuiBu(0, 0);
					 else 
					 	TuiBu(1, 1); }
				else		// 按键释放过
					{TuiBu(1, 1);flag_relase=1;}
			}
			else				// 腿升 不在放平位置，或腿升在放平位置且
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
	//			if(g_sensor.TuiBuFangPing&&(m_TuiCmd>60))	// 持续按着且放平到位
	//				TuiBu(0, 0);
	//			else
	//				TuiBu(1, 0);
	//		}
			if(g_sensor.TuiBuFangPing)
			{			
				if(m_bottonrelease) 	// 按键未释放
					{if(!flag_relase)
						TuiBu(0, 0);
					 else 
						TuiBu(1, 0); }
				else		// 按键释放过
					{TuiBu(1, 0);flag_relase=1;}
			}
			else				// 腿升 不在放平位置，或腿升在放平位置且
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
			if(g_sensor.YouFanFangPing)   // 左翻身时不允许右翻身
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
			if(g_sensor.ZuoFanFangPing)    // 右翻身时不允许左翻身
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
		if(g_sensor.DianJiGuoLiuBao)		// 如果过流保护，发出停止命令
			 ActionStopCmd(STOP);
//		DoStop();		
//		ActionTimeout(NULL);
//		RestartTimer();
		break;
	default:
		DbgPrintf("发送消息到动作进程\r\n");
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
FINSH_FUNCTION_EXPORT(ActionStartCmd,"开始命令[cmd]");

// 停止动作命令
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

FINSH_FUNCTION_EXPORT(ActionStopCmd, "停止命令");


static void ActionStatusInit(void)
{
	g_sensor.TuiBuFangPing=((IO1PIN&BIT20)>0);	 // 假设腿部水平位为电机最高位
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

	if(g_sensor.BianMenPingYiDaoDing) // 假设坐便器升
	{
		ZuobianqiCount = 10*ZuoBianQiShengJiangYanShi;
		BianmenCount = 0;
	}
	if(g_sensor.BianMenPingYiDaoDi)    //  假设便门关
	{
		BianmenCount = 10*BianMenShengJiangYanShi;
		ZuobianqiCount = 0;	
	}

	DoAllStop();

	// 增加腿部初始化
	TuiBu(1,1);		// 向上 半秒 再回原位。读取信息
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
					if(g_sensor.BianMenPingYiDaoDing)  //   便门平移机构处于顶部位置，未移走,即便门开着
					{	
						//1 先座便器升降推杆下降到位
						ZuoBianQiShengJiang(1,0);
						BianMenStatus = 1;
						tickBianMen = i;
					}else{
						 BianMenStatus = 2;			// 说明坐便器在底部，便门平移机构在中间位置
					}
					break;
		case 1:
					if(i-tickBianMen > 10*ZuoBianQiShengJiangYanShi)
					{
						ZuoBianQiShengJiang(0, 0);
						ZuobianqiCount = 0;// 座便器升降推杆降到位
						BianMenStatus = 2;
					}	
					break;
		case 2:
					if(g_sensor.BianMenPingYiDaoDi)  // 2 便门移回到底部
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
						BianMenShengJiang(1, 1);				// 便门升降
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
						BianMenStatus = 6;											// 结束
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

// 需要为强制初始化平躺模式，所有电机位置归零，用于解决餐桌推杆位置冲突
// 初始化过程，
// 1 背部 腿部 放平，2 餐桌推杆变靠背   3 左右护栏降到底  4 便门关
// 背部延时 30s  腿部延时30S(如果方向反了放平就是60S)  餐桌推杆延时13s   左右护栏延时 15s  便门关是3电机串行联动15+18+6 <= 39S  
void initAllPosition(void)
{
	u16 i;
	rt_kprintf("开始初始化\r\n");		

	ErrorWheelCnt_back = 0;
	ErrorWheelCnt_leg = 0;
	
	
//	g_sensor.BianMenPingYiDaoDi= (0==(IO1PIN&BIT26));
//	g_sensor.BianMenPingYiDaoDing= (0==(IO1PIN&BIT25));

//	g_sensor.BianMenGuanDaoDi=g_sensor.BianMenPingYiDaoDi;
//	g_sensor.BianMenKaiDaoDing=g_sensor.BianMenPingYiDaoDing;

//	if(g_sensor.BianMenPingYiDaoDing) // 假设坐便器升
//	{
//		ZuobianqiCount = 10*ZuoBianQiShengJiangYanShi;
//		BianmenCount = 0;
//	}
//	if(g_sensor.BianMenPingYiDaoDi)    //  假设便门关
//	{
//		BianmenCount = 10*BianMenShengJiangYanShi;
//		ZuobianqiCount = 0;	
//	}
	
//	BeiBu(1,0);       
//	g_sensor.BeiBuFangPing=(0==(IO1PIN&BIT27));   // 只有通电才能读取	

	// 腿部初始化   不可省，如果腿部在水平位，一下降就检测不到水平，所以先升一点点。
	TuiBu(1,1);		// 向上 半秒 再回原位。读取信息
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
		TuiBu(1,0);				//下降
	}
	
	BeiBu(1,0);       //背部初始化下降
	initMode = 1;    // 腿部初始化模式	
	TuiBu(1,0);				// 腿部下降
	LeftGuard(1,GuardDirDown);    // 左护栏降    如果升，有前提条件，必须等餐桌推杆离开后，才能升。
	RightGuard(1,GuardDirDown);		// 右护栏降    如果升，有前提条件，必须等餐桌推杆离开后，才能升。
//	Dining(1,DiningDirUp);			// 餐桌推杆变靠背  有前提条件，必须背部放平后运行，以免和滑背机构冲突
	g_sensor.BianMenPingYiDaoDi= (0==(IO1PIN&BIT26));
	g_sensor.BianMenPingYiDaoDing= (0==(IO1PIN&BIT25));
	if(g_sensor.BianMenPingYiDaoDing)  //   便门平移机构处于顶部位置，未移走,即便门开着
	{	
		// 先座便器升降推杆下降到位
		ZuoBianQiShengJiang(1,0);
	}		
	for(i=0;i<60*50;i++)  // 60S
	{
		rt_kprintf("#");
		g_sensor.BeiBuFangPing=(0==(IO1PIN&BIT27));   // 限位开关，只有通电才能读取	
	  if(g_sensor.BeiBuFangPing)
		{
			rt_kprintf("背部放平到位\r\n");
				BeiBu(0,0);
				Dining(1,DiningDirUp);		// 餐桌变靠背
		}
		
		g_sensor.TuiBuFangPing=((IO1PIN&BIT20)>0);
		if(g_sensor.TuiBuFangPing)
		{
			//rt_kprintf("腿部放平到位\r\n");
			//rt_thread_delay(200);    // 150-168mm 均能检测到传感器值,所以往下运动时检测到位置也要延时停
			tuibumapancount = m_tuibu_middle;
			TuiBu(0,0);
		}
		
		if(!g_MT_work.beibu && !g_MT_work.tuibu && i>10*LeftGuardSetTime && (initBianMen(i) == 6))   //循环体出口 背部和腿部电机多不工作
		{
			rt_kprintf("初始化所有电机完毕\r\n");
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
		
		rt_thread_delay(20);   //先延时100mS后判断
		// 腿部推杆在水平位置以下，为了减少定时时间，在电机工作的时候，读取码盘是否有度数，没有增量度数说明电机停，
		if(g_MT_work.tuibu == 2)   //工作在下降时
		{
			rt_kprintf("^");
			if(prelegCnt == ErrorWheelCnt_leg)	// 腿部推杆到底
			{
					tuibumapancount = 0;
					TuiBu(0,0);							
					initMode = 0;    // 结束初始化模式	
					rt_thread_delay(20);				
					TuiBu(1,1);											// 开始升
			}
			prelegCnt = ErrorWheelCnt_leg;
		}
	}
	// 餐桌和左右护栏初始化   左右护栏升到顶，餐桌变靠背
//	LeftGuard(1,GuardDirUp);
//	RightGuard(1,GuardDirUp);
//	Dining(1,DiningDirUp);
//	rt_kprintf("护栏升，餐桌收\r\n");
//	rt_thread_delay(RT_TICK_PER_SECOND*LeftGuardSetTime);
//	LeftGuard(0,0);
//	RightGuard(0,0);
//	Dining(0,0);
//	DiningCount = 0;
//	LeftGuardCount = 10*LeftGuardSetTime;
//	RightGuardCount = 10*RightGuardSetTime;	
//	rt_kprintf("背部码盘-%d,腿部码盘-%d,",beibumapancount,tuibumapancount);
//	rt_kprintf("餐桌计时-%d,左护栏-%d,右护栏-%d\r\n",DiningCount,LeftGuardCount,RightGuardCount);
	
			DoAllStop();
			DiningCount = 0;	
			LeftGuardCount = 0;
			RightGuardCount = 0;
			if(g_sensor.BianMenPingYiDaoDi)    //  假设便门关
			{
				BianmenCount = 10*BianMenShengJiangYanShi;
				ZuobianqiCount = 0;	
			}
	rt_kprintf("初始化时长:%d s\r\n",i/10);
	rt_kprintf("初始化数据完毕！\r\n");
}



// 硬件初始化
static void ActionHwInit(void)
{
	// PA8,9 作为输入
	PINSEL0 &= ~(BIT16|BIT17);
	PINSEL0 &= ~(BIT18|BIT19);	
	
	DoStop();
	ActionTimeout(RT_NULL);
}

static void ActionInit(void)
{

	// 硬件初始化
	ActionHwInit();
	// 变量初始化

	// 初始化邮箱
	rt_mb_init(&m_mb, "act mb", mbPool, sizeof(mbPool)/sizeof(rt_uint32_t), RT_IPC_FLAG_FIFO);
	// 初始化事件
	rt_event_init(&m_evt, "act evt", RT_IPC_FLAG_FIFO);
	// 初始化定时器
	rt_timer_init(&m_timer, "act", ActionTimeout, RT_NULL, 0.35f*RT_TICK_PER_SECOND, RT_TIMER_FLAG_ONE_SHOT);
//	rt_timer_init(&m_timer , "act", ActionTimeout, RT_NULL, RT_TICK_PER_SECOND*0.1f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_HARD_TIMER);		// 周期性调用

}


static void RestartTimer(void)
{
	rt_timer_stop(&m_timer);
	rt_timer_start(&m_timer);
}

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

// 接收到CAN回调函数
extern rt_err_t CanRxInd(rt_device_t dev, rt_size_t size);
extern void CanAnalyzerInit(void);


// 动作进程入口
void ActionThreadEntry(void* parameter)
{
	rt_uint32_t cmd,evt;
	
	ActionInit();

	rt_kprintf("start \r\n");
	// 定时器1，用于编码盘采样
//	timer1_init();
//	timer1_clear();
	// 开启码盘读取
//	StartReadMaPan();
	rt_hw_timer1_init();
	
	
	// 电机状态初始化
//	rt_thread_delay(RT_TICK_PER_SECOND*10/1000);   // 10mS
//	ActionStatusInit();
		initAllPosition();
	
	// 读取是否有蓄电池
//	IO1SET = BIT17; 	// 打开蓄电池继电器
//	rt_thread_delay(RT_TICK_PER_SECOND*3000/1000); // 3s
	g_diancidianliangAd = GetAdcSmoothly(3);
	if(g_diancidianliangAd<399)			// 读取电压<12V
		g_sensor.DianChiZhuOn=0;		// 不存在电池
	else
		g_sensor.DianChiZhuOn=1;		// 存在电池
//	IO1CLR = BIT17;		// 关闭蓄电池
	rt_kprintf("\t电池电压: %d \r\n",g_diancidianliangAd);
	if(g_sensor.DianChiZhuOn)
		rt_kprintf("电池存在并打开\r\n");
	else
		rt_kprintf("电池组不存在或者关闭\r\n");
	// 打开 CAN1 设备
	CanAnalyzerInit();
	rt_thread_delay(RT_TICK_PER_SECOND*10/1000);   // 10mS
	
	while(1)
	{	
		// 等待接收邮件命令
		rt_mb_recv(&m_mb, &cmd, RT_WAITING_FOREVER);	// 等待接收命令
		DoStop();
		rt_kprintf("接收到动作命令 %d\r\n",(int)cmd);
		rt_event_recv(&m_evt,0xffffffff , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, RT_WAITING_NO, &evt);	// 清事件
		switch(cmd)    // 为触发型自动完成整个过程
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
				if(!DoBianmenkai())	// 说明没有按键按下
					DoZuoYi();
				break;	

			case FuYuan  :	//暂停需要发送2次命令才能跳出
				DoAllStop();
				if(AllowFuYuan())		// 自动洁便结束
				{if(!DoFangPing()) 	// 说明没有按键按下
				  {if(!g_sensor.BianMenGuanDaoDi)  // 便门没有关闭到位
					   DoBianmenguan();}}
				break;
	
			default:
				DoAllStop();
				rt_kprintf("没有找到相应的动作指令\r\n");
				break;
		}
		DoStop();
		rt_mb_recv(&m_mb, &cmd, RT_WAITING_NO);	// 清命令
		rt_event_recv(&m_evt,0xffffffff , RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, RT_WAITING_NO, &evt);	// 清事件
		rt_kprintf("动作结束\r\n");

	}
}

