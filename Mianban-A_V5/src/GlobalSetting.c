// ȫ�����ñ�������Щ������Ҫ���浽 EEPROM ��
#include "Common.h"
#include "GlobalSetting.h"
#include "Iap/Iap.h"
#include <string.h>
#include "Rtc/Rtc.h"
#include "rtthread.h"
#include <rthw.h>

S_GlobalSetting g_sGlobalSetting;	// ȫ�ֱ���
S_ZuobianqiSetA g_ZuobianqiSetA;
S_ZuobianqiSetB g_ZuobianqiSetB;

// Flash�洢�ռ����
// 12��0X0C����������ַ 0X0001 8000-9FFF����8KB	��	���ڼ�¼��С�㣬��13�����γ�ƹ��
// 13��0X0D����������ַ 0X0001 A000-BFFF����8KB	��	���ڼ�¼��С�㣬��12�����γ�ƹ��
// 14��0X0E����������ַ 0X0001 C000-DFFF����8KB	��	���ڴ洢�û�������Ϣ

#define SECTION_SIZE	(8*1024)
#define SECTION_LOG_ADDR_1	0X00018000
#define SECTION_LOG_ADDR_2	0X0001A000

void SetDefault(void)
{
	g_ZuobianqiSetA.ShuiWenGaoDi=0x02;     // Ĭ������Ϊ37��
	g_ZuobianqiSetA.NuanFengQiangRuo=0x03; // Ĭ������Ϊ�ߵ�λ
	g_ZuobianqiSetA.QiangXiQiangRuo=0x03;  // Ĭ������Ϊ�ߵ�λ
	g_ZuobianqiSetA.HoldOn= 0;			   // Ĭ�ϲ��������п�����������һ��
}

// �û�������Ϣ�洢��ʽ��
// ʹ��256�ֽڣ�64�֣����ȵĴ洢�ռ䣬ǰ��252�ֽ���Ϊ��Ч���ݣ����һ������ΪУ��ͣ�ǰ�������ֽڵĺͣ�ע�ⲻ���֣�

// �� EEPROM ������ȫ�ֱ���
void LoadGlobalSetting()
{
	u8* pFlash = (u8*)0X0001C000;
	u32 sum = 0;
	int i;
	for (i=0;i<256-4;i++)
	{
		sum += *pFlash++;
	}
	if (sum==*(u32*)(0X0001C000+252))
	{
		g_sGlobalSetting = *(S_GlobalSetting*)0X0001C000;
		// ���ź�������
		g_sGlobalSetting.acMsgNumber[15] = 0;
	}
	else
	{
		// У�鲻��ȷ��ʹ��Ĭ��ֵ
		memset(&g_sGlobalSetting,0,sizeof(g_sGlobalSetting));
	}
}

static u8 buf[512];
// ��ȫ�ֱ�������� EEPROM ��
void SaveGlobalSeeting()
{
	u32 sum = 0;
	int i;
	rt_base_t level;
	memset(buf,0xff,512);
	*(S_GlobalSetting*)buf = g_sGlobalSetting;
	for (i=0;i<256-4;i++)
	{
		sum += buf[i];
	}
	*(u32*)(buf+252) = sum;
	// ����д��ʱӦ�ر��ж�
	level = rt_hw_interrupt_disable();
	SelSector(14,14);
	EraseSector(14,14);
	SelSector(14,14);
	RamToFlash(0X0001C000,(u32)buf,512);	// һ��������д��512�ֽ�
	rt_hw_interrupt_enable(level);
}

/*
	�洢��ʽ���¼���ƹ�ҷ�ʽ���д洢
		0-3�ֽڣ�������ţ������鱻ʹ�ú��Զ���1
		4-7�ֽڣ�������ȫ0XFF
		8-8,191�ֽڣ���8�ֽ�Ϊ��λ�������¼��Ϣ
*/


// ��С���¼
//	���� bDaXiaoBian 1=��㣬0=С��
void DaXiaoBianJiLu(u8 bDaXiaoBian)
{
	u32 dwAddr;
	S_DaXiaoBianJiLu jilu;
	int i;
	rt_base_t level;

	DbgPrintf("��С���¼���¼�=%d\r\n",bDaXiaoBian);
	GetTime();
	jilu = *(S_DaXiaoBianJiLu*)&g_sNow;
	jilu.bEvent = bDaXiaoBian;
	// ���浽 Flash ��
	// 1�����ҵ�����
	if (0xFFFFFFFF==*(u32*)SECTION_LOG_ADDR_2)
	{
		dwAddr = SECTION_LOG_ADDR_1;
	}
	else if (0xFFFFFFFF==*(u32*)SECTION_LOG_ADDR_1)
	{
		dwAddr = SECTION_LOG_ADDR_2;
	}
	else if (*(u32*)SECTION_LOG_ADDR_2 > *(u32*)SECTION_LOG_ADDR_1)
	{
		dwAddr = SECTION_LOG_ADDR_2;			
	}
	else
	{
		dwAddr = SECTION_LOG_ADDR_1;
	}
	// 2���ҵ����һ����¼��λ��
	for (i=0;i<SECTION_SIZE/sizeof(S_DaXiaoBianJiLu);i++)
	{
		if (0xFFFFFFFF == *(u32*)dwAddr)
		{
			break;
		}
		else
		{
			dwAddr += sizeof(S_DaXiaoBianJiLu);
		}
	}
	// 3���жϸü�¼�Ƿ��Ѿ���������ѵ�������л���ǰ�����鲢��ǰ��������в�����д������Լ���¼
	if (i == SECTION_SIZE/sizeof(S_DaXiaoBianJiLu))	// ˵����ǰ������������Ҫ�л����µ�����
	{
		u32 serialNo ;
		dwAddr -= SECTION_SIZE;
		// ����Ҫ�������к�
		serialNo = *(u32*)dwAddr + 1;
		// �л�����һ����
		if (dwAddr == SECTION_LOG_ADDR_1)
		{
			dwAddr = SECTION_LOG_ADDR_2;
		}
		else
		{
			dwAddr = SECTION_LOG_ADDR_1;
		}
		if (0xFFFFFFFF != *(u32*)dwAddr)
		{
			level = rt_hw_interrupt_disable();
			SelSector(dwAddr/SECTION_SIZE,dwAddr/SECTION_SIZE);
			EraseSector(dwAddr/SECTION_SIZE,dwAddr/SECTION_SIZE);
			rt_hw_interrupt_enable(level);
		}
		memset(buf,0xff,sizeof(buf));
		*(u32*)(buf) = serialNo;
		*(S_DaXiaoBianJiLu*)(buf+sizeof(S_DaXiaoBianJiLu)) = jilu;
		level = rt_hw_interrupt_disable();
		SelSector(dwAddr/SECTION_SIZE,dwAddr/SECTION_SIZE);
		RamToFlash(dwAddr,(u32)buf,512);
		rt_hw_interrupt_enable(level);
	}
	else if (0 == i)	// ˵����ȫ�µĿ�ʼ��������0��ʼ
	{
		memset(buf,0xff,sizeof(buf));
		*(u32*)buf = 0;		// ��Ŵ�0��ʼ
		*(S_DaXiaoBianJiLu*)(buf+sizeof(S_DaXiaoBianJiLu)) = jilu;
		level = rt_hw_interrupt_disable();
		SelSector(dwAddr/SECTION_SIZE,dwAddr/SECTION_SIZE);
		RamToFlash(dwAddr,(u32)buf,512);
		rt_hw_interrupt_enable(level);
	}
	else
	{
		memcpy(buf,(void*)(dwAddr&(-512)),512);
		*(S_DaXiaoBianJiLu*)(buf+(dwAddr&(512-1))) = jilu;
		level = rt_hw_interrupt_disable();
		SelSector(dwAddr/SECTION_SIZE,dwAddr/SECTION_SIZE);
		RamToFlash(dwAddr&(-SECTION_SIZE),(u32)buf,512);
		rt_hw_interrupt_enable(level);
	}
}

// ��ȡ��¼����
u32 GetJiLuZongShu()
{
	u32 count = 0;	// ������
	u32 addr;
	if (-1 != *(s32*)SECTION_LOG_ADDR_2)	// ˵�������ռ䶼��ռ��
	{
		count = SECTION_SIZE/sizeof(S_DaXiaoBianJiLu)-1;
		addr = *(u32*)SECTION_LOG_ADDR_1 > *(u32*)SECTION_LOG_ADDR_2 ? SECTION_LOG_ADDR_1:SECTION_LOG_ADDR_2;
		while((-1 != *(u32*)(addr+sizeof(S_DaXiaoBianJiLu)*(count+2-SECTION_SIZE/sizeof(S_DaXiaoBianJiLu))))
						&&(count < 2*SECTION_SIZE/sizeof(S_DaXiaoBianJiLu) -2))
		{
			count++;
		}
	}
	else
	{
		while((-1 != *(u32*)(SECTION_LOG_ADDR_1+sizeof(S_DaXiaoBianJiLu)*(count+1))) &&
			(count < SECTION_SIZE/sizeof(S_DaXiaoBianJiLu) -1)
			)
		{
			count++;
		}
	}
	return count;
}

// ��ȡ��һ����¼��ַ
S_DaXiaoBianJiLu* GetFirstJiLu()
{
	u32 addr = *(u32*)SECTION_LOG_ADDR_1 < *(u32*)SECTION_LOG_ADDR_2 
		? SECTION_LOG_ADDR_1:SECTION_LOG_ADDR_2;
	return -1==*(s32*)(addr+sizeof(S_DaXiaoBianJiLu)) ? NULL : (S_DaXiaoBianJiLu*)(addr+sizeof(S_DaXiaoBianJiLu));
}

// ��ȡ���һ����¼��ַ
S_DaXiaoBianJiLu* GetLastJiLu()
{
	u32 count = 0;
	u32 addr = *(s32*)SECTION_LOG_ADDR_1 >= *(s32*)SECTION_LOG_ADDR_2 
						? SECTION_LOG_ADDR_1:SECTION_LOG_ADDR_2;

	while((-1 != *(s32*)(addr+sizeof(S_DaXiaoBianJiLu)*(count+1))) &&
		(count < SECTION_SIZE/sizeof(S_DaXiaoBianJiLu) -1))
	{
		count++;
	}

	if (0==count)
	{
		return NULL;
	}
	else
	{
		return ((S_DaXiaoBianJiLu*)addr) + count;
	}
}


S_DaXiaoBianJiLu* GetNextJiLu(S_DaXiaoBianJiLu* current)
{
	if (( current == NULL ) || ( current <= (S_DaXiaoBianJiLu*)SECTION_LOG_ADDR_1 )
		|| ( current == (S_DaXiaoBianJiLu*)SECTION_LOG_ADDR_2 )
		|| ( current >= (S_DaXiaoBianJiLu*)(SECTION_LOG_ADDR_2+SECTION_SIZE) ))
	{
		return NULL;
	}
	
	if ((u32)(current+1)==SECTION_LOG_ADDR_2)		// ��Ҫ�л�
	{
		if (*(s32*)SECTION_LOG_ADDR_2 > *(s32*)SECTION_LOG_ADDR_1 )
		{
			return -1 == *(s32*)(SECTION_LOG_ADDR_2+sizeof(S_DaXiaoBianJiLu)) 
							? NULL : ((S_DaXiaoBianJiLu*)SECTION_LOG_ADDR_2) +1;
		}
		else
		{
			return NULL;
		}
	}
	else if((u32)(current+1)==SECTION_LOG_ADDR_2+SECTION_SIZE)	// ��Ҫ�л�
	{
		if (*(s32*)SECTION_LOG_ADDR_1 > *(s32*)SECTION_LOG_ADDR_2 )
		{
			return -1 == *(s32*)(SECTION_LOG_ADDR_1+sizeof(S_DaXiaoBianJiLu)) 
				? NULL : ((S_DaXiaoBianJiLu*)SECTION_LOG_ADDR_1) +1;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return -1 == *(s32*)(current+1) ? NULL : current +1;
	}
}


// ��ȡ��һ����¼
S_DaXiaoBianJiLu* GetPreviousJiLu(S_DaXiaoBianJiLu* current)
{
	if (( current == NULL ) || ( current <= (S_DaXiaoBianJiLu*)SECTION_LOG_ADDR_1 )
		|| ( current == (S_DaXiaoBianJiLu*)SECTION_LOG_ADDR_2 )
		|| ( current >= (S_DaXiaoBianJiLu*)(SECTION_LOG_ADDR_2+SECTION_SIZE) ))
	{
		return NULL;
	}

	if ((u32)(current-1)==SECTION_LOG_ADDR_2)		// ��Ҫ�л���1
	{
		if (*(s32*)SECTION_LOG_ADDR_2 > *(s32*)SECTION_LOG_ADDR_1 )
		{
			return -1 == *(s32*)(SECTION_LOG_ADDR_2-sizeof(S_DaXiaoBianJiLu)) 
				? NULL : ((S_DaXiaoBianJiLu*)SECTION_LOG_ADDR_2) -1;
		}
		else
		{
			return NULL;
		}
	}
	else if((u32)(current-1)==SECTION_LOG_ADDR_1)	// ��Ҫ�л���2
	{
		if (*(s32*)SECTION_LOG_ADDR_1 > *(s32*)SECTION_LOG_ADDR_2 )
		{
			return -1 == *(s32*)(SECTION_LOG_ADDR_2+SECTION_SIZE-sizeof(S_DaXiaoBianJiLu)) 
				? NULL : (S_DaXiaoBianJiLu*)(SECTION_LOG_ADDR_2+SECTION_SIZE-sizeof(S_DaXiaoBianJiLu));
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return -1 == *(s32*)(current-1) ? NULL : current -1;
	}
}


