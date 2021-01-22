#ifndef	__CONFIG_H
#define	__CONFIG_H

/********************************/
/*      ARM的特殊代码           */
/********************************/
//这一段无需改动

#include    "LPC214X.h"


//用于收发的CAN端口
#define	CAN_NUM	CAN1

#define ENABLE_DEBUG	1	//允许调试开关，允许调试时会打印调试信息，同时可通过串口进行交互

#ifdef WIN32
#define __irq
#endif


#endif
