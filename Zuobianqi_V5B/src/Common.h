#ifndef	__COMMON_H
#define	__COMMON_H

#include "Config.h"
#include <string.h>
#include <rtthread.h>
#include <finsh.h>

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef unsigned char  u8;                   /* �޷���8λ���ͱ���                        */
typedef signed   char  s8;                    /* �з���8λ���ͱ���                        */
typedef unsigned short u16;                  /* �޷���16λ���ͱ���                       */
typedef signed   short s16;                   /* �з���16λ���ͱ���                       */
typedef unsigned int   u32;                  /* �޷���32λ���ͱ���                       */
typedef signed   int   s32;                   /* �з���32λ���ͱ���                       */
typedef float          f32;                    /* �����ȸ�������32λ���ȣ�                 */
typedef double         f64;                    /* ˫���ȸ�������64λ���ȣ�                 */
typedef unsigned int BOOL;			//��������

#define BIT0		(0x01ul<<0)
#define BIT1		(0x01ul<<1)
#define BIT2		(0x01ul<<2)
#define BIT3		(0x01ul<<3)
#define BIT4		(0x01ul<<4)
#define BIT5		(0x01ul<<5)
#define BIT6		(0x01ul<<6)
#define BIT7		(0x01ul<<7)
#define BIT8		(0x01ul<<8)
#define BIT9		(0x01ul<<9)
#define BIT10		(0x01ul<<10)
#define BIT11		(0x01ul<<11)
#define BIT12		(0x01ul<<12)
#define BIT13		(0x01ul<<13)
#define BIT14		(0x01ul<<14)
#define BIT15		(0x01ul<<15)
#define BIT16		(0x01ul<<16)
#define BIT17		(0x01ul<<17)
#define BIT18		(0x01ul<<18)
#define BIT19		(0x01ul<<19)
#define BIT20		(0x01ul<<20)
#define BIT21		(0x01ul<<21)
#define BIT22		(0x01ul<<22)
#define BIT23		(0x01ul<<23)
#define BIT24		(0x01ul<<24)
#define BIT25		(0x01ul<<25)
#define BIT26		(0x01ul<<26)
#define BIT27		(0x01ul<<27)
#define BIT28		(0x01ul<<28)
#define BIT29		(0x01ul<<29)
#define BIT30		(0x01ul<<30)
#define BIT31		(0x01ul<<31)

#define NULL	0


/********************************/
/*     Ӧ�ó�������             */
/********************************/
//���¸�����Ҫ�Ķ�

/********************************/
/*     �����ӵ�����             */
/********************************/
/* ϵͳ����, Fosc��Fcclk��Fcco��Fpclk���붨��*/
#define Fosc            11059200                    //����Ƶ��,10MHz~25MHz��Ӧ����ʵ��һ��
//#define Fosc            16000000 
//#define Fcclk           (Fosc *3)                  
#define Fcclk           (Fosc * 4)                  //ϵͳƵ�ʣ�����ΪFosc��������(1~32)����<=60MHZ
#define Fcco            (Fcclk * 4)                 //CCOƵ�ʣ�����ΪFcclk��2��4��8��16������ΧΪ156MHz~320MHz
#define Fpclk           (Fcclk / 4) * 1             //VPBʱ��Ƶ�ʣ�ֻ��Ϊ(Fcclk / 4)��1��2��4��


#if ENABLE_DEBUG == 1
#define DbgPrintf	rt_kprintf("file=%s,line=%d,function=%s\t",__FILE__,__LINE__,__FUNCTION__);rt_kprintf
#else
#define DbgPrintf(...)
#endif

#define CLEAN_DATA(data) memset(&data,0,sizeof(data))

#define max(a,b)	((a)>(b))?(a):(b)
#define min(a,b)	((a)<(b))?(a):(b)

extern void DelayXus(u32 dwUs);
extern void DelayXms(u32 dwMs);
extern void Dump(u8* addr,u32 len);
extern const char C_aAsciiTable[];

extern void MemSet(void *src,u32 count,unsigned char data);
extern void MemCpy(u8 *target,const u8 *source,u32 lenth);

#endif
