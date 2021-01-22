#include <rtthread.h>
#include <finsh.h>
#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#endif
#include "CAN/IncludeCan.h"
#include "CAN/Can.h"
#include "lpc214x.h"
#include <SmsApp.h>
#include <ActionThread.h>
#include "StepMotor.h"

#define	PHASE_A(on)	if(on){IO0SET = BIT24;}else{IO0CLR = BIT24;}
#define	PHASE_B(on)	if(on){IO0SET = BIT23;}else{IO0CLR = BIT23;}
#define	PHASE_C(on)	if(on){IO1SET = BIT19;}else{IO1CLR = BIT19;}
#define	PHASE_D(on)	if(on){IO1SET = BIT27;}else{IO1CLR = BIT27;}


// 当前步
s32 m_stepCurrent = 0;
// 目标步
s32 m_stepGoal=SLOW_STEP;

// 当前相位
static u8 m_currentPhase = 0;

// 步进电机初始化
void StepMotorInit(void)
{
	IO0DIR |= BIT24|BIT23;
	IO1DIR |= BIT19|BIT27;
	
	PHASE_C(0);
	PHASE_D(0);
	PHASE_A(1);
	PHASE_B(1);
}

u8 m_stepDiv = 0;
FINSH_VAR_EXPORT(m_stepDiv,finsh_type_uchar,"步进电机运动速度分频数，基准为200Hz");



//  步进电机做步进动作，周期性调用
void StepMotorStep(void)
{
	static u8 inited = 0;
	static u8 div=0;
	if(!inited)
	{
		if(m_stepCurrent != FAST_STEP)
		{
			m_stepCurrent++;
			m_currentPhase = (m_currentPhase+1)%4;
		}
		else
		{
			inited = 1;
			div = 0;
			m_stepDiv = 0;
		}
	}
	else
	{
		if(++div >= m_stepDiv)
		{
			div = 0;
			if(m_stepGoal==m_stepCurrent)
			{
				return;
			}
			else if(m_stepGoal>m_stepCurrent)	// 正转 
			{
				m_stepCurrent ++;
				m_currentPhase = (m_currentPhase+1)%4;
			}
			else	// 反转
			{
				m_stepCurrent --;
				if(m_currentPhase)
				{
					m_currentPhase--;
				}
				else
				{
					m_currentPhase = 3;
				}
			}
		}
		else
		{
			return;
		}
	}
	
	
	switch(m_currentPhase)
	{
		case 0:
			PHASE_C(0);
			PHASE_D(0);
			PHASE_A(1);
			PHASE_B(1);
			break;
		case 1:
			PHASE_D(0);
			PHASE_A(0);
			PHASE_B(1);
			PHASE_C(1);
			break;
		case 2:
			PHASE_A(0);
			PHASE_B(0);
			PHASE_C(1);
			PHASE_D(1);
			break;
		case 3:
			PHASE_B(0);
			PHASE_C(0);
			PHASE_D(1);
			PHASE_A(1);
			break;
	}
}

// 运行到指定位置
void StepMotoRunTo(s32 goal)
{
	if(goal <= FAST_STEP)
	{
		if(goal < m_stepCurrent)
		{
			m_stepDiv = 0;
		}
		else
		{
			m_stepDiv = 7;
		}
		m_stepGoal = goal;
	}
}


#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(StepMotoRunTo, "步进电机运行到指定角度");
FINSH_VAR_EXPORT(m_stepCurrent,finsh_type_int,"当前步进电机步数");
FINSH_VAR_EXPORT(m_stepGoal,finsh_type_int,"当前步进电机目标位置");
#endif


