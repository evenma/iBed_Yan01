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
#include <common.h>
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

	// ��ʼ����ʱ��
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

int main (void)
{
	PINSEL0 = 0;
	PINSEL1 = 0;
	PINSEL2 = 0;

	// �ܽ����PA4,5,6,10,11,12,13,15,16,17,18,19,20,22,23,24,
	// dir=1,���; dir=0,���룻
	// clr =1,�����; set=1,�����;
	// Ĭ��ȫ������ͣ�
	// PA7 PWM2,PA21 PWM5; ��������٣�����ֱ��Ĭ������ģʽ��
	// PA8,9 ��RXD1,TXD1���ܸ��ã��������źż��;
	// �ܽ�����PA2,3  ϵͳĬ��Ϊ����ģʽ
	// ���⹦��PA0,1,14,25,26,27,28,29,30
	// p0.0 txd0 ;P0.1 rxd0; P0.25 can rd1 ;P0.26 can TD1; 
	// P0.27 ain0;p0.28 ain1;p0.29 ain2; p0.30 ain3;
	IO0DIR |= BIT4|BIT5|BIT6|BIT10|BIT11|BIT12|BIT13|BIT15|BIT16|BIT17|BIT18|BIT19|BIT20|BIT22|BIT23|BIT24;
	IO0CLR |= BIT4|BIT5|BIT6|BIT10|BIT11|BIT12|BIT13|BIT15|BIT16|BIT17|BIT18|BIT19|BIT20|BIT22|BIT23|BIT24;

	// �ܽ����PB16,17,31 Ĭ�������
	// �ܽ�����PB18,19,20,21,22,23,24,25,26,27,28,29,30,	
	IO1DIR |= BIT16|BIT17|BIT31;
	IO1CLR |= BIT16|BIT17|BIT31;
		
    /* invoke rtthread_startup */
    rtthread_startup();

    return 0;
}

/*@}*/
