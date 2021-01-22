/********************************************************************
	������
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include "FormMain.h"
#include "../Lcm/Lcm.h"
#include "../GlobalSetting.h"
#include "../Sensor/Sensor.h"
#include "FormInfo.h"
#include "FormId.h"
#include "../Key/Key.h"
#include "FormMenu.h"
#include "FormMain.h"
#include "FormTemperature.h"
#include "FormSmsNo.h"
#include "FormTime.h"
#include "FormLog.h"
#include "../GlobalSetting.h"

static void Display();
static void Active(bool active);
static bool Event(DWORD msg);

static const S_Form fmLog = 
{
	LOG_FORM_ID,			// ����ID��Ӧ����ȫ��Ψһ
	GUI_DELAY_COUNTER_S(30),	// ��ʱ�˳�ʱ�䣬0��ʾ����ʱ�˳�
	NULL,					// ���崴��ʱ����
	NULL,					// �����˳�ʱ����
	Active,					// ���弤���¼�������true��ʾ���false��ʾ����Ǽ���״̬
	Event,					// �¼���Ӧ���簴�����Լ������û��Զ�����Ϣ
	NULL	/*Display*/					// ��ʾ����
};

const S_FormNode g_fmLog = 
{
	(PS_Form)&fmLog,					// ��Ӧ����
	(PS_FormNode)&g_fmMenu,		// ���˵�
	NULL,			// ����
	NULL,			// ��һ��
	NULL			// ��һ��
};

static S_DaXiaoBianJiLu* m_psCurrentJiLu;		// ��ǰ��¼ָ��

// �ػ�
static void Display()
{
	DbgPrintf("��¼��ַ:%x\r\n",m_psCurrentJiLu);

	ClrScreen();
	LcmCenterString(0,"��¼��ѯ");
	// ��ʾ4����¼
	S_DaXiaoBianJiLu *jilu = m_psCurrentJiLu;
	for (int i=0;i<4;i++)
	{
		if (NULL == jilu)
		{
			return;
		}
		char buf[20];
		sprintf(buf,"%04d-%02d-%02d %02d:%02d%s",
			jilu->wYear,jilu->bMonth,jilu->bDay,jilu->bHour,jilu->bMinute,jilu->bEvent?"��":"С");
		LcmPrintString(0,13*i+13,buf);
		jilu = GetPreviousJiLu(jilu);
	}
}

static void Active(bool active)
{
	if (active)
	{
		DbgPrintf("��¼����:%d\r\n",GetJiLuZongShu());
		m_psCurrentJiLu = GetLastJiLu();
		Display();
	}
}

static bool Event(DWORD msg)
{
	switch(msg)
	{
	case KEY_UP:	// ������һҳ
		m_psCurrentJiLu = GetFirstJiLu();
		for (int i=0;i<3;i++)	// ��ǰ��3��
		{
			S_DaXiaoBianJiLu* jilu = GetNextJiLu(m_psCurrentJiLu);
			if (NULL == jilu)
			{
				break;
			}
			else
			{
				m_psCurrentJiLu = jilu;
			}
		}
		Display();
		return true;
	case KEY_DOWN:	// �������һҳ
		Active(true);
		return true;
	case KEY_ENTER:	// ����ɾ�����棨�ݲ��ṩɾ�����ܣ�
		return true;
	case KEY_LEFT:	// ��һҳ
		if (m_psCurrentJiLu==NULL)
		{
			Active(true);
		}
		else
		{
			S_DaXiaoBianJiLu* jilu = m_psCurrentJiLu;
			for (int i=0;i<4;i++)	// ��ǰ4��
			{
				jilu = GetPreviousJiLu(jilu);
				if (NULL == jilu)	// �Ѿ��������򲻼�����ҳ
				{
					return true;
				}
			}
			m_psCurrentJiLu = jilu;
			Display();
		}
		return true;
	case KEY_RIGHT:	// ��һҳ
		if (m_psCurrentJiLu==NULL)
		{
			Active(true);
		}
		else
		{
			for (int i=0;i<4;i++)	// ����4��
			{
				S_DaXiaoBianJiLu* jilu = GetNextJiLu(m_psCurrentJiLu);
				if (NULL == jilu)
				{
					break;
				}
				else
				{
					m_psCurrentJiLu = jilu;
				}
			}
			Display();
		}
		return true;
	default:
		return false;
		break;
	}
}

