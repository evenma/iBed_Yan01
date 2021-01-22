#include "Key.h"
#include "Alarm.h"

#define	HANG_MAX	5 		//按键扫描行数
#define	LIE_MAX		5 		//按键扫描列数

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

// 获取行列按键状态
// 输出参数已经经过整理
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
			if(0 != r)	// 保证只能有一次
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
			if(0 != c)	// 保证只能有一次
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

// 根据行列返回按键值
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
	u32 key;	// 物理键值
	E_KeyMessage msg;	// 对应的按键消息
}S_KeyMapping;

// 物理键值与按键消息关系映射表
static const S_KeyMapping C_adwKeyMapping[] = 
{
	{KEY_NULL,KEY_RELEASE},
	{16,KEY_SET_NUANFEN_SUB},
	{17,KEY_SET_NUANFEN_ADD},
	{18,KEY_SET_SHUIWEN},
	{11,KEY_GANZAO},                   	 // 暖风烘干
	{12,KEY_QINGXISHENGZHIQI},		  	 // 前部清洗
	{13,KEY_QINGXITUNBU},		  		 // 后部清洗
	{6,KEY_CHONGXIBIANPEN}, 			 // 冲洗马桶
	{8,KEY_CANCEL},					 	// 停止键或者取消动作命令
	{1,KEY_BIANMEN_KAI},				 // 便门开
	{3,KEY_BIANMEN_GUAN},				 // 便门关
	{0,KEY_BEIBAN_UP},					 // 背板升
	{4,KEY_BEIBAN_DOWN},				 // 背板降
	{5,KEY_TUIBU_UP}, 					 // 腿部升
	{9,KEY_TUIBU_DOWN},				 // 腿部降
//	{10,KEY_FANSHEN_ZUO_UP}, 			 // 左翻身升
//	{14,KEY_FANSHEN_YOU_UP},				 // 右翻身升
	{10,KEY_PAD_UP}, 			 // 液晶屏升
	{14,KEY_PAD_DOWN},				 // 液晶屏降
	{15,KEY_QIZUO}, 				 		 // 座椅姿势
	{2,KEY_XINZANGTANGWEI},			 // 心脏躺位
	{19,KEY_PINGTANG}, 				 	 // 平躺
};
/*
static const S_KeyMapping C_adwKeyMapping[] = 
{
	{KEY_NULL,KEY_RELEASE},
	{3,KEY_SET_NUANFEN_SUB},
	{2,KEY_SET_NUANFEN_ADD},
	{1,KEY_SET_SHUIWEN},
	{8,KEY_GANZAO},                   	 // 暖风烘干
	{7,KEY_QINGXISHENGZHIQI},		  	 // 前部清洗
	{6,KEY_QINGXITUNBU},		  		 // 后部清洗
	{13,KEY_CHONGXIBIANPEN}, 			 // 冲洗马桶
	{11,KEY_CANCEL},					 // 停止键或者取消动作命令
	{18,KEY_BIANMEN_KAI},				 // 便门开
	{16,KEY_BIANMEN_GUAN},				 // 便门关
	{19,KEY_BEIBAN_UP},					 // 背板升
	{15,KEY_BEIBAN_DOWN},				 // 背板降
	{14,KEY_TUIBU_UP}, 					 // 腿部升
	{10,KEY_TUIBU_DOWN},				 // 腿部降
	{9,KEY_FANSHEN_ZUO_UP}, 			 // 左翻身升
	{5,KEY_FANSHEN_YOU_UP},				 // 右翻身升
	{4,KEY_QIZUO}, 				 		 // 座椅姿势
	{17,KEY_XINZANGTANGWEI},			 // 心脏躺位
	{0,KEY_PINGTANG}, 				 	 // 平躺
};*/




// 物理键值转换为消息值，转换成功返回消息值，失败返回 KEY_NULL
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
static s32 m_dwLastKeyCode = KEY_NULL;	// 最后一次按键编码值
static bool m_flgKeyDown = false;		// 已经有按键按下事件发生

s32 g_KeyMsgDown = 0;	// 当前被按下的按键消息

// 定期调用，这里开始扫描
// 连续两次检测到同一个信号认为是按键按下，不支持组合键
void KeyTick()
{
	u32 row,col;
	s32 key;
	KeyCheck(&row,&col);
	key = GetKeyCode(row,col);
	if (key == m_dwLastKeyCode)		//  两次相同的按键值
	{
		if (!m_flgKeyDown)		// 该键还没有被按下过
		{
			m_flgKeyDown = true;
			DbgPrintf("按键被按下=%d\r\n",key);
			g_KeyMsgDown = Key2Msg(key);
			if (g_KeyMsgDown == 0)
			{
				DbgPrintf("没有找到对应的消息\r\n");
				return;
			}
			// 触发事件
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
		m_flgKeyDown = false;	// 键值发生了变化，重新允许发射消息
	}
	m_dwLastKeyCode = key;
}



/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

