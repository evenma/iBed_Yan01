#ifndef _74HC165_H_
#define _74HC165_H_

#include <Common.h>

// ��ʼ�� 74HC165
extern void Hc165Init(void);

/*  ��ȡ 74HC165 ������
	���� ��
		output: ���������
		len : ��Ҫ��ȡ�������ֽ���(������)
*/
extern void GetHc165Input(u8* output, u8 len);


#endif

