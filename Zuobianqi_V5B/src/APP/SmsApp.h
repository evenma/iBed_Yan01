#ifndef	_SMS_APP_H_
#define	_SMS_APP_H_

#include <Common.h>

#define	SMS_NUM_MAX_LEN	16

// ���ź���
extern u8 g_SmsNum[SMS_NUM_MAX_LEN+1];

/* ���¶��ź����ֶ�
������
	pNum �� ����ĺ��뻺����ָ��
	begin : ���µĺ�����ʼλ��
	len �� ����ĺ��볤��
*/
extern void SmsSetNum(u8* pNum, u8 begin, u8 len);



#endif
