/********************************************************************
	��Ϣ����
*********************************************************************/
#include <stdio.h>
#include "FormMain.h"
#include "Lcm/Lcm.h"
#include "GlobalSetting.h"
#include "FormMain.h"
#include "FormInfo.h"
#include "FormId.h"
#include "FormMenu.h"
#include "Key/Key.h"
#include "CanAnalyzer.h"

static void Display(void);
static void Active(bool active);
static bool FormEvent(u32 msg);

#define ROW_HEIGH	13	// �и�

static const S_Form fmInfo = 
{
	INFO_FORM_ID,			// ����ID��Ӧ����ȫ��Ψһ
	GUI_DELAY_COUNTER_S(30),		// ��ʱ�˳�ʱ�䣬0��ʾ����ʱ�˳�
	NULL,					// ���崴��ʱ����
	NULL,					// �����˳�ʱ����
	Active,					// ���弤���¼�������true��ʾ���false��ʾ����Ǽ���״̬
	FormEvent,				// �¼���Ӧ���簴�����Լ������û��Զ�����Ϣ
	Display					// ��ʾ����
};

const S_FormNode g_fmInfo = 
{
	(PS_Form)&fmInfo,					// ��Ӧ����
	(PS_FormNode)&g_fmMenu,				// ���˵�
	NULL,			// ����
	NULL,			// ��һ��
	NULL			// ��һ��
};

// ��ӡˮ����Һλ
// ���� forceRefresh��true=����Һλ�Ƿ�仯��Ҫˢ��
static void ShowShuiXiangYeWei(bool forceRefresh )
{
	if ((!forceRefresh))
	{
		return;
	}
	// ˮ��ˮλ
	LcmClearRectangle(0,0,128,12);
	if (g_ZuobianSensor.ShuiXiangLow)
	{
		LcmPrintString(0,0,"��ˮλ");
	}
	else if(g_ZuobianSensor.ShuiXiangHigh)
	{
		LcmPrintString(0,0,"ˮ����");
	}
	else
	{
		LcmPrintString(0,0,"ˮλ����"); 		
	}
}

static u32 m_dwWuWuShuiWei;
// ��ӡ������Һλ
// ���� forceRefresh��true=����Һλ�Ƿ�仯��Ҫˢ��
static void ShowWuWuYeWei(bool forceRefresh )
{
	if ((!forceRefresh)&&(m_dwWuWuShuiWei == g_ZuobianSensor.WuWuXiangZhongLiang/100))
	{
		return;
	}
	// ˮ��ˮλ
	m_dwWuWuShuiWei = g_ZuobianSensor.WuWuXiangZhongLiang/100;
	LcmClearRectangle(0,ROW_HEIGH,128,12);
	LcmPrintString(0,ROW_HEIGH,"����������: %d	g",(u32)g_ZuobianSensor.WuWuXiangZhongLiang);
}

static s8 m_cReShuiWen;
static s8 m_cLengShuiWen;
// ��ʾˮ��
// ���� forceRefresh��true=����Һλ�Ƿ�仯��Ҫˢ��
static void ShowReShuiWen(bool forceRefresh )
{
	if ((!forceRefresh)&&(m_cReShuiWen == g_ZuobianSensor.ReShuiWenDu))
	{
		return;
	}
	// ˮ���¶�
	m_cReShuiWen = g_ZuobianSensor.ReShuiWenDu;
	LcmClearRectangle(0,ROW_HEIGH*2,128,12);
	LcmPrintString(0,ROW_HEIGH*2,"��ǰ��ˮˮ�£�%d��",(s32)m_cReShuiWen);
}

static void ShowLengShuiWen(bool forceRefresh )
{
	if ((!forceRefresh)&&(m_cLengShuiWen == g_ZuobianSensor.LengShuiWenDu))
	{
		return;
	}
	// ˮ���¶�
	m_cLengShuiWen = g_ZuobianSensor.LengShuiWenDu;
	LcmClearRectangle(0,ROW_HEIGH*3,128,12);
	LcmPrintString(0,ROW_HEIGH*3,"��ǰˮ��ˮ�£�%d��",(s32)m_cLengShuiWen);
}

static u32 m_cNuanFengWenDu;

static void ShowNuanFengWenDu(bool forceRefresh )
{
	if ((!forceRefresh)&&(m_cNuanFengWenDu == g_ZuobianSensor.ChuFengWenDu))
	{
		return;
	}
	//������¶�
	m_cNuanFengWenDu = g_ZuobianSensor.ChuFengWenDu;
	LcmClearRectangle(0,ROW_HEIGH*4,128,12);
	LcmPrintString(0,ROW_HEIGH*4,"��ǰ������¶ȣ�%d��",(s32)m_cNuanFengWenDu);
}

//-------------------- �ڶ�ҳ --------------------------

// ��ʾ���Ŵ�״̬
static void ShowBianMenDaKai(bool forceRefresh)
{
	static u8 bianmen=0;
	if ((!forceRefresh)&&(bianmen == g_ChuangTiSensor.BianMenKaiDaoDing))
	{
		return;
	}
	bianmen = g_ChuangTiSensor.BianMenKaiDaoDing;
	LcmClearRectangle(0,ROW_HEIGH*0,128,12);
	LcmPrintString(0,ROW_HEIGH*0,g_ChuangTiSensor.BianMenKaiDaoDing?"���Ŵ򿪵�λ":"����û�д򿪵�λ");
}

// ��ʾ���Źر�״̬
static void ShowBianMenGuanBi(bool forceRefresh)
{
	static u8 bianmen=0;
	if ((!forceRefresh)&&(bianmen == g_ChuangTiSensor.BianMenGuanDaoDi))
	{
		return;
	}
	bianmen = g_ChuangTiSensor.BianMenGuanDaoDi;
	LcmClearRectangle(0,ROW_HEIGH*1,128,12);
	LcmPrintString(0,ROW_HEIGH*1,g_ChuangTiSensor.BianMenGuanDaoDi?"���Źرյ�λ":"����û�йرյ�λ");
}

// ��ʾ���״��
static void ShowBatt(bool forceRefresh)
{
	if ((!forceRefresh))
	{
		return;
	}
	LcmClearRectangle(0,ROW_HEIGH*2,128,12);	
	if(!g_ChuangTiSensor.DianChiZhuOn)
	{
		LcmPrintString(0,ROW_HEIGH*2,"�޵��");
	}
	else
	{
		if(g_ChuangTiSensor.TingDian)
			LcmPrintString(0,ROW_HEIGH*2,"ͣ��");
		else
			if(g_ChuangTiSensor.DianLiangBuZu)
				LcmPrintString(0,ROW_HEIGH*2,"��ص�������");
	}
}

static u8 m_bPage = 0;
void ShowPage(bool forceRefresh)
{
	if (0==m_bPage)
	{
		ShowWuWuYeWei(forceRefresh);
		ShowShuiXiangYeWei(forceRefresh);
		ShowReShuiWen(forceRefresh);
		ShowLengShuiWen(forceRefresh);
		ShowNuanFengWenDu(forceRefresh);
	}
	else
	{
		ShowBianMenDaKai(forceRefresh);
		ShowBianMenGuanBi(forceRefresh);
		ShowBatt(forceRefresh);
	}
}

static void Display()
{
	ShowPage(false);
}



static void Active(bool active)
{
	if (active)
	{
		ClrScreen();
		ShowPage(true);
	}
}

static bool FormEvent(u32 msg)
{
	switch(msg)
	{
	case KEY_LEFT:
		if (m_bPage!=0)
		{
			m_bPage--;
		}
		else
		{
			m_bPage = 1;
		}
		ClrScreen();
		ShowPage(true);
		return true;
	case KEY_RIGHT:
		if (m_bPage!=1)
		{
			m_bPage++;
		}
		else
		{
			m_bPage = 0;
		}
		ClrScreen();
		ShowPage(true);
		return true;
	}
	return false;
}

