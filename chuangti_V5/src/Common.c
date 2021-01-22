#include "Common.h"

const char C_aAsciiTable[] = "0123456789ABCDEF";

// us 延时
//该函数只在ads1.2中测试通过
//系统时钟 11059200*4
// 测试结果 输入参数10000，输出波形时间为9900us
void DelayXus(u32 dwUs)	
{
#ifndef WIN32
#if	Fcclk!= 11059200*4
#error	"系统时钟发生更改，请调整该函数"
#endif
	u32 i;
	while(dwUs--)
		for(i=0;i<9;i++);
#endif
}

// ms 延时
//该函数只在ads1.2中测试通过
//系统时钟 11059200*4
// 测试结果 输入参数100，输出波形时间为100ms
void DelayXms(u32 dwMs)	
{
	while(dwMs--)
	{
		DelayXus(1000);	//微秒定时器有一定误差，所以不一定是传入参数1000
	}
}



#define 	TOUPPER(c) 		((((c)>='a')&&((c)<='z')) ? (c) - 'a' +'A':c)
#define 	TOLOWER(c) 		((((c)>='A')&&((c)<='Z')) ? (c) - 'A' +'a':c)

