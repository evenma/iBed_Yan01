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


static void Display(void);
static void Active(bool active);
static bool Event(u32 msg);
static void Create(u32 msg);

static const S_Form fmTemperature = 
{
	TEMPERATURE_FORM_ID,			// ����ID��Ӧ����ȫ��Ψһ
	GUI_DELAY_COUNTER_S(30),		// ��ʱ�˳�ʱ�䣬0��ʾ����ʱ�˳�
	Create,					// ���崴��ʱ����
	NULL,					// �����˳�ʱ����
	Active,					// ���弤���¼�������true��ʾ���false��ʾ����Ǽ���״̬
	Event,					// �¼���Ӧ���簴�����Լ������û��Զ�����Ϣ
	Display					// ��ʾ����
};

const S_FormNode g_fmTemperature = 
{
	(PS_Form)&fmTemperature,	// ��Ӧ����
	(PS_FormNode)&g_fmMenu,		// ���˵�
	NULL,			// ����
	NULL,			// ��һ��
	NULL			// ��һ��
};

// �༭ģʽ
static bool m_flgEdit;
// �༭��ˮ��
static u8 m_bTemperature;
// ��ǰ�༭��λ��0=��λ��1=ʮλ����
static u8 m_bIndex;

// ��ʾ��ǰ����ֵ
static void ShowSetting(bool flgShow)
{
	if (flgShow)
	{
		LcmPrintString(0,13*2,"�趨�¶ȣ�%d��",m_bTemperature);
	}
	else
	{
		LcmClearRectangle(0,13*2,128,12);
	}
}

// �ػ�
static void Display()
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
			if (m_bIndex)
			{
				PutChar(5*12+5,13*2,'0'+(m_bTemperature/10));
			}
			else
			{
				PutChar(5*12+12,13*2,'0'+(m_bTemperature%10));
			}
			FontMode(1,0);		// ����ģʽ������ɫΪ��ɫ
			FontSet(0,1);
		}
	}
}

static void Active(bool active)
{
	if (active)
	{
		ClrScreen();
		g_dwGuiCounter = 0;
		LcmCenterString(0,"ˮ������");
		// ��ʾ��ǰ���õ��¶�
		LcmPrintString(0,13*1,"��ǰ�¶ȣ�%d��",g_sGlobalSetting.ToiletCfg.ShuiWen);
		if (m_flgEdit)
		{
			ShowSetting(true);
		}
	}
}

static void Create(u32 msg)
{
	m_flgEdit = false;
	m_bIndex = 0;
}

#define MAX_TEMPERATURE	40
#define MIN_TEMPERATURE	30
static void SetTemperature(u8 temp)
{
	if (temp > MAX_TEMPERATURE)
	{
		m_bTemperature = MAX_TEMPERATURE;
	}
	else if (temp < MIN_TEMPERATURE)
	{
		m_bTemperature = MIN_TEMPERATURE;
	}
	else
	{
		m_bTemperature = temp;
	}
}

static bool Event(u32 msg)
{
	switch(msg)
	{
	case KEY_UP:
		if (m_flgEdit)
		{
			if (m_bIndex==0)
			{
				SetTemperature(m_bTemperature + 1);
			}
			else
			{
				SetTemperature(m_bTemperature + 10);
			}
			g_dwGuiCounter = 0;
			ShowSetting(true);
		}
		return true;
	case KEY_DOWN:
		if (m_flgEdit)
		{
			if (m_bIndex==0)
			{
				SetTemperature(m_bTemperature - 1);
			}
			else
			{
				SetTemperature(m_bTemperature - 10);
			}
			g_dwGuiCounter = 0;
			ShowSetting(true);
		}
		return true;
	case KEY_LEFT:
	case KEY_RIGHT:
		if (m_flgEdit)
		{
			m_bIndex = (m_bIndex+1)%2;
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
			SetTemperature(g_sGlobalSetting.ToiletCfg.ShuiWen);
			Display();
		}
		else
		{
			// ���ﱣ�������flash��
			if (g_sGlobalSetting.ToiletCfg.ShuiWen != m_bTemperature)
			{
				g_sGlobalSetting.ToiletCfg.ShuiWen = m_bTemperature;
				SaveGlobalSeeting();
			}
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
		if (m_flgEdit)
		{
			if (m_bIndex)
			{
				SetTemperature((m_bTemperature%10)+10*(msg-KEY_0));
			}
			else
			{
				SetTemperature(10*(m_bTemperature/10)+(msg-KEY_0));
			}
			ShowSetting();
		}
		return true;
#endif
	default:
		return false;
		break;
	}
}

