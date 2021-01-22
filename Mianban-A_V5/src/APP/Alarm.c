#include <rtthread.h>
#include <finsh.h>
#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif
#include "CAN/IncludeCan.h"
#include "CAN/Can.h"
#include "lpc214x.h"
#include <Key/Key.h>
#include "Alarm.h"
#include "CanAnalyzer.h"

// ����Ӳ����ʼ��
static void AlarmHwInit(void)
{		
	ALARM_BUZZER(0);		
	IO0DIR |= BIT21|BIT22|BIT23|BIT24|BIT27|BIT28|BIT29;
	IO1DIR |= BIT16| BIT17|BIT18|BIT19;
	IO0SET |= BIT21|BIT22|BIT23|BIT24|BIT27|BIT28|BIT29;
	IO1SET |= BIT16| BIT17|BIT18|BIT19;

//	rt_thread_delay(RT_TICK_PER_SECOND/2);
	
	ALARM_ERROR_LED(0);
	ALARM_BUZZER(0);
	ALARM_RUN_LED(0);
	ALARM_SHUIWEI_LOW(0);
	ALARM_WUWU_HIGN(0);

// ��������ˮ�¸ߵ�ů��ǿ��	
	SET_NUANFEN_HIGH(0);
	SET_NUANFEN_MIDDLE(0);
	SET_NUANFEN_LOW(0);

	SET_SHUIWEN_HIGH(0);
	SET_SHUIWEN_MIDDLE(0);
	SET_SHUIWEN_LOW(0);
}


// �����߼� ��ʼ��
void AlarmInit(void)
{
	AlarmHwInit();
}


// ���Ź���ʼ��
// 2880000 ,1��
void WDInit(void)
{
//	WDTC = 0x00DBBA00;	// ���ÿ��Ź���ʱ���Ĺ̶�װ��ֵ : 5 ��
	WDTC = 0x04F1A000;	// ���ÿ��Ź���ʱ���Ĺ̶�װ��ֵ : 30 ��
	WDMOD= 0X03;   // ģʽ�趨
	feedWD();
}

void feedWD(void) // ι��
{
	WDFEED =0XAA;
	WDFEED =0X55;
}

u32 WDtimers(void)
{
	return WDTV; //���Ź���ʱ���ĵ�ǰֵ
}

