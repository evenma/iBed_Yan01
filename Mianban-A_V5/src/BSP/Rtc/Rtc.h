#ifndef __RTC_H
#define __RTC_H

#include "../DS1302/ds1302.h"


extern S_DateTime g_sNow;	// ��ǰʱ��


// ��ȡʱ��ֵ
void GetTime(void);
// ����ʱ��
void SetTime(void);


#endif
