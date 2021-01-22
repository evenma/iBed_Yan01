#ifndef __GUI_CORE_H
#define	__GUI_CORE_H

#include "../Config.h"
#include "../Common.h"

// ˵�����ص� CallBack ʹ����дCB

// ��Ϣ���壬ע��������ڲ�Ҫ�������˳���Լ��������ݣ����������������
typedef enum
{
	MSG_NONE = 0,			// ����Ϣ����
	MSG_AUTO_CREATE,		// �����Զ���������������
	MSG_SIGNAL_CREATE,		// ���������źŶ�����
	MSG_USER_CREATE,		// �������û����������
	MSG_TIMEOUT_EXIT,		// ���泬ʱ�˳�
	MSG_SIGNAL_EXIT,		// ���������ź���ʧ���������������Ҫ���˳�
	MSG_USER_EXIT,			// �����û�Ҫ���˳�
	MSG_KEY = 100,			// ������Ϣ��ʼ���
	MSG_USER = 1000			// �û���Ϣ��ʼ���
}E_Message;



typedef void (*CB_FormCreate)(u32);		// ���崴��ʱ����
typedef void (*CB_FormExit)(u32);		// ��������ʱ����
typedef bool (*CB_FormEvent)(u32);		// �����¼���Ӧ���簴������Ϣ������true��ʾ�ڲ��Ѵ�������false��ʾ��Ҫ�׳��¼�
typedef void (*PF_FormDisplay)();		// ˢ����ʾ�����������Ե���
typedef void (*CB_FormActive)(bool);	// ���弤���¼�������true��ʾ���false��ʾ����Ǽ���״̬

#define FORM_CREATE(fm,msg)				if((fm)->pForm->cbCreate) {(fm)->pForm->cbCreate(msg);}
#define FORM_EXIT(fm,msg)				if((fm)->pForm->cbExit)	  {(fm)->pForm->cbExit(msg);}
#define FORM_ACTIVE(fm,active)			if((fm)->pForm->cbActive) {(fm)->pForm->cbActive(active);}
#define FORM_EVENT(fm,msg)				(((fm)->pForm->cbEvent)?(fm)->pForm->cbEvent(msg):false)
#define FORM_DISPLAY(fm)				if((fm)->pForm->pfDisplay) {(fm)->pForm->pfDisplay();}

// �������ͽṹ��
typedef struct
{
	u32 id;						// ����ID��Ӧ����ȫ��Ψһ
	u32 dwTimeout;				// ��ʱ�˳�ʱ�䣬0��ʾ����ʱ�˳�
	CB_FormCreate	cbCreate;		// ���崴��ʱ����
	CB_FormExit		cbExit;			// �����˳�ʱ����
	CB_FormActive	cbActive;		// ���弤���¼�������true��ʾ���false��ʾ����Ǽ���״̬
	CB_FormEvent	cbEvent;		// �¼���Ӧ���簴�����Լ������û��Զ�����Ϣ
	PF_FormDisplay	pfDisplay;		// ��ʾ����
}S_Form,*PS_Form;

// ����ڵ㣬��Բ˵��ṹ�ȼ���ע��˵��ṹ����ѭ������
typedef struct __S_FormNode
{
	PS_Form pForm;					// ��Ӧ����
	struct __S_FormNode* pFatherNode;		// ���˵�
	struct __S_FormNode* pSubNode;			// ����
	struct __S_FormNode* pNextNode;		// ��һ��
	struct __S_FormNode* pPreviousNode;	// ��һ��
//	u32 dwTimeout;				// ��ʱ�˳�ʱ�䣬0��ʾ����ʱ�˳�
}S_FormNode,*PS_FormNode;


// ѭ����ʾ����ָ�룬�����γ�һ������������
typedef struct __S_FormList
{
	PS_Form pForm;					// ��Ӧ����
	u32 dwTimeoutCounter;			// ��ʱ�˳�ʱ���������0��ʾ����ʱ�˳�
	struct __S_FormList* pNextNode;		// ��һ�ڵ�
}S_FormList,*PS_FormList;

// ���������ջ���¼���Ĵ���������ʾ
typedef struct __S_FormPop
{
	PS_Form pForm;					// ��Ӧ����
	u32 dwTimeoutCounter;			// ��ʱ�˳���������0��ʾ����ʱ�˳�
	struct __S_FormPop* pPreviousNode;		// ֮ǰ�ڵ�
}S_FormPop,*PS_FormPop;

// GUI ģ��������ڣ���λ����
#define GUI_TICK_CYCLE			(20ul)
// ѭ����ʾ������ÿ��������ʾʱ��
#define FORM_REPEAT_CYCLE			5000	// ѭ����ʾ������ʾʱ��Ϊ5s����λ����
#define FORM_REPEAT_CYCLE_COUNT		(FORM_REPEAT_CYCLE/GUI_TICK_CYCLE)	// ѭ����ʾ������ʾʱ��Ϊ����ֵ

#define GUI_DELAY_COUNTER_S(s)	(s*1000/GUI_TICK_CYCLE)	// �뼶��ʱ��������

extern u32 g_dwGuiCounter;		// һ�����õļ�������ÿ�ν��붼���1����ͬ��Ҳû���Ը��ݸ�ֵ������Ӧ�Ķ������Ĳ���

extern void GuiInit(void);
extern void GuiTick(void);
extern void GuiSetRootForm(const PS_FormNode form);
extern void GuiSetDefaultEvent(CB_FormEvent evt);
extern void GuiEventHandle(u32 msg);

// ��ʾ��Ϣ�Ի���
// ���� timeout
//	0=�˳�
//	-1= ���Զ��˳�
//	����=�˳���ʱ����
extern void GuiMessageBoxShow(u32 timeout);
// �ر���Ϣ�Ի���
extern void GuiMessageBoxClose(void);
// ��Ϣҳ�浼����ָ��ҳ��
extern void GuiNav(PS_FormNode fm);



#endif

