/********************************************************************
	������
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include "FormMain.h"
#include "Lcm/Lcm.h"
//#include "FormInfo.h"
#include "FormId.h"
#include "Key/Key.h"
#include "FormMenu.h"
#include "CanAnalyzer.h"

static void Display(void);
static void Active(bool active);

static const S_Form fmMain = 
{
	MAIN_FORM_ID,			// ����ID��Ӧ����ȫ��Ψһ
	0,						// ��ʱ�˳�ʱ�䣬0��ʾ����ʱ�˳�
	NULL,					// ���崴��ʱ����
	NULL,					// �����˳�ʱ����
	Active,					// ���弤���¼�������true��ʾ���false��ʾ����Ǽ���״̬
	NULL	/*MainFormEvent*/,					// �¼���Ӧ���簴�����Լ������û��Զ�����Ϣ
	Display					// ��ʾ����
};

const S_FormNode g_fmMain = 
{
	(PS_Form)&fmMain,					// ��Ӧ����
	NULL,			// ���˵�
	(PS_FormNode)&g_fmMenu,			// ����
	NULL,			// ��һ��
	NULL			// ��һ��
};

//static u8 m_flgShuiWei;
// ��ӡˮ����Һλ
// ���� forceRefresh��true=����Һλ�Ƿ�仯��Ҫˢ��
static void ShowShuiXiangYeWei(bool forceRefresh)
{
	if (forceRefresh)
	{
		if (g_ZuobianSensor.ShuiXiangLow)
		{
			LcmClearRectangle(0,0,64,12);
			LcmPrintString(0,0,"��ˮλ");
		}
		else if(g_ZuobianSensor.ShuiXiangHigh)
		{
			LcmClearRectangle(0,0,64,12);
			LcmPrintString(0,0,"ˮ����");
		}
		else
		{
			LcmClearRectangle(0,0,64,12);
			LcmPrintString(0,0,"ˮλ����");			
		}
	}
}

static u16 m_flgWuWu;

// ��ӡ������Һλ
// ���� forceRefresh��true=����Һλ�Ƿ�仯��Ҫˢ��
static void ShowWuWuYeWei(bool forceRefresh )
{
	if (forceRefresh || ((g_ZuobianSensor.WuWuXiangZhongLiang/100)!=m_flgWuWu))
	{
		if (g_ZuobianSensor.WuWuXiangZhongLiang>=WU_WU_XIANG_BORDER)
		{
			LcmClearRectangle(64,0,64,12);
			LcmPrintString(64+16,0,"������");
		}
		else
		{
			LcmClearRectangle(64,0,64,12);
			LcmPrintString(64+16,0,"����������: %d  g",(u32)g_ZuobianSensor.WuWuXiangZhongLiang);
		}
	}
	m_flgWuWu = g_ZuobianSensor.WuWuXiangZhongLiang/100;
}


// ��ʾ��ǰ����
// ���� forceRefresh��true=����Һλ�Ƿ�仯��Ҫˢ��
static void ShowAction(bool forceRefresh )
{
#if 1
	if(forceRefresh)
	{
		LcmClearRectangle(0,20,128,12);
		LcmCenterString(20,"ǿӥ�Ƽ�");
	}
#else
	extern u8 m_actionCmd;
	char str[16];
	sprintf(str,"��ǰ����: %d",(u32)m_actionCmd);
	LcmClearRectangle(0,20,128,12);
	LcmCenterString(20,str);
#endif
}

// ��ʾ��С��״̬
// ���� forceRefresh��true=����Һλ�Ƿ�仯��Ҫˢ��
static void ShowDaXiaoBian(bool forceRefresh )
{
	static u8 last = 0;
#if 1
	if(!forceRefresh)
	{
		if(g_ZuobianSensor.DaBian)
		{
			if(last == 2)
			{
				return;
			}
			else
			{
				last = 2;
			}
		}
		else if(g_ZuobianSensor.ManXiaoBian||g_ZuobianSensor.WomenXiaoBian)
		{
			if(last==1)
			{
				return;
			}
			else
			{
				last = 1;
			}
		}
		else
		{
			if(last ==0)
			{
				return;
			}
			else
			{
				last = 0;
			}
		}
	}
	LcmClearRectangle(0,48,128,12);
	if(g_ZuobianSensor.DaBian)
	{
		LcmCenterString(48,"��⵽���");
	}
	else if(g_ZuobianSensor.ManXiaoBian||g_ZuobianSensor.WomenXiaoBian)
	{
		LcmCenterString(48,"��⵽С��");
	}

#else
	LcmClearRectangle(0,48,128,12);
	LcmPrintString(16,48,"������:%X %X",(u32)*(u8*)&g_ZuobianSensor,(u32)*(((u8*)&g_ZuobianSensor)+1));
#endif
}



static void Display()
{
	ShowWuWuYeWei(false);
	ShowShuiXiangYeWei(false);
	ShowAction(false);
	ShowDaXiaoBian(false);
}

static void Active(bool active)
{
	if (active)
	{
		ClrScreen();
		ShowWuWuYeWei(true);
		ShowShuiXiangYeWei(true);
		ShowAction(true);
		ShowDaXiaoBian(true);
	}
}

// ������ʾ�Ի���ÿ������ռ�� 16*12 �ռ�
void MessageBox(const char * strMsg,const char *caption)
{
	ClrScreen();
	LcmPrintString(0,0,caption);
	LcmCenterString(20,strMsg);
	GuiMessageBoxShow(GUI_DELAY_COUNTER_S(3));
};


bool MainFormEvent(u32 msg)
{
	return false;
}

