#include "pwm.h"
#include "rtthread.h"

//��ʼ��
void PwmInit(void)
{
	PCONP |= BIT5;	//��Դ��
	PINSEL1 = (PINSEL1&(~BIT11)) | BIT10;	//P0.21 ��Ϊpwm���
	PINSEL0 = (PINSEL0&(~BIT14)) | BIT15;	//P0.7 ��Ϊpwm���
	PWMPCR &= ~(BIT5);	 	// pwm5 �����ش���
	PWMPCR &= ~(BIT2);	 	// pwm2 �����ش���
								   
	PWMPCR |= BIT13;		// ʹ��pwm5���
	PWMPCR |= BIT10;		// ʹ��pwm2���

	PWMPR = PWM_PR;
	PWMMR0 = PWM_MR0;		//����
	PWMMCR = BIT1;			//PWMMR0ƥ��ʱ��λPWMTC
	PWMMR2 = 0;
	PWMMR5 = 0;
	PWMLER |= BIT2|BIT4;		//����Ĵ���2,4
	// PWMʹ�ܣ���������λ
	PWMTCR = BIT1|BIT3;
	PWMTC = 0;
	PWMPC = 0;
	PwmEnable();
}

//ʹ��
void PwmEnable(void)
{
	PWMTCR = BIT3|BIT0;	
}

//ƥ��Ĵ�����ַ�б�
static volatile unsigned long * const c_apdwPwmMr[]={
	&PWMMR0,
	&PWMMR1,
	&PWMMR2,
	&PWMMR3,
	&PWMMR4,
	&PWMMR5,
	&PWMMR6
};

//����ռ�ձȣ�ռ�ձȷ�Χ 0-1000
void PwmChange(u32 dwNum,u32 dwDuty)
{
	*(c_apdwPwmMr[dwNum]) = dwDuty*PWM_MR0/1000;
	PWMLER |= 1<<dwNum;
}

#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(PwmChange, "����[0]·PWMռ�ձ�Ϊ[1]")

#endif
