/********************************************************************
**   filename:	ds1302.h	Copyright   (c)   2010
**   author:	Weijie Gu
**	E-mail:	guwj1984@qq.com
**   date:	2010.09.26
**   discription:DS1302ʱ��оƬ����ͷ�ļ�
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

// ʱ��ṹ��
typedef struct
{
	u32 wYear;			// ��
	u8 bMonth;		// ��
	u8 bDay;			// ��
	u8 bHour;			// ʱ
	u8 bMinute;		// ��
	u8 bSecond;		// ��
	u8 bReserve;		// ����
}S_DateTime;


extern	void Ds1302Init(void);
//����ʱ��ֵ
extern	void Ds1302SetTime(S_DateTime *pDateTime);
//��ȡʱ��ֵ
extern	void Ds1302GetTime(S_DateTime *pDateTime);
// дRAM�������ֱ�Ϊ��ַ�����ȡ�������
extern	void Ds1302WriteRam(u8 addr,u8 len,u8 * buf);
// ��RAM�������ֱ�Ϊ��ַ�����ȡ�������
extern	void Ds1302ReadRam(u8 addr,u8 len,u8 * buf);

#endif

