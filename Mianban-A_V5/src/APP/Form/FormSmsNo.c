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
#include "FormSmsNo.h"

static void Display(void);
static void Active(bool active);
static bool Event(u32 msg);
static void Create(u32 msg);

static const S_Form fmSmsNo = 
{
	SMS_NO_FORM_ID,			// ����ID��Ӧ����ȫ��Ψһ
	GUI_DELAY_COUNTER_S(30),		// ��ʱ�˳�ʱ�䣬0��ʾ����ʱ�˳�
	Create,					// ���崴��ʱ����
	NULL,					// �����˳�ʱ����
	Active,					// ���弤���¼�������true��ʾ���false��ʾ����Ǽ���״̬
	Event,					// �¼���Ӧ���簴�����Լ������û��Զ�����Ϣ
	Display					// ��ʾ����
};

const S_FormNode g_fmSmsNo = 
{
	(PS_Form)&fmSmsNo,	// ��Ӧ����
	(PS_FormNode)&g_fmMenu,		// ���˵�
	NULL,			// ����
	NULL,			// ��һ��
	NULL			// ��һ��
};

// �༭ģʽ
static bool m_flgEdit;
// �༭�ĺ���
static char m_acNo[MSG_NUMBER_MAX_LENGTH+1];
// ��ǰ�༭��λ��0=��һλ��1=�ڶ�λ������
static u8 m_bIndex;

// ��ʾ��ǰ����ֵ
static void ShowSetting(bool flgShow)
{
	if (flgShow)
	{
		LcmPrintString(0,13*3,"�趨���룺");
		LcmClearRectangle(12,13*4,128-12,10);
		LcmPrintString(12,13*4,m_acNo);
		if (strlen(m_acNo)==m_bIndex)
		{
			PutChar(12+7*m_bIndex,13*4,'_');
		}
	}
	else
	{
		LcmClearRectangle(0,13*3,128,25);
	}
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
			if (m_acNo[m_bIndex])
			{
				PutChar(12+7*m_bIndex,13*4,m_acNo[m_bIndex]);
			}
			else
			{
				PutChar(12+7*m_bIndex,13*4,'_');
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
		LcmCenterString(0,"���ź�������");
		// ��ʾ��ǰ���õ��¶�
		LcmPrintString(0,13*1,"��ǰ���룺");
		LcmPrintString(12,13*2,g_sGlobalSetting.acMsgNumber);
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

static bool Event(u32 msg)
{
	switch(msg)
	{
	case KEY_UP:
		if (m_flgEdit)
		{
			if(m_acNo[m_bIndex]=='9')
			{
				m_acNo[m_bIndex]=0;
			}
			else if(m_acNo[m_bIndex]==0)
			{
				m_acNo[m_bIndex]='0';
			}
			else 
			{
				m_acNo[m_bIndex]++;
			}
			g_dwGuiCounter = 0;
			Display();
		}
		return true;
	case KEY_DOWN:
		if (m_flgEdit)
		{
			if(m_acNo[m_bIndex]=='0')
			{
				m_acNo[m_bIndex]=0;
			}
			else if(m_acNo[m_bIndex]==0)
			{
				m_acNo[m_bIndex]='9';
			}
			else 
			{
				m_acNo[m_bIndex]--;
			}
			g_dwGuiCounter = 0;
			Display();
		}
		return true;
	case KEY_LEFT:
		if (m_flgEdit&&(m_bIndex!=0))
		{
			m_bIndex--;
			g_dwGuiCounter = 0;
			Display();
		}
		return true;
	case KEY_RIGHT:
		if (m_flgEdit)
		{
			u32 len = strlen(m_acNo);
			len = min(len,MSG_NUMBER_MAX_LENGTH-1);		// len��1��ʼ��index��0��ʼ��������Ҫ��1
			if (m_bIndex<len)
			{
				m_bIndex++;
				g_dwGuiCounter = 0;
				Display();
			}
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
//			memcpy(m_acNo,g_sGlobalSetting.acMsgNumber,sizeof(g_sGlobalSetting.acMsgNumber));
			memset(m_acNo,0,sizeof(m_acNo));
			Display();
		}
		else
		{
			// ���ﱣ�������flash��
			if (0!=strcmp(m_acNo,g_sGlobalSetting.acMsgNumber))
			{
				memcpy(g_sGlobalSetting.acMsgNumber,m_acNo,sizeof(g_sGlobalSetting.acMsgNumber));
				SaveGlobalSeeting();
			}
			m_flgEdit = false;
			Active(true);
		}
		return true;
	default:
		return false;
//		break;
	}
}

