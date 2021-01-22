#include <rtthread.h>
#include <finsh.h>

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif

#include "CAN/IncludeCan.h"
#include "CAN/Can.h"

#include "lpc214x.h"


#include "SmsApp.h"

// 短信号码
u8 g_SmsNum[SMS_NUM_MAX_LEN+1] = {0};

/* 更新短信号码字段
参数：
	pNum ： 传入的号码缓冲区指针
	begin : 更新的号码起始位置
	len ： 传入的号码长度
*/
void SmsSetNum(u8* pNum, u8 begin, u8 len)
{
	if(begin+len >= SMS_NUM_MAX_LEN)
	{
		return;
	}
	else
	{
		memcpy(g_SmsNum+begin,pNum,len);
	}
}

