#include "Common.h"

const char C_aAsciiTable[] = "0123456789ABCDEF";

// us ��ʱ
//�ú���ֻ��ads1.2�в���ͨ��
//ϵͳʱ�� 11059200*4
// ���Խ�� �������10000���������ʱ��Ϊ9900us
void DelayXus(u32 dwUs)	
{
#ifndef WIN32
#if	Fcclk!= 11059200*4
#error	"ϵͳʱ�ӷ������ģ�������ú���"
#endif
	u32 i;
	while(dwUs--)
		for(i=0;i<9;i++);
#endif
}

// ms ��ʱ
//�ú���ֻ��ads1.2�в���ͨ��
//ϵͳʱ�� 11059200*4
// ���Խ�� �������100���������ʱ��Ϊ100ms
void DelayXms(u32 dwMs)	
{
	while(dwMs--)
	{
		DelayXus(1000);	//΢�붨ʱ����һ�������Բ�һ���Ǵ������1000
	}
}



#define 	TOUPPER(c) 		((((c)>='a')&&((c)<='z')) ? (c) - 'a' +'A':c)
#define 	TOLOWER(c) 		((((c)>='A')&&((c)<='Z')) ? (c) - 'A' +'a':c)

