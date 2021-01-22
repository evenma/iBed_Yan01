#ifndef	_SMS_APP_H_
#define	_SMS_APP_H_

#include <Common.h>

#define	SMS_NUM_MAX_LEN	16

// 短信号码
extern u8 g_SmsNum[SMS_NUM_MAX_LEN+1];

/* 更新短信号码字段
参数：
	pNum ： 传入的号码缓冲区指针
	begin : 更新的号码起始位置
	len ： 传入的号码长度
*/
extern void SmsSetNum(u8* pNum, u8 begin, u8 len);



#endif
