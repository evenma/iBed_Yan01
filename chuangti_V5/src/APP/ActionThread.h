#ifndef _ACTION_THREAD_H_
#define _ACTION_THREAD_H_

#include <Common.h>

// ��������
typedef enum {
	ActionCmdNone = 0,		// �޶�����ֹͣ�������͸�λʱ
	BeiSheng,		// ����
	BeiJiang,		// ����
	TuiSheng,		// ����
	TuiJiang,		// �Ƚ�
	QiZuo,			// ����
	PingTang,		// ƽ��
	XinZangTangWei,	// ������λ
	BianMenQuanKai, // ���ſ�
	BianMenQuanGuan,// ���Ź�
	ZuoFanSheng,			// ����
	ZuoFanJiang,			// �󷭽�
	YouFanSheng,			// �ҷ���
	YouFanJiang,            // �ҷ���
	ZuoBian, 				// ����
	FuYuan,					// ��ԭ
	STOP,					// ֹͣ��
	LeftGuardUp,		// ������
	LeftGuardDown,		// ������
	RightGuardUp,		// �һ�����	
	RightGuardDown,		// �һ�����
	GuardUp,					// ���һ�����
	GuardDown,				// ���һ�����
	DiningUp,					// �������俿��
	DiningDown,				//�����������
}ActionCmd;

// ͨ��CAN���͵Ķ���
extern u8 g_actionSend[8] ;

// �����������
extern void ActionThreadEntry(void* parameter);
// ��ʼ��������
extern void ActionStartCmd(ActionCmd cmd);
// ֹͣ��������
extern void ActionStopCmd(ActionCmd cmd);

extern void BeiBu(u8 work,u8 dir);
extern void TuiBei(u8 work,u8 dir);
extern void adjustTable(void);	  
extern void DoStop(void);

extern void DoAllStop(void);

extern void WDInit(void);
extern u32 WDtimers(void);
void feedWD(void);
static int WaitTimeout(rt_int32_t timeout,rt_uint32_t set);
static void RestartTimer(void);

#endif


