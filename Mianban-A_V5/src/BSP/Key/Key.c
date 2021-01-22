#include "Key.h"
#include "Alarm.h"

#define	HANG_MAX	5 		//����ɨ������
#define	LIE_MAX		5 		//����ɨ������

#define	LIE0		BIT8	
#define	LIE1		BIT9	
#define	LIE2		BIT10	
#define	LIE3		BIT11	
#define	LIE4		BIT12	
/*//#define	LIE5		BIT13	
//#define	LIE6		BIT15	
//#define	LIE7		BIT16	*/
//#define	LIE0		BIT12	
//#define	LIE1		BIT11	
//#define	LIE2		BIT10	
//#define	LIE3		BIT9	
//#define	LIE4		BIT8	
//#define	HANG0		BIT20	
//#define	HANG1		BIT21	
//#define	HANG2		BIT22	
//#define	HANG3		BIT23	
//#define	HANG4		BIT24	

#define	HANG0		BIT24	
#define	HANG1		BIT23	
#define	HANG2		BIT22	
#define	HANG3		BIT21	
#define	HANG4		BIT20	

#define	HANG		(HANG0|HANG1|HANG2|HANG3|HANG4)
#define	LIE			(LIE0|LIE1|LIE2|LIE3|LIE4)

#define	KEY_CTRL_HANG_IN()		IO1DIR &= ~HANG
#define	KEY_CTRL_HANG_OUT()		IO1DIR |= HANG
#define	KEY_CTRL_LIE_IN()			IO0DIR &= ~LIE
#define	KEY_CTRL_LIE_OUT()		IO0DIR |= LIE

#define	KEY_HANG_SET()		IO1SET = HANG
#define	KEY_HANG_CLR()		IO1CLR = HANG
#define	KEY_LIE_SET()			IO0SET = LIE
#define	KEY_LIE_CLR()			IO0CLR = LIE

#define GET_HANG()			((~IO1PIN) & HANG)
#define GET_LIE()			((~IO0PIN) & LIE)

#define KEY_NULL	-1

void KeyInit(void)
{
	KEY_CTRL_HANG_IN();
	KEY_CTRL_LIE_IN();
}

// ��ȡ���а���״̬
// ��������Ѿ���������
const u32 C_adwHangList[] = 
{
	HANG0,
	HANG1,
	HANG2,
	HANG3,
	HANG4,
};
const u32 C_adwLieList[] = 
{
	LIE0,
	LIE1,
	LIE2,
	LIE3,
	LIE4,
};

static void KeyCheck(u32* row,u32* col)
{
	u32 rowData,colData;
	u32 i;
	u32 r=0,c=0;
	KEY_CTRL_LIE_OUT();
	KEY_LIE_CLR();
	DelayXus(20);
	rowData =  GET_HANG();
//	DbgPrintf("rowData=%x\r\n",rowData);
//	KEY_LIE_SET();
	KEY_CTRL_LIE_IN();
	if(0==rowData)
	{
		*row = 0;
		*col = 0;
		return;
	}
	for(i=0;i<HANG_MAX;i++)
	{
		if (rowData&C_adwHangList[i])
		{
			if(0 != r)	// ��ֻ֤����һ��
			{
				*row = 0;
				*col = 0;
				return;
			}
			r |= 1ul<<i;
		}
	}

	KEY_CTRL_HANG_OUT();
	KEY_HANG_CLR();
	DelayXus(20);
	colData = GET_LIE();
//	DbgPrintf("colData=%x\r\n",colData);
//	KEY_HANG_SET();
	KEY_CTRL_HANG_IN();
	for(i=0;i<LIE_MAX;i++)
	{
		if (colData&C_adwLieList[i])
		{
			if(0 != c)	// ��ֻ֤����һ��
			{
				*row = 0;
				*col = 0;
				return;
			}
			c |= 1ul<<i;
		}
	}
	*row = r;
	*col = c;
}

// �������з��ذ���ֵ
u32 GetKeyCode(u32 row,u32 col)
{
	u32 r,c;
	if((0==col)||(0==row))
	{
		return KEY_NULL;
	}
	for (r=0;r<HANG_MAX;r++)
	{
		if (row&(1ul<<r))
		{
			break;
		}
	}
	for (c=0;c<LIE_MAX;c++)
	{
		if (col&(1ul<<c))
		{
			break;
		}
	}
	return (r * LIE_MAX) + c;
}

typedef struct
{
	u32 key;	// �����ֵ
	E_KeyMessage msg;	// ��Ӧ�İ�����Ϣ
}S_KeyMapping;

// �����ֵ�밴����Ϣ��ϵӳ���
static const S_KeyMapping C_adwKeyMapping[] = 
{
	{KEY_NULL,KEY_RELEASE},
	{16,KEY_SET_NUANFEN_SUB},
	{17,KEY_SET_NUANFEN_ADD},
	{18,KEY_SET_SHUIWEN},
	{11,KEY_GANZAO},                   	 // ů����
	{12,KEY_QINGXISHENGZHIQI},		  	 // ǰ����ϴ
	{13,KEY_QINGXITUNBU},		  		 // ����ϴ
	{6,KEY_CHONGXIBIANPEN}, 			 // ��ϴ��Ͱ
	{8,KEY_CANCEL},					 	// ֹͣ������ȡ����������
	{1,KEY_BIANMEN_KAI},				 // ���ſ�
	{3,KEY_BIANMEN_GUAN},				 // ���Ź�
	{0,KEY_BEIBAN_UP},					 // ������
	{4,KEY_BEIBAN_DOWN},				 // ���彵
	{5,KEY_TUIBU_UP}, 					 // �Ȳ���
	{9,KEY_TUIBU_DOWN},				 // �Ȳ���
//	{10,KEY_FANSHEN_ZUO_UP}, 			 // ������
//	{14,KEY_FANSHEN_YOU_UP},				 // �ҷ�����
	{10,KEY_PAD_UP}, 			 // Һ������
	{14,KEY_PAD_DOWN},				 // Һ������
	{15,KEY_QIZUO}, 				 		 // ��������
	{2,KEY_XINZANGTANGWEI},			 // ������λ
	{19,KEY_PINGTANG}, 				 	 // ƽ��
};
/*
static const S_KeyMapping C_adwKeyMapping[] = 
{
	{KEY_NULL,KEY_RELEASE},
	{3,KEY_SET_NUANFEN_SUB},
	{2,KEY_SET_NUANFEN_ADD},
	{1,KEY_SET_SHUIWEN},
	{8,KEY_GANZAO},                   	 // ů����
	{7,KEY_QINGXISHENGZHIQI},		  	 // ǰ����ϴ
	{6,KEY_QINGXITUNBU},		  		 // ����ϴ
	{13,KEY_CHONGXIBIANPEN}, 			 // ��ϴ��Ͱ
	{11,KEY_CANCEL},					 // ֹͣ������ȡ����������
	{18,KEY_BIANMEN_KAI},				 // ���ſ�
	{16,KEY_BIANMEN_GUAN},				 // ���Ź�
	{19,KEY_BEIBAN_UP},					 // ������
	{15,KEY_BEIBAN_DOWN},				 // ���彵
	{14,KEY_TUIBU_UP}, 					 // �Ȳ���
	{10,KEY_TUIBU_DOWN},				 // �Ȳ���
	{9,KEY_FANSHEN_ZUO_UP}, 			 // ������
	{5,KEY_FANSHEN_YOU_UP},				 // �ҷ�����
	{4,KEY_QIZUO}, 				 		 // ��������
	{17,KEY_XINZANGTANGWEI},			 // ������λ
	{0,KEY_PINGTANG}, 				 	 // ƽ��
};*/




// �����ֵת��Ϊ��Ϣֵ��ת���ɹ�������Ϣֵ��ʧ�ܷ��� KEY_NULL
u32 Key2Msg(u32 key)
{
	u32 i;
	for (i=0;i<sizeof(C_adwKeyMapping)/sizeof(S_KeyMapping);i++)
	{
		if (C_adwKeyMapping[i].key==key)
		{
			return C_adwKeyMapping[i].msg;
		}
	}
	return 0;
}
static s32 m_dwLastKeyCode = KEY_NULL;	// ���һ�ΰ�������ֵ
static bool m_flgKeyDown = false;		// �Ѿ��а��������¼�����

s32 g_KeyMsgDown = 0;	// ��ǰ�����µİ�����Ϣ

// ���ڵ��ã����￪ʼɨ��
// �������μ�⵽ͬһ���ź���Ϊ�ǰ������£���֧����ϼ�
void KeyTick()
{
	u32 row,col;
	s32 key;
	KeyCheck(&row,&col);
	key = GetKeyCode(row,col);
	if (key == m_dwLastKeyCode)		//  ������ͬ�İ���ֵ
	{
		if (!m_flgKeyDown)		// �ü���û�б����¹�
		{
			m_flgKeyDown = true;
			DbgPrintf("����������=%d\r\n",key);
			g_KeyMsgDown = Key2Msg(key);
			if (g_KeyMsgDown == 0)
			{
				DbgPrintf("û���ҵ���Ӧ����Ϣ\r\n");
				return;
			}
			// �����¼�
			if(KEY_RELEASE!=g_KeyMsgDown)
			{
				SetBackLight(64);			
				ALARM_BUZZER(1);
				rt_thread_delay(RT_TICK_PER_SECOND*30/1000);
				ALARM_BUZZER(0);
			}
			GuiEventHandle(g_KeyMsgDown);
		}
	}
	else
	{
		g_KeyMsgDown = 0;
		m_flgKeyDown = false;	// ��ֵ�����˱仯��������������Ϣ
	}
	m_dwLastKeyCode = key;
}



/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

