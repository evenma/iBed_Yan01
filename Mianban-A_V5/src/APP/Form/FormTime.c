/********************************************************************
	������
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include "FormMain.h"
#include "Lcm/Lcm.h"
#include "GlobalSetting.h"
#include "FormInfo.h"
#include "FormId.h"
#include "Key/Key.h"
#include "FormMenu.h"
#include "FormTemperature.h"
#include "GlobalSetting.h"
#include "FormTime.h"
#include "Rtc/Rtc.h"

static void Display(void);
static void Active(bool active);
static bool Event(u32 msg);
static void Create(u32 msg);

static const S_Form fmTime = 
{
	TIME_FORM_ID,				// ����ID��Ӧ����ȫ��Ψһ
	GUI_DELAY_COUNTER_S(30),	// ��ʱ�˳�ʱ�䣬0��ʾ����ʱ�˳�
	Create,					// ���崴��ʱ����
	NULL,					// �����˳�ʱ����
	Active,					// ���弤���¼�������true��ʾ���false��ʾ����Ǽ���״̬
	Event,					// �¼���Ӧ���簴�����Լ������û��Զ�����Ϣ
	Display					// ��ʾ����
};

const S_FormNode g_fmTime = 
{
	(PS_Form)&fmTime,	// ��Ӧ����
	(PS_FormNode)&g_fmMenu,		// ���˵�
	NULL,			// ����
	NULL,			// ��һ��
	NULL			// ��һ��
};

// �༭ģʽ
static bool m_flgEdit;
// �༭��ʱ��
static S_DateTime m_sTime;
// ��ǰ�༭��λ��0=��ʮλ��1=���λ��2=��ʮλ��3=�¸�λ��4=��ʮλ��5=���λ��6=Сʱʮλ��7=Сʱ��λ
// 8=����ʮλ��9=���Ӹ�λ��10=��ʮλ��11=���λ
static u8 m_bIndex;


// ��ʾʱ��
static void ShowTime(S_DateTime* time)
{
	char acTimeStr[12];
	sprintf(acTimeStr,"%04d-%02d-%02d",
		time->wYear,time->bMonth,time->bDay);
	LcmCenterString(13*2,acTimeStr);
	sprintf(acTimeStr,"%02d:%02d:%02d",
		time->bHour,time->bMinute,time->bSecond);
	LcmCenterString(13*3,acTimeStr);
}

// ��ʾ��ǰ����ֵ
static void ShowSetting(bool flgShow )
{
	ShowTime(&m_sTime);
// 	if (flgShow)
// 	{
// 		ShowTime(&m_sTime);
// 	}
// 	else
// 	{
// 		LcmClearRectangle(0,13*2,128,25);
// 	}
}

// �ػ�
static void Display(void)
{
	if (m_flgEdit)
	{
		// �༭ģʽ��Ҫ��˸
		u32 dwFlash = g_dwGuiCounter%GUI_DELAY_COUNTER_S(1);
		if (dwFlash==0)
		{
			ShowSetting(true);
		}
		else if (dwFlash == (500/GUI_TICK_CYCLE))
		{
			ShowSetting(true);
			// ��ɫ��ʾ
			FontMode(1,1);		// ����ģʽ������Ϊ��ɫ
			FontSet(0,0);		// ǰ��ɫΪ��ɫ
			if (m_bIndex<=5)
			{
				u32 index = m_bIndex>=4?m_bIndex+4:
					m_bIndex>=2?m_bIndex+3:m_bIndex+2;
				char acTimeStr[12];
				sprintf(acTimeStr,"%04d-%02d-%02d",
				m_sTime.wYear,m_sTime.bMonth,m_sTime.bDay);
				PutChar(24+7*index,13*2,acTimeStr[index]);
			}
			else
			{
				u32 index = m_bIndex>=10?m_bIndex-4:
					m_bIndex>=8?m_bIndex-5:m_bIndex-6;
				char acTimeStr[12];
				sprintf(acTimeStr,"%02d:%02d:%02d",
					m_sTime.bHour,m_sTime.bMinute,m_sTime.bSecond);
				PutChar(32+7*index,13*3,acTimeStr[index]);
			}
			FontMode(1,0);		// ����ģʽ������ɫΪ��ɫ
			FontSet(0,1);
		}
	}
	else
	{
		if (0 == g_dwGuiCounter%GUI_DELAY_COUNTER_S(1))
		{
			GetTime();
			ShowTime(&g_sNow);
		}
	}
}

static void Active(bool active)
{
	if (active)
	{
		ClrScreen();
		g_dwGuiCounter = 0;
		LcmCenterString(0,"ʱ������");
		GetTime();
		ShowTime(&g_sNow);
	}
}

static void Create(u32 msg)
{
	m_flgEdit = false;
	m_bIndex = 0;
}


static u32 GetSafeValue(u32 val,u32 maxVal,u32 minVal)
{
	if (val>maxVal)
	{
		return maxVal;
	}
	else if (val<minVal)
	{
		return minVal;
	}
	else
	{
		return val;
	}
}

// ���¼���ʱ�䣬����λ�����޶�����Ч��Χ��
void RecomputeTime()
{
	u8 maxDay=31;
	m_sTime.wYear = GetSafeValue(m_sTime.wYear,2099,2000);
	m_sTime.bMonth = GetSafeValue(m_sTime.bMonth,12,1);
	switch(m_sTime.bMonth)
	{
	case 2:		// ��Ҫ��������
		if ((0==(m_sTime.wYear%400))||
			((0==(m_sTime.wYear%4))&&
			(0!=(m_sTime.wYear%100)))
			)
		{
			maxDay = 29;
		}
		else
		{
			maxDay = 28;
		}
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		maxDay = 30;
		break;
	}
	m_sTime.bDay = GetSafeValue(m_sTime.bDay,maxDay,1);
	m_sTime.bHour = GetSafeValue(m_sTime.bHour,23,0);
	m_sTime.bMinute = GetSafeValue(m_sTime.bMinute,59,0);
	m_sTime.bSecond = GetSafeValue(m_sTime.bSecond,59,0);
}



static bool Event(u32 msg)
{
	s32 i;
	switch(msg)
	{
	case KEY_UP:
		i = 1;
		goto LabelAddTime;
	case KEY_DOWN:
		i = -1;
		goto LabelAddTime;
	case KEY_LEFT:
		if (m_flgEdit&&(m_bIndex!=0))
		{
			m_bIndex--;
			g_dwGuiCounter = 0;
			Display();
		}
		return true;
	case KEY_RIGHT:
		if (m_flgEdit&&(m_bIndex<11))
		{
			m_bIndex++;
			g_dwGuiCounter = 0;
			Display();
		}
		return true;
	case KEY_RETURN:
		if (m_flgEdit)
		{
			m_flgEdit = false;
			Active(true);
			return true;
		}
		else
		{
			return false;
		}
	case KEY_ENTER:
		if (!m_flgEdit)
		{
			m_flgEdit = true;
			g_dwGuiCounter = 0;
			m_bIndex = 0;
			m_sTime = g_sNow;
			Display();
		}
		else
		{
			g_sNow = m_sTime;
			SetTime();
			m_flgEdit = false;
			Active(true);
		}
		return true;
#if 0
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		i = msg - KEY_0;
		goto LabelSetTime;
#endif
	default:
		return false;
//		break;
	}
	return false;

LabelSetTime:
	if (m_flgEdit)
	{
		switch(m_bIndex)
		{
		case 0:
			m_sTime.wYear += (i-((m_sTime.wYear/10)%10))*10;
			break;
		case 1:
			m_sTime.wYear += (i-((m_sTime.wYear)%10));
			break;
		case 2:
			m_sTime.bMonth += (i-((m_sTime.bMonth/10)%10))*10;				
			break;
		case 3:
			m_sTime.bMonth += (i-((m_sTime.bMonth)%10));
			break;
		case 4:
			m_sTime.bDay += (i-((m_sTime.bDay/10)%10))*10;
			break;
		case 5:
			m_sTime.bDay += (i-((m_sTime.bDay)%10));
			break;
		case 6:
			m_sTime.bHour += (i-((m_sTime.bHour/10)%10))*10;
			break;
		case 7:
			m_sTime.bHour += (i-((m_sTime.bHour)%10));
			break;
		case 8:
			m_sTime.bMinute += (i-((m_sTime.bMinute/10)%10))*10;
			break;
		case 9:
			m_sTime.bMinute += (i-((m_sTime.bMinute)%10));
			break;
		case 10:
			m_sTime.bSecond += (i-((m_sTime.bSecond/10)%10))*10;
			break;
		case 11:
			m_sTime.bSecond += (i-((m_sTime.bSecond)%10));
			break;
		default:
			break;
		}
	}
	RecomputeTime();
	ShowTime(&m_sTime);
	Event(KEY_RIGHT);
	return true;

LabelAddTime:
	if (m_flgEdit)
	{
		switch(m_bIndex)
		{
		case 0:
			m_sTime.wYear += i*10;
			break;
		case 1:
			m_sTime.wYear += i;
			break;
		case 2:
			m_sTime.bMonth += i*10;				
			break;
		case 3:
			m_sTime.bMonth += i;
			break;
		case 4:
			m_sTime.bDay += i*10;
			break;
		case 5:
			m_sTime.bDay += i;
			break;
		case 6:
			m_sTime.bHour += i*10;
			break;
		case 7:
			m_sTime.bHour += i;
			break;
		case 8:
			m_sTime.bMinute += i*10;
			break;
		case 9:
			m_sTime.bMinute += i;
			break;
		case 10:
			m_sTime.bSecond += i*10;
			break;
		case 11:
			m_sTime.bSecond += i;
			break;
		default:
			break;
		}
		RecomputeTime();
		g_dwGuiCounter = 0;
		ShowTime(&m_sTime);
	}

	return true;
}

