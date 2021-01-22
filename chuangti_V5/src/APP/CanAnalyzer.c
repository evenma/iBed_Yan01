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
#include "CanAnalyzer.h"

unsigned char flag_qidong=0;
//static u8 flag_bianmen=0;
//static u8 m_bianmen=0;
//u16 YanShiCount=0;		// ����������ʱ������


s_FlagCmd g_FlagCmd={0};	
unsigned char MianBan_B=0;
unsigned char MianBan_A=0;



// ���յ�CAN�ص�����   10mSͨѶ���ʣ�2����壬�൱��10ms��ȡ����
rt_err_t CanRxInd(rt_device_t dev, rt_size_t size)
{
	stcRxBUF rcvBuf;
	unsigned char cmd;
	u32 whosend;
	while(CanRead(0, &rcvBuf))
	{
		whosend=rcvBuf.RxCANID.FrameFm.stcSTRFRFM.ID_BIT;
		if(whosend==0x100)		// �ֿ�����������
			MianBan_A=rcvBuf.CANRDA.Bits.Data1_BIT;
		if(whosend==0x101)
			MianBan_B=rcvBuf.CANRDA.Bits.Data1_BIT;
		if((MianBan_A!=0)&&(MianBan_B!=0))	// ��ʾͬʱ���߶��а���
			cmd = MianBan_A;		// �ֿ��� ��������
		else
			cmd = MianBan_A | MianBan_B;
		switch(whosend)
		{
		case 0x100:	
		case 0x101: 
			switch(cmd)
			{
			case 1:
				ActionStartCmd(BeiSheng);
				break;
			case 2:
				ActionStartCmd(BeiJiang);
				break;
			case 3:
				ActionStartCmd(TuiSheng);
				break;
			case 4:
				ActionStartCmd(TuiJiang);
				break;
			case 15:		
				ActionStartCmd(ZuoFanSheng);
				break;
			case 16:		
				ActionStartCmd(ZuoFanJiang);
				break;
			case 17:		
				ActionStartCmd(YouFanSheng);
				break;
			case 18:		
				ActionStartCmd(YouFanJiang);
				break;

			case 100: //ֹͣ����
				g_FlagCmd.Anything=0;
				ActionStopCmd(STOP);
				break;
		case 110: // ������
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
						ActionStartCmd(LeftGuardUp);
					else
						ActionStopCmd(LeftGuardUp);				
				}
				break;			
		case 111: // ������
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
						ActionStartCmd(LeftGuardDown);
					else
						ActionStopCmd(LeftGuardDown);				
				}
				break;
		case 112: // �һ�����
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
						ActionStartCmd(RightGuardUp);
					else
						ActionStopCmd(RightGuardUp);				
				}
				break;			
		case 113: // �һ�����
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
						ActionStartCmd(RightGuardDown);
					else
						ActionStopCmd(RightGuardDown);				
				}
				break;	
		case 114: // ���һ�����
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
						ActionStartCmd(GuardUp);
					else
						ActionStopCmd(GuardUp);				
				}
				break;			
		case 115: // ���һ�����
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
						ActionStartCmd(GuardDown);
					else
						ActionStopCmd(GuardDown);				
				}
				break;
		case 116:				//ACTION_DINING_DOWN, �����
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
						ActionStartCmd(DiningDown);
					else
						ActionStopCmd(DiningDown);				
				}		
				break;
		case 117:				//	ACTION_DINING_UP,�俿��
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
						ActionStartCmd(DiningUp);
					else
						ActionStopCmd(DiningUp);				
				}						
				break;
			case 121: // ����
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
		//			g_FlagCmd.ZuoYi= !g_FlagCmd.ZuoYi;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
		//			if(g_FlagCmd.ZuoYi)
					if(g_FlagCmd.Anything)
						ActionStartCmd(QiZuo);
					else
						ActionStopCmd(QiZuo);				
				}
				break;
			case 122: // ƽ��
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
	//				g_FlagCmd.PingTang= !g_FlagCmd.PingTang;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
	//				if(g_FlagCmd.PingTang)
					if(g_FlagCmd.Anything)
						ActionStartCmd(PingTang);
					else
						ActionStopCmd(PingTang);				
				}
				break;
			case 123: //������λ��������
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
		//			g_FlagCmd.TangYi= !g_FlagCmd.TangYi;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
		//			if(g_FlagCmd.TangYi)
						ActionStartCmd(XinZangTangWei);
					else
						ActionStopCmd(XinZangTangWei);				
				}
				break;	
			case 124: // ����ȫ��
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
		//			g_FlagCmd.BianMenKai= !g_FlagCmd.BianMenKai;			
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
		//			if(g_FlagCmd.BianMenKai)		
					{
//						flag_bianmen=1;
//						if((m_bianmen==0)&&(flag_bianmen==1))
//							YanShiCount=0;
						ActionStartCmd(BianMenQuanKai);
	//					m_bianmen=flag_bianmen;
					}
					else
						ActionStopCmd(BianMenQuanKai);				
				}
				break;
			case 125: // ����ȫ��
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
	//				g_FlagCmd.BianMenGuan= !g_FlagCmd.BianMenGuan;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
	//				if(g_FlagCmd.BianMenGuan)
					{
//						flag_bianmen=0;
//						if((m_bianmen==1)&&(flag_bianmen==0))
//							YanShiCount=0;						
						ActionStartCmd(BianMenQuanGuan);
//						m_bianmen=flag_bianmen;						
					}
					else
						ActionStopCmd(BianMenQuanGuan);				
				}
				break;

			case 126: // ����
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
	//				g_FlagCmd.ZuoBian= !g_FlagCmd.ZuoBian;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
	//				if(g_FlagCmd.ZuoBian)
						ActionStartCmd(ZuoBian);
					else
						ActionStopCmd(ZuoBian);				
				}
				break;					

			case 127: //��ԭ
				if(flag_qidong==0)	// ��һ�δ���
				{
					flag_qidong = 1;
	//				g_FlagCmd.FuYuan= !g_FlagCmd.FuYuan;
					g_FlagCmd.Anything = !g_FlagCmd.Anything;
					ActionStopCmd(STOP);
					if(g_FlagCmd.Anything)
	//				if(g_FlagCmd.FuYuan)
						ActionStartCmd(FuYuan);
					else
						ActionStopCmd(FuYuan); 			
				}
				break;	

				case 0:			// �޶���
					ActionStartCmd(ActionCmdNone);
					break;
				default:
					ActionStartCmd(ActionCmdNone);
					break;
			}
			break;
			case 0x300:
				g_ZuobianqiSensor= *(s_ZuobianqiSensor*)&rcvBuf.CANRDA;
				break;
		default:
			break;
		}	
	}
	return RT_EOK;
}

// 50ms ���ڷ���
static void CanSend50(void* p)
{
	static u8 cntSend = 0;
	
//	if(cntSend++ >= 5)
	if(cntSend++ >= 2)
	{
		cntSend = 0;
	}
	if(cntSend == 0)
	{
		CanWrite(0, 0x200,8,(u8*)&g_sensor);	
	}
	else if(cntSend == 1)
	{
		CanWrite(0, 0x201,8,(u8*)&g_MT_work);
	}
}

static struct rt_timer timer50;

// CANAnalyzer ��ʼ��
void CanAnalyzerInit(void)
{
	memset((void*)&g_FlagCmd,0,sizeof(g_FlagCmd));

	flag_qidong=0;
	g_FlagCmd.Anything=0;
	
	// �� CAN1 �豸
	CanOpen(0,CanRxInd);
//	rt_timer_init(&timer50, "can snd", CanSend50, RT_NULL, RT_TICK_PER_SECOND*0.05f, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
	rt_timer_init(&timer50, "can snd", CanSend50, RT_NULL, RT_TICK_PER_SECOND*100/1000, RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
	
	rt_timer_start(&timer50); 
}


