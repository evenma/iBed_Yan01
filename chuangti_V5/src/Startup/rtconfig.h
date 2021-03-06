/* RT-Thread config file */
#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

/* RT_NAME_MAX*/
#define RT_NAME_MAX	8

/* RT_ALIGN_SIZE*/
#define RT_ALIGN_SIZE	4

/* PRIORITY_MAX*/
#define RT_THREAD_PRIORITY_MAX	32

/* Tick per Second*/
#define RT_TICK_PER_SECOND	200

#define	RT_USING_TIMER_SOFT
#define	RT_TIMER_THREAD_STACK_SIZE	512
#define	RT_TIMER_TICK_PER_SECOND	(RT_TICK_PER_SECOND/2)
#define	RT_TIMER_THREAD_PRIO	20

/* SECTION: RT_DEBUG */
/* Thread Debug*/
/* #define RT_THREAD_DEBUG */

/* Using Hook*/
// #define RT_USING_HOOK

/* SECTION: IPC */
/* Using Semaphore*/
#define RT_USING_SEMAPHORE

/* Using Mutex*/
// #define RT_USING_MUTEX

/* Using Event*/
#define RT_USING_EVENT

/* Using Faset Event*/
/* #define RT_USING_FASTEVENT */

/* Using MailBox*/
#define RT_USING_MAILBOX

/* Using Message Queue*/
// #define RT_USING_MESSAGEQUEUE

/* SECTION: Memory Management */
/* Using Memory Pool Management*/
// #define RT_USING_MEMPOOL

/* Using Dynamic Heap Management*/
// #define RT_USING_HEAP

/* Using Small MM*/
#define RT_USING_SMALL_MEM

/* Using SLAB Allocator*/
/* #define RT_USING_SLAB */

/* SECTION: Device System */
/* Using Device System*/
#define RT_USING_DEVICE
#define RT_USING_UART1
#define	UART1_BAUDRATE	115200
//#define RT_USING_UART2
#define	UART2_BAUDRATE	2400
#define RT_UART_RX_BUFFER_SIZE	64



/* SECTION: Console options */
/* the buffer size of console*/
#define RT_CONSOLEBUF_SIZE	128

/* SECTION: FinSH shell options */
/* Using FinSH as Shell*/
#define RT_USING_FINSH
/* Using symbol table */
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION




#endif
