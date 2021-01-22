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
	__asm{
		B				c
a:
		MOV			r1,#9
b:
		SUBS    r1,r1,#1
			BCS     b
c:
		SUBS	r0,r0,#1
			NOP
			BCS		a
	}
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


void MemSet(void *src,u32 count,unsigned char data)
{
	while (count--)
	{
		*(unsigned char *)src= data;
		src= (unsigned char *)src + 1;
	}
	return;
}

void MemCpy(u8 *target,const u8 *source,u32 lenth)
{
	for (;lenth>0;lenth--)
	{
		*target++ = *source++;
	}
}

u32 xokostrncpy(char *dest,const char *src,s32 len)
{
	char *s=dest;
	while ((*src)&&(len>=0))
	{
		*s=*src;
		s++;
		src++;
		len--;
	}
	*s=0;
	return len;
}

u32 xokostrcpy(char *dest,const char *src)
{
	char *s=dest;
	u32 len=0;
	while (*src)
	{
		*s=*src;
		s++;
		src++;
		len++;
	}
	*s=0;
	return len;
}

s32 xokostrlen(const char *str)
{
	s32 len = 0;
	if (str==(char *)0)
		return 0;
	while (*str++ != 0)
		len++;
	return len;
}


u8 xokoitoa(s32 value, char *buf, s32 radix)
{
	u8 *p, *first, temp;
	s32 digital;
	u32 val;
	u8 n=0;
	p = (u8 *)buf;
	if (radix==16)
	{
		*p++ = '0';
		*p++ = 'x';
		val = (unsigned long)value;
		n=n+2;
	}
	else if (value < 0)
	{
		*p++ = '-';
		val = -value;
		n++;
	}

	first = p;
	do
	{
		digital = val%radix;
		val=val/radix;
		n++;
		if (digital > 9)
			*p++ = (u8)(digital - 10 + 'A');
		else
			*p++ = (u8)(digital + '0');
	}
	while (val > 0);
	*p = '\0';
	p--;
	do
	{
		temp = *p;
		*p = *first;
		*first = temp;
		p--;
		first++;
	}
	while (first < p);
	return n;
}

s16 xokostrcmp(const char *source,const char *dest)
{
	s16 ret = 0 ;
	while (!(ret =*source-*dest)&&*dest)
	{
		source++;
		dest++;
	}
	if (ret<0)
		ret = -1 ;
	else if ( ret > 0 )
		ret = 1 ;
	return (ret);
}

s16 xokostrncmp(const char *source,const char *dest,u32 len)
{
	s16 ret = 0 ;
	while (!(ret =*source-*dest)&&*dest)
	{
		len--;
		if (len==0) break;
		source++;
		dest++;
	}
	if (ret<0)
		ret = -1 ;
	else if ( ret > 0 )
		ret = 1 ;
	return (ret);
}

s16 xokostrrcmp(const char *source,const char *dest)
{
	s16 ret = 0 ,len=0;
	if ((!dest)&&(!source))
		return 0;
	for (; *source; source++)
		for (; *dest; dest++)
		{
			len++;
		}
		do
		{

			if (len==0) break;
			source--;
			dest--;
			len--;
		}
		while (!(ret =TOUPPER(*source)-TOUPPER(*dest))&&*dest);
		if (ret<0)
			ret = -1 ;
		else if ( ret > 0 )
			ret = 1 ;
		return (ret);
}

u32 xokostrcat (char * dst, const char * src)
{
	char *d;
	u32 n=0;
	if ((!dst)&&(!src))
		return 0;
	d = dst;
	for (; *d; d++)
	{
		;
	}
	for (; *src; src++)
	{
		*d++ = *src;
		n++;
	}
	*d = 0;
	return (n);
}
						
u8 xokosetbit(u8 pos)
{
	return(1<<pos);
}

#if 0
void vprintf(char *out,const char *cmd, va_list x)
{
	u32 type=0;
	char * y;
	do
	{
		if (*cmd == '%')
		{
			++cmd;
			if (*cmd == 'd')
			{
				type=va_arg(x,int);
				out=out+xokoitoa(type,out,10);
			}
			else if (*cmd == 's')
			{
				y=va_arg(x,char*);
				if (*y==0)
				{
					out=out+xokostrcpy((char *)out,"<NULL>");
				}
				else
				{
					out=out+xokostrcpy((char *)out,y);
				}
			}
			else if (*cmd == 'x')
			{
				type=va_arg(x,int);
				out=out+xokoitoa(type,out,16);
			}
			else	// 对于不支持的转义符直接输出转义符后面的
			{
				*out++=*cmd;
			}
		}
		else
		{
			*out=*cmd;
			out++;
		}
		++cmd;
	}
	while (*cmd != '\0');
	*out=0;
}

#endif