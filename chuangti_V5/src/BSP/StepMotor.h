#ifndef _STEP_MOTOR_H_
#define _STEP_MOTOR_H_

#include "Common.h"

// ���������ʼ��
void StepMotorInit(void);
//  ������������������������Ե���
void StepMotorStep(void);
// ���е�ָ��λ��
void StepMotoRunTo(s32 goal);

#define	SLOW_STEP	1050   // �м�λ
#define	FAST_STEP	1720   // B λ��
//#define	SLOW_STEP	10      // A λ�� 
//#define	FAST_STEP	860    //  �м�λ��


#endif

