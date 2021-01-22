#include "Common.h"

// 预分频数
#define	PWM_CYCLE		(10.0L/20)	//pwm周期，1khz,1ms
#define PWM_PR			0.L		//原来时钟的1/16
#define	PWM_MR0		((u32)((PWM_CYCLE*Fpclk)/(1000.L*(PWM_PR+1))))

//初始化
void PwmInit(void);
//使能
void PwmEnable(void);
//更改占空比
void PwmChange(u32 dwNum,u32 dwDuty);

