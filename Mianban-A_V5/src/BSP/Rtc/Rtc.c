// 实时钟模块
#include "../Common.h"
#include "Rtc.h"
#include "../DS1302/ds1302.h"

S_DateTime g_sNow;	// 当前时间

// 获取时间值
void GetTime(void)
{
	Ds1302GetTime(&g_sNow);
}

void SetTime(void)
{
	Ds1302SetTime(&g_sNow);
}

