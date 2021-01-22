#include "../Common.h"
#include "Iap.h"

/* 定义IAP命令字 */
#define  IAP_SELSECTOR        		50
#define  IAP_RAMTOFLASH       	51
#define  IAP_ERASESECTOR      	52
#define  IAP_BLANKCHK         		53
#define  IAP_READPARTID       		54
#define  IAP_BOOTCODEID       	55
#define  IAP_COMPARE          		56

#define iap_entry(a, b)             ((void (*)(u32*,u32*))(0x7ffffff1))(a, b)

static u32  paramin[1+5];                         // IAP入口参数缓冲区
#define paramout	paramin                        // IAP出口参数缓冲区

/****************************************************************************
* 名称：SelSector()
* 功能：IAP操作扇区选择，命令代码50。
* 入口参数：sec1        起始扇区
*          sec2        终止扇区
* 返回值：IAP返回值(paramout缓冲区)
****************************************************************************/
u32  SelSector(u8 sec1, u8 sec2)
{
	paramin[0] = IAP_SELSECTOR;              	// 设置命令字
	paramin[1] = sec1;                       		// 设置参数
	paramin[2] = sec2;
	iap_entry(paramin, paramout);    	// 调用IAP服务程序
	return paramout[0];
}

/****************************************************************************
* 名称：RamToFlash()
* 功能：复制RAM的数据到FLASH，命令代码51。
* 入口参数：dst        目标地址，即FLASH起始地址。以512字节为分界
*          src        源地址，即RAM地址。地址必须字对齐
*          no         复制字节个数，为512/1024/4096/8192
* 返回值：IAP返回值(paramout缓冲区)
****************************************************************************/
u32  RamToFlash(u32 dst, u32 src, u32 no)
{
	paramin[0] = IAP_RAMTOFLASH;             // 设置命令字
	paramin[1] = dst;                        // 设置参数
	paramin[2] = src;
	paramin[3] = no;
	//paramin[4] = IAP_FCCLK;
	paramin[4] = Fosc/1000;
	iap_entry(paramin, paramout);         // 调用IAP服务程序
	return paramout[0];
}

/****************************************************************************
* 名称：EraseSector()
* 功能：扇区擦除，命令代码52。
* 入口参数：sec1       起始扇区
*          sec2       终止扇区
* 返回值：IAP返回值(paramout缓冲区)
****************************************************************************/
u32  EraseSector(u8 sec1, u8 sec2)
{
	paramin[0] = IAP_ERASESECTOR;            // 设置命令字
	paramin[1] = sec1;                       		// 设置参数
	paramin[2] = sec2;
	//paramin[3] = IAP_FCCLK;
	paramin[3] = Fosc/1000;				//当不使用PLL功能时,Fcclk=Fosc
	iap_entry(paramin, paramout);         	// 调用IAP服务程序
	return paramout[0];
}

/****************************************************************************
* 名称：BlankCHK()
* 功能：扇区查空，命令代码53。
* 入口参数：sec1       起始扇区
*          sec2       终止扇区
* 返回值：IAP返回值(paramout缓冲区)
****************************************************************************/
u32  BlankCHK(u8 sec1, u8 sec2)
{  
	paramin[0] = IAP_BLANKCHK;               // 设置命令字
	paramin[1] = sec1;                       // 设置参数
	paramin[2] = sec2;
	iap_entry(paramin, paramout);         // 调用IAP服务程序
	return paramout[0];
}


/****************************************************************************
* 名称：ReadParID()
* 功能：读器件ID
* 入口参数：无
* 返回值：器件ID
****************************************************************************/
u32  ReadParID(void)
{ 
	paramin[0] = IAP_READPARTID;             // 设置命令字
	iap_entry(paramin, paramout);         // 调用IAP服务程序
	return paramout[1];
}


/****************************************************************************
* 名称：BootCodeID()
* 功能：读取boot代码版本号，命令代码55。
* 入口参数：无
* 返回值：2字节boot版本号，解释为<字节1(主)>.<字节0(次)>
****************************************************************************/
u32  BootCodeID(void)
{ 
	paramin[0] = IAP_BOOTCODEID;             // 设置命令字
	iap_entry(paramin, paramout);         // 调用IAP服务程序
	return paramout[1];
}


/****************************************************************************
* 名称：Compare()
* 功能：校验数据，命令代码56。
* 入口参数：dst        目标地址，即RAM/FLASH起始地址。地址必须字对齐
*          src        源地址，即FLASH/RAM地址。地址必须字对齐
*          no         复制字节个数，必须能被4整除
* 返回值：IAP返回值(paramout缓冲区)
****************************************************************************/
u32  Compare(u32 dst, u32 src, u32 no)
{
	paramin[0] = IAP_COMPARE;                // 设置命令字
	paramin[1] = dst;                        // 设置参数
	paramin[2] = src;
	paramin[3] = no;
	iap_entry(paramin, paramout);         // 调用IAP服务程序
	return paramout[0];
}

