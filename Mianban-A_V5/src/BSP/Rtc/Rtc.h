#ifndef __RTC_H
#define __RTC_H

#include "../DS1302/ds1302.h"


extern S_DateTime g_sNow;	// 当前时间


// 获取时间值
void GetTime(void);
// 设置时间
void SetTime(void);


#endif
