/********************************************************************
**   filename:	ds1302.c	Copyright   (c)   2010
**   author:	Weijie Gu
**	E-mail:	guwj1984@qq.com
**   date:	2010.09.26
**   discription:DS1302ʱ��оƬ�����ļ�
**   version:  1.0.0.1
********************************************************************/
#include "ds1302.h"
#include <string.h>

// �˿ڶ���
#define DS1302_SET_RST()	IO1PIN |= BIT26
#define DS1302_CLR_RST()	IO1PIN &= (~BIT26)
#define DS1302_SET_CLK()	IO0PIN |= BIT2
#define DS1302_CLR_CLK()	IO0PIN &= (~BIT2)
#define DS1302_SET_IO()		IO0PIN |= BIT3
#define DS1302_CLR_IO()		IO0PIN &= (~BIT3)
#define DS1302_IN_IO()		IO0DIR &= (~BIT3)
#define DS1302_OUT_IO()		IO0DIR |= BIT3
#define DS1302_READ()		(IO0PIN&BIT3)


//DS1302ʵʱ�����ݽṹ��
typedef	struct _stcDS1302_
{
	union _unDS1302A_
	{
		u32		WordA;			//�ֲ�������
		struct	
		{
			//byte0
			u32	SEC_SEC			:4;//SEC_SEC
			u32	SEC_10SEC			:3;//SEC_10SEC
			u32	RSV1_BIT1			:1;//SEC_CH
			//byte1
			u32	MIN_MIN				:4;//MIN_MIN
			u32	MIN_10MIN			:3;//MIN_10MIN
			u32	RSV2_BIT1 			:1;//����
			//byte2
			u32	HOUR_HOUR			:4;//HOUR_HOUR
			u32	HOUR_10HOUR			:2;//HOUR_10HOUR
//			u32	HOUR_10_A_P		:1;//HOUR_10_A/P��1=AM��0=PM
			u32	RSV3_BIT1 			:1;//����
			u32	HOUR_12_24			:1;//HOUR_12/24��1=12��0=24
			//byte3
			u32	DATE_DATE			:4;//DATE_DATE��һ�����е�����
			u32	DATE_10DATE		:2;//DATE_10DATE
			u32	RSV4_BIT2			:2;//����
		}Bits;		
	}DS1302A;

	union _unDS1302B_
	{
		u32		WordB;			//�ֲ�������
		struct	
		{
			//byte0
			u32	MONTH_MONTH		:4;//MONTH_MONTH
			u32	MONTH_10MONTH	:1;//MONTH_10MONTH
			u32	RSV1_BIT3			:3;//����
			//byte1
			u32	DAY					:3;//DAY�����ڼ�
			u32	RSV2_BIT5 			:5;//����
			//byte2
			u32	YEAR_YEAR			:4;//YEAR_YEAR
			u32	YEAR_10YEAR		:4;//YEAR_10YEAR
			//byte3
			u32	RSV4_BIT8			:8;//����
		}Bits;		
	}DS1302B;
}*PS_DS1302,S_DS1302;

void Ds1302Init(void)
{
	// Ĭ�Ͼ�����ͨIO
	IO0DIR |= (BIT2|BIT3);	
	IO1DIR |= (BIT26);	
	DS1302_CLR_RST();
	DS1302_CLR_CLK();
	DS1302_IN_IO();
}

// �������Ϸ����ֽڣ�DS1302оƬ�����ز������͵�ƽCPU�ͳ��źţ�������Ϻ�ʱ���źŴ��ڸߵ�ƽ״̬
static void SendByte(u8 byte)
{
	u8 i;

	DS1302_OUT_IO();
	for(i=0;i<8;i++)
	{
		DS1302_CLR_CLK();		// �͵�ƽ�ͳ��ź�
		if((byte>>i)&0x01)
		{
			DS1302_SET_IO();
		}
		else
		{
			DS1302_CLR_IO();
		}
		DS1302_SET_CLK();		// ��������ʱ��оƬ����
	}
}

// �������ϻ�ȡ�ֽڣ��½���ʱ��оƬ�����źţ��͵�ƽ״̬����оƬ������������ɺ�ʱ���ź�Ϊ��
static u8 RcvByte(void)
{
	u8 i;
	u8 data=0;

	DS1302_IN_IO();
	for(i=0;i<8;i++)
	{
		DS1302_CLR_CLK();		// �½���ʱ��оƬ�����ź�
		if(DS1302_READ())		// ��Ƭ���͵�ƽ����
		{
			data |= 0x01<<i;
		}
		else
		{
			data &= ~(0x01<<i);
		}
		DS1302_SET_CLK();
	}
	return data;
}

// ��ȡ���ֽ����ݣ�����Ϊ��ַ
static u8 ReadByte(u8 addr)
{
	u8 ret;
	DS1302_SET_RST();
	SendByte(0x80|(addr<<1)|1);		// ������
	ret = RcvByte();
	DS1302_CLR_CLK();
	DS1302_CLR_RST();
	return ret;
}

static void WriteByte(u8 addr,u8 dat)
{
	DS1302_SET_RST();
	SendByte(0x80|(addr<<1)|0);		// д����
	SendByte(dat);		// д����
	DS1302_CLR_CLK();
	DS1302_CLR_RST();

}


// д�洢�����Զ����нⱣ���Լ�д��������
void Ds1302WriteRam(u8 addr,u8 len,u8 * buf)
{
	// ���Ƚ��нⱣ��
	WriteByte(7,0);

	while(len--)
	{
		WriteByte(addr++,*buf++);
	}

	WriteByte(7,0x80);
}

// ��ȡRAM�е����ݣ�ʱ����Ϣ��ַ��32��ʼ
void Ds1302ReadRam(u8 addr,u8 len,u8 * buf)
{
	while(len--)
	{
		*buf++ = ReadByte(addr++);
	}
}

//����ʱ��ֵ
void Ds1302SetTime(S_DateTime *pDateTime)
{
	S_DS1302 time;
	u8* d ;
	CLEAN_DATA(time);	// ����
	time.DS1302A.Bits.SEC_SEC = pDateTime->bSecond % 10;
	time.DS1302A.Bits.SEC_10SEC = pDateTime->bSecond / 10;
	time.DS1302A.Bits.MIN_MIN = pDateTime->bMinute % 10;
	time.DS1302A.Bits.MIN_10MIN = pDateTime->bMinute / 10;
	time.DS1302A.Bits.HOUR_HOUR = pDateTime->bHour % 10;
	time.DS1302A.Bits.HOUR_10HOUR = pDateTime->bHour / 10;
//	time.DS1302A.Bits.HOUR_10_A_P = 0;			// 1=AM��0=PM
	time.DS1302A.Bits.HOUR_12_24 = 0;			// 0=24��1=12
	time.DS1302A.Bits.DATE_DATE = pDateTime->bDay % 10;
	time.DS1302A.Bits.DATE_10DATE = pDateTime->bDay / 10;

	time.DS1302B.Bits.MONTH_MONTH = pDateTime->bMonth % 10;
	time.DS1302B.Bits.MONTH_10MONTH = pDateTime->bMonth / 10;
	time.DS1302B.Bits.DAY = 1;			// ���ڣ�1-7��Ч
	time.DS1302B.Bits.YEAR_YEAR = pDateTime->wYear % 10;
	time.DS1302B.Bits.YEAR_10YEAR = (pDateTime->wYear%100) / 10;

	d = (u8*)(&time);
	DbgPrintf("time:%x, %x, %x, %x, %x, %x, %x, %x\r\n",d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7]);

	Ds1302WriteRam(0,7,(u8*)(&time));
}
//��ȡʱ��ֵ
void Ds1302GetTime(S_DateTime *pDateTime)
{
	S_DS1302 time;
	u8* d ;
	Ds1302ReadRam(0,7,(u8*)(&time));
	
	d = (u8*)(&time);
	DbgPrintf("time:%x, %x, %x, %x, %x, %x, %x, %x\r\n",d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7]);

	pDateTime->bSecond = time.DS1302A.Bits.SEC_SEC + time.DS1302A.Bits.SEC_10SEC * 10;
	pDateTime->bMinute = time.DS1302A.Bits.MIN_MIN + time.DS1302A.Bits.MIN_10MIN * 10;
	pDateTime->bHour = time.DS1302A.Bits.HOUR_12_24? 
						((time.DS1302A.Bits.HOUR_10HOUR&BIT1) ? (time.DS1302A.Bits.HOUR_10HOUR&BIT0)*10+time.DS1302A.Bits.HOUR_HOUR 
							:(time.DS1302A.Bits.HOUR_HOUR&BIT0)*10 + time.DS1302A.Bits.HOUR_HOUR + 12)
						:time.DS1302A.Bits.HOUR_HOUR + time.DS1302A.Bits.HOUR_10HOUR * 10;
	pDateTime->bDay = time.DS1302A.Bits.DATE_DATE + time.DS1302A.Bits.DATE_10DATE * 10;
	pDateTime->bMonth = time.DS1302B.Bits.MONTH_MONTH + 10 * time.DS1302B.Bits.MONTH_10MONTH ;
	pDateTime->wYear = 2000 + time.DS1302B.Bits.YEAR_YEAR + time.DS1302B.Bits.YEAR_10YEAR * 10;
}

