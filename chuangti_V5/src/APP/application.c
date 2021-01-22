											
#include <rtthread.h>
#include <finsh.h>

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif

#include "CAN/IncludeCan.h"
#include "CAN/Can.h"
#include "lpc214x.h"
#include <CanAnalyzer.h>
#include <ActionThread.h>

#include "Sensor.h"

char thread_action_stack[2048];
struct rt_thread thread_action;


// 接收到CAN回调函数
extern rt_err_t CanRxInd(rt_device_t dev, rt_size_t size);

int rt_application_init()
{
		rt_kprintf("Software Version: iBed-ChuangTi-5.1.25 20210121 Yan Beta \n");
    rt_thread_init(&thread_action,
                   "act th",
                   ActionThreadEntry, RT_NULL,
                   &thread_action_stack[0], sizeof(thread_action_stack),
                   10, 10);
    rt_thread_startup(&thread_action);

	SensorInit();	// 传感器初始化

	WDInit();
								 
    rt_kprintf("\r\nenter list() to get function list!\r\n");

    return 0;
}

/*@}*/
