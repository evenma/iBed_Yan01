// ʵʱ��ģ��
#include "../Common.h"
#include "Rtc.h"
#include "../DS1302/ds1302.h"

S_DateTime g_sNow;	// ��ǰʱ��

// ��ȡʱ��ֵ
void GetTime(void)
{
	Ds1302GetTime(&g_sNow);
}

void SetTime(void)
{
	Ds1302SetTime(&g_sNow);
}

