#include "Common.h"

// Ԥ��Ƶ��
#define	PWM_CYCLE		(10.0L/20)	//pwm���ڣ�1khz,1ms
#define PWM_PR			0.L		//ԭ��ʱ�ӵ�1/16
#define	PWM_MR0		((u32)((PWM_CYCLE*Fpclk)/(1000.L*(PWM_PR+1))))

//��ʼ��
void PwmInit(void);
//ʹ��
void PwmEnable(void);
//����ռ�ձ�
void PwmChange(u32 dwNum,u32 dwDuty);

