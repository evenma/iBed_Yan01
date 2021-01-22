#ifndef __GUI_CORE_H
#define	__GUI_CORE_H

#include "../Config.h"
#include "../Common.h"

// 说明：回调 CallBack 使用缩写CB

// 消息定义，注意这里后期不要随意更改顺序以及插入数据，尽量往后面接数据
typedef enum
{
	MSG_NONE = 0,			// 空消息参数
	MSG_AUTO_CREATE,		// 界面自动创建，如主界面
	MSG_SIGNAL_CREATE,		// 界面由于信号而创建
	MSG_USER_CREATE,		// 界面由用户请求而创建
	MSG_TIMEOUT_EXIT,		// 界面超时退出
	MSG_SIGNAL_EXIT,		// 界面由于信号消失或者条件不满足而要求退出
	MSG_USER_EXIT,			// 界面用户要求退出
	MSG_KEY = 100,			// 按键消息开始编号
	MSG_USER = 1000			// 用户消息开始编号
}E_Message;



typedef void (*CB_FormCreate)(u32);		// 窗体创建时调用
typedef void (*CB_FormExit)(u32);		// 窗体销毁时调用
typedef bool (*CB_FormEvent)(u32);		// 窗体事件响应，如按键等消息，返回true表示内部已处理，返回false表示需要抛出事件
typedef void (*PF_FormDisplay)();		// 刷新显示函数，周期性调用
typedef void (*CB_FormActive)(bool);	// 窗体激活事件，参数true表示激活，false表示进入非激活状态

#define FORM_CREATE(fm,msg)				if((fm)->pForm->cbCreate) {(fm)->pForm->cbCreate(msg);}
#define FORM_EXIT(fm,msg)				if((fm)->pForm->cbExit)	  {(fm)->pForm->cbExit(msg);}
#define FORM_ACTIVE(fm,active)			if((fm)->pForm->cbActive) {(fm)->pForm->cbActive(active);}
#define FORM_EVENT(fm,msg)				(((fm)->pForm->cbEvent)?(fm)->pForm->cbEvent(msg):false)
#define FORM_DISPLAY(fm)				if((fm)->pForm->pfDisplay) {(fm)->pForm->pfDisplay();}

// 窗体类型结构体
typedef struct
{
	u32 id;						// 窗体ID，应当是全局唯一
	u32 dwTimeout;				// 超时退出时间，0表示不超时退出
	CB_FormCreate	cbCreate;		// 窗体创建时调用
	CB_FormExit		cbExit;			// 窗体退出时调用
	CB_FormActive	cbActive;		// 窗体激活事件，参数true表示激活，false表示进入非激活状态
	CB_FormEvent	cbEvent;		// 事件响应，如按键等以及其他用户自定义消息
	PF_FormDisplay	pfDisplay;		// 显示函数
}S_Form,*PS_Form;

// 窗体节点，针对菜单结构等级，注意菜单结构不能循环引用
typedef struct __S_FormNode
{
	PS_Form pForm;					// 对应窗体
	struct __S_FormNode* pFatherNode;		// 父菜单
	struct __S_FormNode* pSubNode;			// 子项
	struct __S_FormNode* pNextNode;		// 下一项
	struct __S_FormNode* pPreviousNode;	// 上一项
//	u32 dwTimeout;				// 超时退出时间，0表示不超时退出
}S_FormNode,*PS_FormNode;


// 循环显示窗体指针，必须形成一个单向环形链表
typedef struct __S_FormList
{
	PS_Form pForm;					// 对应窗体
	u32 dwTimeoutCounter;			// 超时退出时间计数器，0表示不超时退出
	struct __S_FormList* pNextNode;		// 下一节点
}S_FormList,*PS_FormList;

// 弹出窗体堆栈，新激活的窗体优先显示
typedef struct __S_FormPop
{
	PS_Form pForm;					// 对应窗体
	u32 dwTimeoutCounter;			// 超时退出计数器，0表示不超时退出
	struct __S_FormPop* pPreviousNode;		// 之前节点
}S_FormPop,*PS_FormPop;

// GUI 模块调用周期，单位毫秒
#define GUI_TICK_CYCLE			(20ul)
// 循环显示窗体中每个窗体显示时间
#define FORM_REPEAT_CYCLE			5000	// 循环显示界面显示时间为5s，单位毫秒
#define FORM_REPEAT_CYCLE_COUNT		(FORM_REPEAT_CYCLE/GUI_TICK_CYCLE)	// 循环显示界面显示时间为计数值

#define GUI_DELAY_COUNTER_S(s)	(s*1000/GUI_TICK_CYCLE)	// 秒级延时计数个数

extern u32 g_dwGuiCounter;		// 一个公用的计数器，每次进入都会加1，不同的也没可以根据该值进行相应的动画或别的操作

extern void GuiInit(void);
extern void GuiTick(void);
extern void GuiSetRootForm(const PS_FormNode form);
extern void GuiSetDefaultEvent(CB_FormEvent evt);
extern void GuiEventHandle(u32 msg);

// 显示消息对话框
// 参数 timeout
//	0=退出
//	-1= 不自动退出
//	其他=退出超时计数
extern void GuiMessageBoxShow(u32 timeout);
// 关闭消息对话框
extern void GuiMessageBoxClose(void);
// 信息页面导航到指定页面
extern void GuiNav(PS_FormNode fm);



#endif

