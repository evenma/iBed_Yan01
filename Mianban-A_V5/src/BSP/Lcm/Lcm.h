/********************************************************************
**   filename:	lcm.h	Copyright   (c)   2010
**   author:	Weijie Gu
**	E-mail:	guwj1984@qq.com
**   date:	2010.05.13
**   discription:LCM驱动文件
**   version:  1.0.0.1
********************************************************************/
/********************************************************************
**   modifier:
**	date:
**	discription:
**	version:
********************************************************************/
#ifndef __LCM_H
#define __LCM_H

#include "Common.h"

extern void LCM_Init(void);
extern void SPI_SSSet(unsigned char Status);
extern void SPI_Send(unsigned char Data);
extern void FontSet(unsigned char Font_NUM,unsigned char Color);
extern void FontSet_cn(unsigned char Font_NUM,unsigned char Color);
extern void PutChar(unsigned char x,unsigned char y,unsigned char a);
extern void PutString(unsigned char x,unsigned char y,const unsigned char *p);
extern void PutChar_cn(unsigned char x,unsigned char y,const unsigned char * GB);
extern void PutString_cn(unsigned char x,unsigned char y,const unsigned char *p);
extern void SetPaintMode(unsigned char Mode,unsigned char Color);
extern void PutPixel(unsigned char x,unsigned char y);
extern void Line(unsigned char s_x,unsigned char  s_y,unsigned char  e_x,unsigned char  e_y);
extern void Circle(unsigned char x,unsigned char y,unsigned char r,unsigned char mode);
extern void Rectangle(unsigned char left, unsigned char top, unsigned char right, unsigned char bottom, unsigned char mode);
extern void ClrScreen(void);
extern void PutBitmap(unsigned char x,unsigned char y,unsigned char width,unsigned char high,const unsigned char *p);
extern void FontMode(unsigned char Cover,unsigned char Color);
extern void ShowChar(unsigned char x,unsigned char y,unsigned char a,unsigned char type) ;
extern void ShowShort(unsigned char x,unsigned char y,unsigned short a,unsigned char type) ;
extern void SetBackLight(unsigned char Deg);

extern void LcmPrintString(int x,int y,const char *fmt,...);
extern void LcmCenterString(int y,const char *info);
// 对指定矩形区域清屏
extern void LcmClearRectangle(unsigned char x,unsigned char y,unsigned char w,unsigned h);

#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

