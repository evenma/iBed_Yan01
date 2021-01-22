#include "../Common.h"
#include "Iap.h"

/* ����IAP������ */
#define  IAP_SELSECTOR        		50
#define  IAP_RAMTOFLASH       	51
#define  IAP_ERASESECTOR      	52
#define  IAP_BLANKCHK         		53
#define  IAP_READPARTID       		54
#define  IAP_BOOTCODEID       	55
#define  IAP_COMPARE          		56

#define iap_entry(a, b)             ((void (*)(u32*,u32*))(0x7ffffff1))(a, b)

static u32  paramin[1+5];                         // IAP��ڲ���������
#define paramout	paramin                        // IAP���ڲ���������

/****************************************************************************
* ���ƣ�SelSector()
* ���ܣ�IAP��������ѡ���������50��
* ��ڲ�����sec1        ��ʼ����
*          sec2        ��ֹ����
* ����ֵ��IAP����ֵ(paramout������)
****************************************************************************/
u32  SelSector(u8 sec1, u8 sec2)
{
	paramin[0] = IAP_SELSECTOR;              	// ����������
	paramin[1] = sec1;                       		// ���ò���
	paramin[2] = sec2;
	iap_entry(paramin, paramout);    	// ����IAP�������
	return paramout[0];
}

/****************************************************************************
* ���ƣ�RamToFlash()
* ���ܣ�����RAM�����ݵ�FLASH���������51��
* ��ڲ�����dst        Ŀ���ַ����FLASH��ʼ��ַ����512�ֽ�Ϊ�ֽ�
*          src        Դ��ַ����RAM��ַ����ַ�����ֶ���
*          no         �����ֽڸ�����Ϊ512/1024/4096/8192
* ����ֵ��IAP����ֵ(paramout������)
****************************************************************************/
u32  RamToFlash(u32 dst, u32 src, u32 no)
{
	paramin[0] = IAP_RAMTOFLASH;             // ����������
	paramin[1] = dst;                        // ���ò���
	paramin[2] = src;
	paramin[3] = no;
	//paramin[4] = IAP_FCCLK;
	paramin[4] = Fosc/1000;
	iap_entry(paramin, paramout);         // ����IAP�������
	return paramout[0];
}

/****************************************************************************
* ���ƣ�EraseSector()
* ���ܣ������������������52��
* ��ڲ�����sec1       ��ʼ����
*          sec2       ��ֹ����
* ����ֵ��IAP����ֵ(paramout������)
****************************************************************************/
u32  EraseSector(u8 sec1, u8 sec2)
{
	paramin[0] = IAP_ERASESECTOR;            // ����������
	paramin[1] = sec1;                       		// ���ò���
	paramin[2] = sec2;
	//paramin[3] = IAP_FCCLK;
	paramin[3] = Fosc/1000;				//����ʹ��PLL����ʱ,Fcclk=Fosc
	iap_entry(paramin, paramout);         	// ����IAP�������
	return paramout[0];
}

/****************************************************************************
* ���ƣ�BlankCHK()
* ���ܣ�������գ��������53��
* ��ڲ�����sec1       ��ʼ����
*          sec2       ��ֹ����
* ����ֵ��IAP����ֵ(paramout������)
****************************************************************************/
u32  BlankCHK(u8 sec1, u8 sec2)
{  
	paramin[0] = IAP_BLANKCHK;               // ����������
	paramin[1] = sec1;                       // ���ò���
	paramin[2] = sec2;
	iap_entry(paramin, paramout);         // ����IAP�������
	return paramout[0];
}


/****************************************************************************
* ���ƣ�ReadParID()
* ���ܣ�������ID
* ��ڲ�������
* ����ֵ������ID
****************************************************************************/
u32  ReadParID(void)
{ 
	paramin[0] = IAP_READPARTID;             // ����������
	iap_entry(paramin, paramout);         // ����IAP�������
	return paramout[1];
}


/****************************************************************************
* ���ƣ�BootCodeID()
* ���ܣ���ȡboot����汾�ţ��������55��
* ��ڲ�������
* ����ֵ��2�ֽ�boot�汾�ţ�����Ϊ<�ֽ�1(��)>.<�ֽ�0(��)>
****************************************************************************/
u32  BootCodeID(void)
{ 
	paramin[0] = IAP_BOOTCODEID;             // ����������
	iap_entry(paramin, paramout);         // ����IAP�������
	return paramout[1];
}


/****************************************************************************
* ���ƣ�Compare()
* ���ܣ�У�����ݣ��������56��
* ��ڲ�����dst        Ŀ���ַ����RAM/FLASH��ʼ��ַ����ַ�����ֶ���
*          src        Դ��ַ����FLASH/RAM��ַ����ַ�����ֶ���
*          no         �����ֽڸ����������ܱ�4����
* ����ֵ��IAP����ֵ(paramout������)
****************************************************************************/
u32  Compare(u32 dst, u32 src, u32 no)
{
	paramin[0] = IAP_COMPARE;                // ����������
	paramin[1] = dst;                        // ���ò���
	paramin[2] = src;
	paramin[3] = no;
	iap_entry(paramin, paramout);         // ����IAP�������
	return paramout[0];
}

