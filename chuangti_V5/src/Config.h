#ifndef	__CONFIG_H
#define	__CONFIG_H

/********************************/
/*      ARM���������           */
/********************************/
//��һ������Ķ�

#include    "LPC214X.h"


//�����շ���CAN�˿�
#define	CAN_NUM	CAN1

#define ENABLE_DEBUG	1	//������Կ��أ��������ʱ���ӡ������Ϣ��ͬʱ��ͨ�����ڽ��н���

#ifdef WIN32
#define __irq
#endif


#endif
