/********************************************************************
**   filename:	ds1302.h	Copyright   (c)   2010
**   author:	Weijie Gu
**	E-mail:	guwj1984@qq.com
**   date:	2010.09.26
**   discription:DS1302时钟芯片驱动头文件
**   version:  1.0.0.1
********************************************************************/
/********************************************************************
**   modifier:
**	date:
**	discription:
**	version:
********************************************************************/

#ifndef	__DS1302_H
#define	__DS1302_H

#include "../Config.h"
#include "../Common.h"

// 时间结构体
typedef struct
{
	u32 wYear;			// 年
	u8 bMonth;		// 月
	u8 bDay;			// 日
	u8 bHour;			// 时
	u8 bMinute;		// 分
	u8 bSecond;		// 秒
	u8 bReserve;		// 保留
}S_DateTime;


extern	void Ds1302Init(void);
//设置时间值
extern	void Ds1302SetTime(S_DateTime *pDateTime);
//读取时间值
extern	void Ds1302GetTime(S_DateTime *pDateTime);
// 写RAM，参数分别为地址、长度、缓冲区
extern	void Ds1302WriteRam(u8 addr,u8 len,u8 * buf);
// 读RAM，参数分别为地址、长度、缓冲区
extern	void Ds1302ReadRam(u8 addr,u8 len,u8 * buf);

#endif

