#ifndef __IAP_H_
#define __IAP_H_

#include "../Common.h"

/* 定义IAP返回状态字 */
#define  CMD_SUCCESS         	 	0
#define  INVALID_COMMAND      	1
#define  SRC_ADDR_ERROR       	2 
#define  DST_ADDR_ERROR       	3
#define  SRC_ADDR_NOT_MAPPED  	4
#define  DST_ADDR_NOT_MAPPED  	5
#define  COUNT_ERROR          		6
#define  INVALID_SECTOR       		7
#define  SECTOR_NOT_BLANK     	8
#define  SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION 9
#define  COMPARE_ERROR        	10
#define  BUSY                 			11
#define  PARAM_ERROR          		12 /* Insufficient number of parameters */
#define  ADDR_ERROR           		13 /* Address not on word boundary */
#define  ADDR_NOT_MAPPED      	14
#define  CMD_LOCKED          	 	15 /* Command is locked */
#define  INVALID_CODE         		16 /* Unlock code is invalid */
#define  INVALID_BAUD_RATE    	17
#define  INVALID_STOP_BIT     	18


/****************************************************************************
* 名称：SelSector()
* 功能：IAP操作扇区选择，命令代码50。
* 入口参数：sec1        起始扇区
*          sec2        终止扇区
* 返回值：IAP返回值(paramout缓冲区)
****************************************************************************/
u32  SelSector(u8 sec1, u8 sec2);

/****************************************************************************
* 名称：RamToFlash()
* 功能：复制RAM的数据到FLASH，命令代码51。
* 入口参数：dst        目标地址，即FLASH起始地址。以512字节为分界
*          src        源地址，即RAM地址。地址必须字对齐
*          no         复制字节个数，为512/1024/4096/8192
* 返回值：IAP返回值(paramout缓冲区)
****************************************************************************/
u32  RamToFlash(u32 dst, u32 src, u32 no);

/****************************************************************************
* 名称：EraseSector()
* 功能：扇区擦除，命令代码52。
* 入口参数：sec1       起始扇区
*          sec2       终止扇区
* 返回值：IAP返回值(paramout缓冲区)
****************************************************************************/
u32  EraseSector(u8 sec1, u8 sec2);

/****************************************************************************
* 名称：BlankCHK()
* 功能：扇区查空，命令代码53。
* 入口参数：sec1       起始扇区
*          sec2       终止扇区
* 返回值：IAP返回值(paramout缓冲区)
****************************************************************************/
u32  BlankCHK(u8 sec1, u8 sec2);

/****************************************************************************
* 名称：ReadParID()
* 功能：读器件ID
* 入口参数：无
* 返回值：器件ID
****************************************************************************/
u32  ReadParID(void);

/****************************************************************************
* 名称：BootCodeID()
* 功能：读取boot代码版本号，命令代码55。
* 入口参数：无
* 返回值：2字节boot版本号，解释为<字节1(主)>.<字节0(次)>
****************************************************************************/
u32  BootCodeID(void);

/****************************************************************************
* 名称：Compare()
* 功能：校验数据，命令代码56。
* 入口参数：dst        目标地址，即RAM/FLASH起始地址。地址必须字对齐
*          src        源地址，即FLASH/RAM地址。地址必须字对齐
*          no         复制字节个数，必须能被4整除
* 返回值：IAP返回值(paramout缓冲区)
****************************************************************************/
u32  Compare(u32 dst, u32 src, u32 no);

#endif
