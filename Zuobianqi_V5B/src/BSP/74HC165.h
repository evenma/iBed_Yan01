#ifndef _74HC165_H_
#define _74HC165_H_

#include <Common.h>

// 初始化 74HC165
extern void Hc165Init(void);

/*  获取 74HC165 的输入
	参数 ：
		output: 输出缓冲区
		len : 需要获取的数据字节数(级联数)
*/
extern void GetHc165Input(u8* output, u8 len);


#endif

