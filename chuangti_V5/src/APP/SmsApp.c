#include <rtthread.h>
#include <finsh.h>

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif

#include "CAN/IncludeCan.h"
#include "CAN/Can.h"

#include "lpc214x.h"


#include "SmsApp.h"

// ���ź���
u8 g_SmsNum[SMS_NUM_MAX_LEN+1] = {0};

/* ���¶��ź����ֶ�
������
	pNum �� ����ĺ��뻺����ָ��
	begin : ���µĺ�����ʼλ��
	len �� ����ĺ��볤��
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

