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
#include "FormSmsAlarm.h"

static void Display(void);
static void Active(bool active);
static bool Event(u32 msg);
static void Create(u32 msg);

static const S_Form fmSmsAlarm = 
{
	SMS_ALARM_FORM_ID,			// ����ID��Ӧ����ȫ��Ψһ
	GUI_DELAY_COUNTER_S(30),		// ��ʱ�˳�ʱ�䣬0��ʾ����ʱ�˳�
	Create,					// ���崴��ʱ����
	NULL,					// �����˳�ʱ����
	Active,					// ���弤���¼�������true��ʾ���false��ʾ����Ǽ���״̬
	Event,					// �¼���Ӧ���簴�����Լ������û��Զ�����Ϣ
	NULL/*Display*/					// ��ʾ����
};

const S_FormNode g_fmSmsAlarm = 
{
	(PS_Form)&fmSmsAlarm,	// ��Ӧ����
	(PS_FormNode)&g_fmMenu,		// ���˵�
	NULL,			// ����
	NULL,			// ��һ��
	NULL			// ��һ��
};

// �༭ģʽ
static bool m_flgEdit;
// ������Ϣ
static bool m_aflgSetting[4];
static const char *C_aTitle[4] = {
	"ˮλ����",
	"��������",
	"����¼�",
	"С���¼�"
};
// ��ǰ�༭����
static u8 m_bIndex;


// �ػ�
static void Display(void)
{
	char acBuf[20];
	int i;
	ClrScreen();
	LcmCenterString(0,"���ű���������");
	// ��ʾ������Ŀ
	for (i =0;i<4;i++)
	{
		if (m_aflgSetting[i])
		{
			strcpy(acBuf,"��");
		}
		else
		{
			strcpy(acBuf,"��");
		}
		strcpy(acBuf+2,C_aTitle[i]);

		if (m_flgEdit&&(i==m_bIndex))
		{
			// ��ɫ��ʾ
			Rectangle(0,13*(i+1),127,13*(i+1)+11,1);
			FontMode(0,0);		// ������ģʽ
			FontSet(0,0);
			FontSet_cn(0,0);
			LcmPrintString(0,13*(i+1),acBuf);
			FontMode(1,0);		// ����ģʽ������ɫΪ��ɫ
			FontSet(0,1);
			FontSet_cn(0,1);
		}
		else
		{
			// ������ʾ
			LcmPrintString(0,13*(i+1),acBuf);
		}
	}
}

static void Active(bool active)
{
	if (active)
	{
		m_aflgSetting[0] = g_sGlobalSetting.ToiletCfg.AlarmShuiWeiBuZuEn;
		m_aflgSetting[1] = g_sGlobalSetting.ToiletCfg.AlarmChuBianXiangManEn;
		m_aflgSetting[2] = g_sGlobalSetting.ToiletCfg.AlarmXiaoBianEn;
		m_aflgSetting[3] = g_sGlobalSetting.ToiletCfg.AlarmDaBianEn;
		Display();
	}
}

static void Create(u32 msg)
{
	m_flgEdit = false;
	m_bIndex = 0;
}


static bool Event(u32 msg)
{
	switch(msg)
	{
	case KEY_UP:
		if (m_flgEdit&&(m_bIndex!=0))
		{
			m_bIndex--;
			Display();
		}
		return true;
	case KEY_DOWN:
		if (m_flgEdit&&(m_bIndex<3))
		{
			m_bIndex++;
			Display();
		}
		return true;
	case KEY_LEFT:
	case KEY_RIGHT:
		// ������Ӧ��������
		if (m_flgEdit)
		{
			m_aflgSetting[m_bIndex] = !m_aflgSetting[m_bIndex];
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
			m_bIndex = 0;
			Display();
		}
		else
		{
			g_sGlobalSetting.ToiletCfg.AlarmShuiWeiBuZuEn = m_aflgSetting[0];
			g_sGlobalSetting.ToiletCfg.AlarmChuBianXiangManEn = m_aflgSetting[1];
			g_sGlobalSetting.ToiletCfg.AlarmXiaoBianEn = m_aflgSetting[2];
			g_sGlobalSetting.ToiletCfg.AlarmDaBianEn = m_aflgSetting[3];
			SaveGlobalSeeting();
			m_flgEdit = false;
			Active(true);
		}
		return true;
	default:
		return false;
//		break;
	}
}

