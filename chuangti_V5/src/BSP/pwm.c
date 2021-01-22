#include "pwm.h"
#include "rtthread.h"

//初始化
void PwmInit(void)
{
	PCONP |= BIT5;	//电源打开
	PINSEL1 = (PINSEL1&(~BIT11)) | BIT10;	//P0.21 作为pwm输出
	PINSEL0 = (PINSEL0&(~BIT14)) | BIT15;	//P0.7 作为pwm输出
	PWMPCR &= ~(BIT5);	 	// pwm5 单边沿触发
	PWMPCR &= ~(BIT2);	 	// pwm2 单边沿触发
								   
	PWMPCR |= BIT13;		// 使能pwm5输出
	PWMPCR |= BIT10;		// 使能pwm2输出

	PWMPR = PWM_PR;
	PWMMR0 = PWM_MR0;		//周期
	PWMMCR = BIT1;			//PWMMR0匹配时复位PWMTC
	PWMMR2 = 0;
	PWMMR5 = 0;
	PWMLER |= BIT2|BIT4;		//锁存寄存器2,4
	// PWM使能，计数器复位
	PWMTCR = BIT1|BIT3;
	PWMTC = 0;
	PWMPC = 0;
	PwmEnable();
}

//使能
void PwmEnable(void)
{
	PWMTCR = BIT3|BIT0;	
}

//匹配寄存器地址列表
static volatile unsigned long * const c_apdwPwmMr[]={
	&PWMMR0,
	&PWMMR1,
	&PWMMR2,
	&PWMMR3,
	&PWMMR4,
	&PWMMR5,
	&PWMMR6
};

//更改占空比，占空比范围 0-1000
void PwmChange(u32 dwNum,u32 dwDuty)
{
	*(c_apdwPwmMr[dwNum]) = dwDuty*PWM_MR0/1000;
	PWMLER |= 1<<dwNum;
}

#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(PwmChange, "更改[0]路PWM占空比为[1]")

#endif
