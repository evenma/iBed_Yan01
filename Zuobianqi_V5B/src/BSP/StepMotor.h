#ifndef _STEP_MOTOR_H_
#define _STEP_MOTOR_H_

#include "Common.h"

// 步进电机初始化
void StepMotorInit(void);
//  步进电机做步进动作，周期性调用
void StepMotorStep(void);
// 运行到指定位置
void StepMotoRunTo(s32 goal);

#define	SLOW_STEP	1050   // 中间位
#define	FAST_STEP	1720   // B 位置
//#define	SLOW_STEP	10      // A 位置 
//#define	FAST_STEP	860    //  中间位置


#endif

