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
#include "FormMain.h"
#include "FormTemperature.h"
#include "FormSmsNo.h"
#include "FormTime.h"
#include "FormSmsAlarm.h"
#include "FormLog.h"

static void Display(void);
static void Active(bool active);
static bool Event(u32 msg);
static void Create(u32 msg);

static const S_Form fmMenu = 
{
	MENU_FORM_ID,			// ����ID��Ӧ����ȫ��Ψһ
	GUI_DELAY_COUNTER_S(30),		// ��ʱ�˳�ʱ�䣬0��ʾ����ʱ�˳�
	Create,					// ���崴��ʱ����
	NULL,					// �����˳�ʱ����
	Active,					// ���弤���¼�������true��ʾ���false��ʾ����Ǽ���״̬
	Event,					// �¼���Ӧ���簴�����Լ������û��Զ�����Ϣ
	NULL	/*Display*/					// ��ʾ����
};

const S_FormNode g_fmMenu = 
{
	(PS_Form)&fmMenu,					// ��Ӧ����
	(PS_FormNode)&g_fmMain,		// ���˵�
	NULL,			// ����
	NULL,			// ��һ��
	NULL			// ��һ��
};

// �˵����Լ���Ӧ��ҳ��
typedef struct
{
	char* pIco;				// ͼ��
	char* pMsg;				// ��ʾ����
	PS_FormNode pForm;		// ��Ӧ��ҳ��
}S_MenuItem;

const S_MenuItem m_asMenu[] = 
{
	{NULL,"ϵͳ״̬",(PS_FormNode)&g_fmInfo},
//	{NULL,"��¼��ѯ",(PS_FormNode)&g_fmLog},
	{NULL,"���ź�������",(PS_FormNode)&g_fmSmsNo},
	{NULL,"���ű���������",(PS_FormNode)&g_fmSmsAlarm},
	{NULL,"ʱ������",(PS_FormNode)&g_fmTime},
	{NULL,"ˮ������",(PS_FormNode)&g_fmTemperature},
};

static u8 m_bIndex;		// �˵�����
#define MENU_LENGTH		sizeof(m_asMenu)/sizeof(S_MenuItem)
// �ػ�
static void Display(void)
{
	u8 top;		// ����
	u8 bottom;	// �ײ�
	int i;
	if (MENU_LENGTH<=5)
	{
		bottom = MENU_LENGTH-1;
	}
	else
	{
		bottom = min(m_bIndex+2,MENU_LENGTH-1);
		bottom = max(bottom,4);
	}
	top = bottom>=5 ? bottom-4:0;
	DbgPrintf("index=%d,\ttop=%d\tbottom=%d\r\n",m_bIndex,top,bottom);
	ClrScreen();
	for (i=top;i<=bottom;i++)
	{
		if (i==m_bIndex)
		{
			// ��ɫ��ʾ
			Rectangle(0,13*(i-top),127,13*(i-top)+11,1);
			FontMode(0,0);		// ������ģʽ
			FontSet(0,0);
			FontSet_cn(0,0);
			LcmPrintString(0,13*(i-top),m_asMenu[i].pMsg);
			FontMode(1,0);		// ����ģʽ������ɫΪ��ɫ
			FontSet(0,1);
			FontSet_cn(0,1);
		}
		else
		{
			// ������ʾ
			LcmPrintString(0,13*(i-top),m_asMenu[i].pMsg);
		}
	}
}

static void Active(bool active)
{
	if (active)
	{
		Display();
	}
}

static void Create(u32 msg)
{
//	m_bIndex = 0;
}

static bool Event(u32 msg)
{
	switch(msg)
	{
	case KEY_UP:
		if (m_bIndex!=0)
		{
			m_bIndex--;
			Display();
		}
		return true;
	case KEY_DOWN:
		if (m_bIndex<MENU_LENGTH-1)
		{
			m_bIndex++;
			Display();
		}
		return true;
	case KEY_ENTER:
		GuiNav(m_asMenu[m_bIndex].pForm);
		return true;
	default:
		return false;
		break;
	}
}

