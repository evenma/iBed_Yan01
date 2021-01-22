#include <rtthread.h>
#include <finsh.h>
#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif
#include "CAN/IncludeCan.h"
#include "CAN/Can.h"
#include "lpc214x.h"
#include <SmsApp.h>
#include <ActionThread.h>
#include "Sensor.h"
unsigned char flag_rotatePad=0;
unsigned char flag_qidong=0;
unsigned char flag_ganzao=0;
unsigned char flag_qianbu=0;
unsigned char flag_houbu=0;
unsigned char flag_zuobianjieshu=0;
unsigned char MianBan_B=0;
unsigned char MianBan_A=0;
// 接收到CAN回调函数
rt_err_t CanRxInd(rt_device_t dev, rt_size_t size)
{
	stcRxBUF rcvBuf;
	unsigned char cmd;
	u32 whosend;
	while(CanRead(0, &rcvBuf))
	{
		whosend=rcvBuf.RxCANID.FrameFm.stcSTRFRFM.ID_BIT;
		if(whosend==0x100)		// 手控器发出命令
			MianBan_A=rcvBuf.CANRDA.Bits.Data1_BIT;
		if(whosend==0x101)
			MianBan_B=rcvBuf.CANRDA.Bits.Data1_BIT;
		cmd = MianBan_A | MianBan_B;
		switch(whosend)
		{
		case 0x100:	
		case 0x101: 
			switch(cmd)
			{	
				case 118:	
				if(flag_qidong==0)	// 第一次触发
				{
				flag_qidong = 1;
				flag_rotatePad = !flag_rotatePad;
				if(flag_rotatePad)
					ActionStartCmd(RotatePADUp);
				else
					ActionStopCmd(RotatePADUp); 			
				}						
				break;
			case 119:		
				if(flag_qidong==0)	// 第一次触发
				{
				flag_qidong = 1;
				flag_rotatePad = !flag_rotatePad;
				if(flag_rotatePad)
					ActionStartCmd(RotatePADDown);
				else
					ActionStopCmd(RotatePADDown); 			
				}	
				break;			
			case 126:		// 座便开始
				ActionStartCmd(ZuoBianKaiShi);
				break;
			case 127:		//  座便结束
				if(flag_qidong==0)	// 第一次触发
				{
				flag_qidong = 1;
				flag_zuobianjieshu= !flag_zuobianjieshu;
				if(flag_zuobianjieshu)
					ActionStartCmd(ZuoBianJieShu);
				else
					ActionStopCmd(ZuoBianJieShu);				
				}
				break;
			case 128:		// 冲洗便盆
				ActionStartCmd(ChongXiBianPeng);
				break;
//			case 129:		//	清洗后部
			case 131:				// 前后安装互换了
				if(flag_qidong==0)	// 第一次触发
				{
				flag_qidong = 1;
				flag_houbu = !flag_houbu;
				if(flag_houbu)
					ActionStartCmd(QingXiTunBu);
				else
					ActionStopCmd(QingXiTunBu); 			
				}			
				break;
			case 130:		// 暖风烘干
				if(flag_qidong==0)	// 第一次触发
				{
				flag_qidong = 1;
				flag_ganzao = !flag_ganzao;
				if(flag_ganzao)
					ActionStartCmd(GanZao);
				else
					ActionStopCmd(GanZao);				
				}
				break;
//			case 131:		//	清洗前部
				case 129:
				if(flag_qidong==0)	// 第一次触发
				{
				flag_qidong = 1;
				flag_qianbu = !flag_qianbu;
				if(flag_qianbu)
					ActionStartCmd(QingXiQianBu);
				else
					ActionStopCmd(QingXiQianBu); 			
				}			
				break;
			case 100:
				flag_ganzao=flag_qianbu=flag_houbu=flag_zuobianjieshu=0;
				flag_rotatePad=0;
				ActionStopCmd(STOP);
				break;
			case 0:			// 无动作
				ActionStartCmd(ActionCmdNone);
				break;
			default:
				break;
			}
			break;
		case 0x110:		//  面板A设置参数
		{
			u8* pData = (u8 *)&rcvBuf.CANRDA;
			Set_A_Config.HoldOn=*pData;
			pData+=1;
			ActionMianBanASetShuiWen(*pData++);
			ActionMianBanASetNuanFengWenDu(*pData++);
			ActionMianBanASetShuiLiuQiangDu(*pData++);	
			break;
		}
		case 0x111:		//面板B设置参数
		{		
				Set_B_Config= *(s_SetBConfig*)&rcvBuf.CANRDA;
				ActionMianBanBSetSwitchControl();		
			
//		u8* pData = (u8 *)&rcvBuf.CANRDA;
//			Set_B_Config.HoldOn=*pData;
//			pData+=1;			
//			ActionMianBanBSetShuiWen(*pData++);
//			ActionMianBanBSetNuanFengWenDu(*pData++);
//			ActionMianBanBSetShuiLiuQiangDu(*pData++);
//			ActionMianBanBSetChuangDianWenDu(*pData++);
//			ActionMianBanBSetZhiNengModel(*pData++);
//			pData+=1;	// 植物人模式暂时不支持
//			ActionMianBanBSetChuangDianPower(*pData++);
//			ActionMianBanBSetChuangDianWenDu(*pData++);
			break;
		}	
		case 0x112:		// 参数配置
		{
			u16* pData = (u16 *)&rcvBuf.CANRDA;
			ActionSetBianPenZiDongPaiWuYanShi(*pData++);
			ActionSetBianPenZiDongPauWuJianCeYanShi(*pData++);
			ActionSetBianPenShouDongQingJieShiJian(*pData++);
			ActionSetHongGanShiJian(*pData++);
			break;
		}
		case 0x113:		// 参数配置
		{
			u16* pData = (u16 *)&rcvBuf.CANRDA;
			ActionSetTunBuQingXiShiJian(*pData++);
			ActionSetQianBuQingXiShiJian(*pData++);
			ActionSetZuoBianQuanQingXiShiJian(*pData++);
			break;
		}
		case 0x200:
			g_bedSensor= *(s_BedSensor*)&rcvBuf.CANRDA;
			break;
		case 0x201:
			g_bedsensor_MT_work = *(s_BedSensor_MT_work *)&rcvBuf.CANRDA;
		 break;
		default:
			break;
		}
	}
	return RT_EOK;
}

// 10ms 周期发送
static void CanSend10(void* p)
{
	static u32 counter = 0;
//	if(counter++>4)
	{
		counter = 0;
		CanWrite(0, 0x300,8,(u8*)&g_sensor);		
	}
}

static struct rt_timer timer10;
// CANAnalyzer 初始化
void CanAnalyzerInit(void)
{
	flag_qidong=0;
	// 打开 CAN1 设备
	CanOpen(0,CanRxInd);
//	rt_timer_init(&timer10, "can snd", CanSend10, RT_NULL, RT_TICK_PER_SECOND/100, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_HARD_TIMER);
	rt_timer_init(&timer10, "can snd", CanSend10, RT_NULL, RT_TICK_PER_SECOND*100/1000, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_HARD_TIMER);

	rt_timer_start(&timer10);
}

