// 全局设置变量，这些变量需要保存到 EEPROM 中
#include "Common.h"
#include "GlobalSetting.h"
#include "Iap/Iap.h"
#include <string.h>
#include "Rtc/Rtc.h"
#include "rtthread.h"
#include <rthw.h>

S_GlobalSetting g_sGlobalSetting;	// 全局变量
S_ZuobianqiSetA g_ZuobianqiSetA;
S_ZuobianqiSetB g_ZuobianqiSetB;

// Flash存储空间分配
// 12（0X0C）扇区，地址 0X0001 8000-9FFF，共8KB	，	用于记录大小便，与13扇区形成乒乓
// 13（0X0D）扇区，地址 0X0001 A000-BFFF，共8KB	，	用于记录大小便，与12扇区形成乒乓
// 14（0X0E）扇区，地址 0X0001 C000-DFFF，共8KB	，	用于存储用户设置信息

#define SECTION_SIZE	(8*1024)
#define SECTION_LOG_ADDR_1	0X00018000
#define SECTION_LOG_ADDR_2	0X0001A000

void SetDefault(void)
{
	g_ZuobianqiSetA.ShuiWenGaoDi=0x02;     // 默认设置为37°
	g_ZuobianqiSetA.NuanFengQiangRuo=0x03; // 默认设置为高档位
	g_ZuobianqiSetA.QiangXiQiangRuo=0x03;  // 默认设置为高档位
	g_ZuobianqiSetA.HoldOn= 0;			   // 默认不挂起，所有控制器参数均一致
}

// 用户设置信息存储格式：
// 使用256字节（64字）长度的存储空间，前面252字节作为有效数据，最后一个字作为校验和（前面所有字节的和，注意不是字）

// 从 EEPROM 中载入全局变量
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
		// 短信号码防溢出
		g_sGlobalSetting.acMsgNumber[15] = 0;
	}
	else
	{
		// 校验不正确，使用默认值
		memset(&g_sGlobalSetting,0,sizeof(g_sGlobalSetting));
	}
}

static u8 buf[512];
// 将全局变量保存的 EEPROM 中
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
	// 擦除写入时应关闭中断
	level = rt_hw_interrupt_disable();
	SelSector(14,14);
	EraseSector(14,14);
	SelSector(14,14);
	RamToFlash(0X0001C000,(u32)buf,512);	// 一次性最少写入512字节
	rt_hw_interrupt_enable(level);
}

/*
	存储方式：事件以乒乓方式进行存储
		0-3字节：区块序号，该区块被使用后自动加1
		4-7字节：保留，全0XFF
		8-8,191字节：以8字节为单位，保存记录信息
*/


// 大小便记录
//	参数 bDaXiaoBian 1=大便，0=小便
void DaXiaoBianJiLu(u8 bDaXiaoBian)
{
	u32 dwAddr;
	S_DaXiaoBianJiLu jilu;
	int i;
	rt_base_t level;

	DbgPrintf("大小便记录，事件=%d\r\n",bDaXiaoBian);
	GetTime();
	jilu = *(S_DaXiaoBianJiLu*)&g_sNow;
	jilu.bEvent = bDaXiaoBian;
	// 保存到 Flash 中
	// 1、查找到区块
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
	// 2、找到最后一条记录的位置
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
	// 3、判断该记录是否已经到最后，如已到最后则切换到前面区块并对前面区块进行擦除再写入序号以及记录
	if (i == SECTION_SIZE/sizeof(S_DaXiaoBianJiLu))	// 说明当前扇区已满，需要切换到新的扇区
	{
		u32 serialNo ;
		dwAddr -= SECTION_SIZE;
		// 首先要更新序列号
		serialNo = *(u32*)dwAddr + 1;
		// 切换到下一扇区
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
	else if (0 == i)	// 说明是全新的开始，计数从0开始
	{
		memset(buf,0xff,sizeof(buf));
		*(u32*)buf = 0;		// 序号从0开始
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

// 获取记录总数
u32 GetJiLuZongShu()
{
	u32 count = 0;	// 总数量
	u32 addr;
	if (-1 != *(s32*)SECTION_LOG_ADDR_2)	// 说明两个空间都被占了
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

// 获取第一条记录地址
S_DaXiaoBianJiLu* GetFirstJiLu()
{
	u32 addr = *(u32*)SECTION_LOG_ADDR_1 < *(u32*)SECTION_LOG_ADDR_2 
		? SECTION_LOG_ADDR_1:SECTION_LOG_ADDR_2;
	return -1==*(s32*)(addr+sizeof(S_DaXiaoBianJiLu)) ? NULL : (S_DaXiaoBianJiLu*)(addr+sizeof(S_DaXiaoBianJiLu));
}

// 获取最后一条记录地址
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
	
	if ((u32)(current+1)==SECTION_LOG_ADDR_2)		// 需要切换
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
	else if((u32)(current+1)==SECTION_LOG_ADDR_2+SECTION_SIZE)	// 需要切换
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


// 获取上一条纪录
S_DaXiaoBianJiLu* GetPreviousJiLu(S_DaXiaoBianJiLu* current)
{
	if (( current == NULL ) || ( current <= (S_DaXiaoBianJiLu*)SECTION_LOG_ADDR_1 )
		|| ( current == (S_DaXiaoBianJiLu*)SECTION_LOG_ADDR_2 )
		|| ( current >= (S_DaXiaoBianJiLu*)(SECTION_LOG_ADDR_2+SECTION_SIZE) ))
	{
		return NULL;
	}

	if ((u32)(current-1)==SECTION_LOG_ADDR_2)		// 需要切换到1
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
	else if((u32)(current-1)==SECTION_LOG_ADDR_1)	// 需要切换到2
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


