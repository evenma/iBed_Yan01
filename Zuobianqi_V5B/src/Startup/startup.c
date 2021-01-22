/*
 * File      : startup.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-02-16     Bernard      first implementation
 */

#include <rthw.h>
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#endif

#include "board.h"

/**
 * @addtogroup lpc2148
 */

/*@{*/

#ifdef RT_USING_FINSH
extern void finsh_system_init(void);
#endif

extern int  rt_application_init(void);
extern void rt_show_version(void);
#ifdef RT_USING_DEVICE
extern rt_err_t rt_hw_serial_init(void);
#endif
#ifdef RT_USING_FINSH
extern void finsh_system_init(void);
#endif

extern void rt_hw_can_init(void);

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#else
extern int __bss_end;
#endif

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
    /* init hardware interrupt */
    rt_hw_interrupt_init();

    /* init board */
    rt_hw_board_init();

    /* init tick */
    rt_system_tick_init();

    /* init kernel object */
    rt_system_object_init();

    /* init timer system */
    rt_system_timer_init();

#ifdef RT_USING_HEAP
#ifdef __CC_ARM
    rt_system_heap_init((void*)&Image$$RW_IRAM1$$ZI$$Limit, (void*)0x40004000);
#else
    /* init memory system */
    rt_system_heap_init((void*)&__bss_end, (void*)0x40004000);
#endif
#endif

    /* init scheduler system */
    rt_system_scheduler_init();

#ifdef RT_USING_HOOK /* if the hook is used */
    /* set idle thread hook */
    rt_thread_idle_sethook(0);
#endif

	rt_hw_can_init();

#ifdef RT_USING_DEVICE
    /* init hardware serial device */
    rt_hw_serial_init();
    /*init all registed devices*/
    rt_device_init_all();
#endif

	// 初始化软定时器
#ifdef RT_USING_TIMER_SOFT
	rt_system_timer_thread_init();
#endif

#ifdef RT_USING_FINSH
    /* init finsh */
    finsh_system_init();
    finsh_set_device("uart1");
#endif

//	rt_console_set_device("uart1");

    /* init idle thread */
    rt_thread_idle_init();

    rt_show_version();

    /* init application */
    rt_application_init();

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
    return ;
}

#include "lpc214x.h"
#include <common.h>

int main (void)
{
	PINSEL0 = 0;
	PINSEL1 = 0;
	PINSEL2 = 0;
	// 管脚输出PA 6,7,10,11,12,13,15,16,17,20,21,
	// dir=1,输出; dir=0,输入；
	// clr =1,输出低; set=1,输出高;
	// 默认全部输出低；
	// PA7 PWM2,PA21 PWM5; 
	// PA8,9 做RXD1,TXD1功能
	// 管脚输入PA2,3,4,23,29,30  系统默认为输入模式,无需设置
	// 特殊功能PA0,1,14,25,26,27,28,
	// p0.0 txd0 ;P0.1 rxd0; P0.25 can rd1 ;P0.26 can TD1; p0.28 ain1;P0.27 AIN0
	// 预留功能: PA5,18,19,22,24
	IO0DIR |= BIT6|BIT7|BIT10|BIT11|BIT12|BIT13|BIT15|BIT16|BIT17|BIT20|BIT21;
	IO0CLR |= BIT6|BIT7|BIT10|BIT11|BIT12|BIT13|BIT15|BIT16|BIT17|BIT20|BIT21;

	IO0DIR |= BIT18|BIT19|BIT22|BIT24;
	IO0CLR |= BIT18|BIT19|BIT22|BIT24;	
	// 管脚输出PB 21,22,23,24,25,27,26,28,29, 默认输出低
	// 管脚输入PB 16,17,18,19,	
	// 预留功能: PB 20,30
	// PB 16,26,特殊功能，通讯
	IO1DIR |= BIT21|BIT22|BIT23|BIT24|BIT25|BIT26|BIT27|BIT28|BIT30;
	IO1CLR |= BIT21|BIT22|BIT23|BIT24|BIT25|BIT27|BIT28|BIT30;
	IO1SET |= BIT26;	
	
	IO1DIR |= BIT20|BIT29;
	IO1CLR |= BIT20|BIT29;

	
     /* invoke rtthread_startup */
    rtthread_startup();

    return 0;
}

/*@}*/
