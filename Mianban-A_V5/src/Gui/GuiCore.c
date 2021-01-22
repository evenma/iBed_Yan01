/********************************************************************
	Gui 用户界面架构核心代码
*********************************************************************/

#include "GuiCore.h"
#include "Key/Key.h"

u32 g_dwGuiCounter;		// 一个公用的计数器，每次进入都会加1，不同的也没可以根据该值进行相应的动画或别的操作

#define GUI_TICK (SYS_TIMER_CYCLE*10)	// 降低界面的刷新率可有效减少CPU占有率

static PS_FormNode m_psRootFrom;		// 主界面，所有页面超时后都会返回到主界面
static CB_FormEvent m_cbDefaultEvent;	// 默认回调函数，当页面不响应事件消息时调用

// 消息对话框仅仅响应确认以及返回按键消息，实际上的消息对话框内容完全由调用者绘制，逻辑部分不做任何绘图操作
static u32 m_dwMessageCounter;		// 消息对话框，!0=当前有消息对话框，-1表示不自动退出的页面
// 显示界面指针
static PS_FormPop	m_psFormPop;		// 当前弹出式页面
static PS_FormList	m_psFormList;		// 当前循环页面
static PS_FormNode	m_psFormNode;		// 当前信息页面

// 初始化
void GuiInit(void)
{
	m_cbDefaultEvent = NULL;
	m_psFormNode = m_psRootFrom = NULL;
	m_psFormList = NULL;
	m_psFormPop = NULL;
	g_dwGuiCounter = 0;
	m_dwMessageCounter = 0;
}

void GuiSetRootForm(const PS_FormNode form)
{
	m_psRootFrom = form;
}

void GuiSetDefaultEvent(CB_FormEvent evt)
{
	m_cbDefaultEvent = evt;
}

// 寻找第一个菜单项，注意这里有个条件是禁止形成环链
PS_FormNode GuiFindFirstNode(PS_FormNode node)
{
	if (node==NULL)
	{
		return NULL;
	}
	while(NULL != node->pPreviousNode)
	{
		node = node->pPreviousNode;
	}
	return node;
}

// 寻找最后一个菜单项，注意这里有个条件是禁止形成环链
PS_FormNode GuiFindLastNode(PS_FormNode node)
{
	if (node==NULL)
	{
		return NULL;
	}
	while(NULL != node->pNextNode)
	{
		node = node->pNextNode;
	}
	return node;
}

// 插入节点到环形链表中，函数中会自动判断该节点是否已添加到链表中
// 输入参数不允许为空
// 返回true表示插入成功
bool GuiInsertList(PS_FormList head,PS_FormList node)
{
	PS_FormList nd = head;
	// 参数有效性判断
	if ((NULL == head) || (NULL == node))
	{
		return false;
	}

	// 检查是否已经在列表中
	do 
	{
		if (nd == node)
		{
			return false;
		}
		nd = nd->pNextNode;
	} while (nd != head);
	
	// 插入到当前节点的下一结点中
	node->pNextNode = head->pNextNode;
	head->pNextNode = node;
	node->dwTimeoutCounter = node->pForm->dwTimeout;	// 新加入的节点应该重置超时计数器
	return true;
}


// 从环形链表中删除节点
// 输入参数不允许为空
// 返回新的列表头
PS_FormList GuiDelList(PS_FormList head,PS_FormList node)
{
	PS_FormList nd = head;
	if ((NULL == head)||(NULL == node))
	{
		return head;
	}
	
	// 需要删除列表头的情况
	if (node == head)
	{
		// 单节点
		if (head->pNextNode == head)
		{
			head->pNextNode = NULL;
			return NULL;
		}

		// 获取最后一个节点
		while(nd->pNextNode!=head)
		{
			nd = nd->pNextNode;
		}
		nd->pNextNode = head->pNextNode;
		head->pNextNode = NULL;
		return nd->pNextNode;
	}
	else	// 不需要删除列表头
	{
		//查找对应的节点
		PS_FormList nd = head;
		// 获取最后一个节点
		while(nd->pNextNode!=head)
		{
			if (nd->pNextNode == node)
			{
				nd->pNextNode = node->pNextNode;
				node->pNextNode = NULL;
				return head;
			}
			nd = nd->pNextNode;
		}
		return head;
	}
}

// 将节点压入堆栈，函数中会自动判断该节点是否已添加到链表中
// 输入参数不允许为空
// 返回新的堆栈顶端
PS_FormPop GuiInsertPop(PS_FormPop head,PS_FormPop node)
{
	PS_FormPop nd = head;
	// 参数有效性判断
	if ((NULL == head) || (NULL == node))
	{
		return head;
	}

	// 检查是否已经在列表中
	do 
	{
		if (nd == node)
		{
			return head;
		}
		nd = nd->pPreviousNode;
	} while (NULL != nd);

	// 插入到当前节点的下一结点中
	node->pPreviousNode = head;
	node->dwTimeoutCounter = node->pForm->dwTimeout;	// 新加入的节点应该重置超时计数器
	return node;
}


// 从堆栈中删除节点
// 输入参数不允许为空
// 返回新的堆栈头
PS_FormPop GuiDelPop(PS_FormPop head,PS_FormPop node)
{
	if ((NULL == head)||(NULL == node))
	{
		return head;
	}

	// 需要删除列表头的情况
	if (node == head)
	{
		// 表示该弹出界面要退出了
		head = head->pPreviousNode;
		node->pPreviousNode = NULL;
		return head;
	}
	else	// 不需要删除列表头
	{
		//查找对应的节点
		PS_FormPop nd = head;
		// 获取最后一个节点
		while(nd->pPreviousNode!=NULL)
		{
			if (nd->pPreviousNode == node)
			{
				nd->pPreviousNode = node->pPreviousNode;
				node->pPreviousNode = NULL;
				return head;
			}
			nd = nd->pPreviousNode;
		}
		return head;
	}
}

// 窗体退出后做的回调
static void FormExit()
{
	// 要做退出操作
	if (m_psFormPop)
	{
		FORM_ACTIVE(m_psFormPop,true);
		FORM_DISPLAY(m_psFormPop);
	}
	else if (m_psFormList)
	{
		FORM_ACTIVE(m_psFormList,true);
		FORM_DISPLAY(m_psFormList);
	}
	else if (m_psFormNode)
	{
		FORM_ACTIVE(m_psFormNode,true);
		FORM_DISPLAY(m_psFormNode);
	}
}

// 循环显示界面计数器
static u32 m_dwRepeatFormCounter;
// 信息页面计数器
static u32 m_dwFormCounter;

// GUI 定时调用任务
void GuiTick()
{
	g_dwGuiCounter++;
	// 这里做窗体管理逻辑
	if (m_dwMessageCounter)
	{
		if ((m_dwMessageCounter!=-1 )&& (0==--m_dwMessageCounter))
		{
			FormExit();
		}
	}
	else if (NULL != m_psFormPop)
	{
		// 弹出式页面显示
		if (m_psFormPop->dwTimeoutCounter && (0 == --m_psFormPop->dwTimeoutCounter))
		{
			FORM_ACTIVE(m_psFormPop,false);
			FORM_EXIT(m_psFormPop,MSG_TIMEOUT_EXIT);
			// 删除相应的窗体
			m_psFormPop = GuiDelPop(m_psFormPop,m_psFormPop);
			FormExit();
		}
		else
		{
			FORM_DISPLAY(m_psFormPop);
		}
	}
	else if (NULL != m_psFormList)
	{
		// 循环页面显示
		if (0==--m_dwRepeatFormCounter)
		{
			m_dwRepeatFormCounter = FORM_REPEAT_CYCLE_COUNT;	// 重载计数值
			if (m_psFormList->dwTimeoutCounter && (0 == --m_psFormList->dwTimeoutCounter))
			{
				// 显示完毕了，需要删除该循环页面
				FORM_ACTIVE(m_psFormList,false);
				FORM_EXIT(m_psFormList,MSG_TIMEOUT_EXIT);
				m_psFormList = GuiDelList(m_psFormList,m_psFormList);
				if (m_psFormList)
				{
					FORM_ACTIVE(m_psFormList,true);
					FORM_DISPLAY(m_psFormList);
				}
				else if (m_psFormNode)
				{
					FORM_ACTIVE(m_psFormNode,true);
					FORM_DISPLAY(m_psFormNode);
				}
			}
			else if (m_psFormList != m_psFormList->pNextNode)	// 有多个节点进行切换的情况
			{
				FORM_ACTIVE(m_psFormList,false);
				m_psFormList = m_psFormList->pNextNode;
				FORM_ACTIVE(m_psFormList,true);
				FORM_DISPLAY(m_psFormList);
			}
			else
			{
				FORM_DISPLAY(m_psFormList);		// 唯一的一个节点，直接显示即可
			}
		}
		else
		{
			FORM_DISPLAY(m_psFormList);
		}
	}
	else if (NULL != m_psFormNode)
	{
		// 信息页面显示
		if (m_dwFormCounter && (0 == --m_dwFormCounter))
		{
			if (m_psFormNode == m_psRootFrom)
			{
				// 对于主界面，直接将超时时间置位无穷大，不允许退出
				m_dwFormCounter = 0;
				FORM_DISPLAY(m_psFormNode);
			}
			else
			{
				FORM_ACTIVE(m_psFormNode,false);
				FORM_EXIT(m_psFormNode,MSG_TIMEOUT_EXIT);
				// 超时退出到主界面
				m_psFormNode = m_psRootFrom;
				if (m_psFormNode)
				{
					m_dwFormCounter = 0;	// 主界面的超时时间为无穷大
					FORM_CREATE(m_psFormNode,MSG_AUTO_CREATE);
					FORM_ACTIVE(m_psFormNode,true);
					FORM_DISPLAY(m_psFormNode);
				}
			}
		}
		else
		{
			FORM_DISPLAY(m_psFormNode);
		}
	}
	else if (NULL != m_psRootFrom)
	{
		m_psFormNode = m_psRootFrom;
		m_dwFormCounter = 0;	// 主界面的超时时间为无穷大
		FORM_CREATE(m_psFormNode,MSG_AUTO_CREATE);
		FORM_ACTIVE(m_psFormNode,true);
		FORM_DISPLAY(m_psFormNode);
	}
	else
	{
//		DbgPrintf("没有需要显示的窗体\r\n");
	}
	// 没有任何页面需要显示的情况下不做任何操作，即液晶界面上会显示最后一次显示的内容
}

// 信息页面导航到指定页面
void GuiNav(PS_FormNode fm)
{
	if (NULL==fm)
	{
		return;
	}
	if (fm==m_psFormNode)
	{
		return;
	}
	if (NULL != m_psFormNode)
	{
		FORM_ACTIVE(m_psFormNode,false);
		FORM_EXIT(m_psFormNode,MSG_USER_EXIT);
		m_psFormNode = fm;
	}
	m_dwFormCounter = m_psFormNode->pForm->dwTimeout;
	FORM_CREATE(m_psFormNode,MSG_AUTO_CREATE);
	FORM_ACTIVE(m_psFormNode,true);
	FORM_DISPLAY(m_psFormNode);
}

#define PAGE_NAV(nav)	if((m_psFormNode->nav)&&(m_psFormNode->nav!=m_psFormNode))	{	\
							FORM_ACTIVE(m_psFormNode,false);				\
							FORM_EXIT(m_psFormNode,MSG_USER_EXIT);			\
							m_psFormNode = m_psFormNode->nav;	\
							goto PageSwitch;}					\
							return true;
// 默认信息页面导航按钮
static bool DefaultNodeHandle(u32 msg)
{
	DbgPrintf("DefaultNodeHandle,msg=%d\r\n",msg);
	// 首先更新超时时间
	m_dwFormCounter = m_psFormNode->pForm->dwTimeout;
	switch (msg)
	{
//	case KEY_UP:
	case  KEY_RETURN:
		PAGE_NAV(pFatherNode);
		break;
	case KEY_LEFT:
		PAGE_NAV(pPreviousNode);
		break;
	case KEY_RIGHT:
		PAGE_NAV(pNextNode);
		break;
//	case KEY_DOWN:
	case KEY_ENTER:
		PAGE_NAV(pSubNode);
		break;
	default:
		return false;
		break;
	}
PageSwitch:			// 这里做页面切换后的回调
	m_dwFormCounter = m_psFormNode->pForm->dwTimeout;
	FORM_CREATE(m_psFormNode,MSG_AUTO_CREATE);
	FORM_ACTIVE(m_psFormNode,true);
	FORM_DISPLAY(m_psFormNode);
	return true;
}

// 消息处理函数
/*
	处理机制：消息传递给最顶层窗体，如果最顶层窗体没有事件响应函数或响应函数返回false，则进入到默认回调函数
*/
void GuiEventHandle(u32 msg)
{
// 	DbgPrintf("GuiEventHandle,msg=%d\r\n",msg);
// 	DbgPrintf("m_dwFormCounter=%x\r\n",m_dwMessageCounter);
// 	DbgPrintf("m_psFormPop=%x\r\n",m_psFormPop);
// 	DbgPrintf("m_psFormList=%x\r\n",m_psFormList);
// 	DbgPrintf("m_psFormNode=%x\r\n",m_psFormNode);

	if (m_dwMessageCounter)
	{
		if ((msg == KEY_ENTER)||(msg == KEY_RETURN))
		{
			GuiMessageBoxClose();
			return;
		}
		goto CallDefaultEvent;
	}
	else if (NULL != m_psFormPop)
	{
		if (!FORM_EVENT(m_psFormPop,msg))
		{
// 			DbgPrintf("CallDefaultEvent,msg=%d,m_psFormPop=%x\r\n",msg,m_psFormPop);
			goto CallDefaultEvent;
		}
		return;
	}
	else if (NULL != m_psFormList)
	{
		if (!FORM_EVENT(m_psFormList,msg))
		{
// 			DbgPrintf("CallDefaultEvent,msg=%d,m_psFormList=%x\r\n",msg,m_psFormList);
			goto CallDefaultEvent;
		}
		return;
	}
	else if (NULL != m_psFormNode)
	{
		if ((msg>=MSG_KEY)&&(msg<MSG_USER))
		{
			m_dwFormCounter = m_psFormNode->pForm->dwTimeout;
		}
		if ((!FORM_EVENT(m_psFormNode,msg)) && (!DefaultNodeHandle(msg)))
		{
// 			DbgPrintf("CallDefaultEvent,msg=%d,m_psFormNode=%x\r\n",msg,m_psFormNode);
			goto CallDefaultEvent;
		}
		return;
	}

CallDefaultEvent:
//	DbgPrintf("CallDefaultEvent,msg=%d,m_psFormNode=%x\r\n",msg,m_psFormNode->pForm->cbEvent);
	if (m_cbDefaultEvent)
	{
		m_cbDefaultEvent(msg);
	}
}

// 显示消息对话框
// 参数 timeout
//	0=退出
//	-1= 不自动退出
//	其他=退出超时计数
void GuiMessageBoxShow(u32 timeout)
{
	if (m_dwMessageCounter!=0)
	{
		if (m_psFormPop)
		{
			FORM_ACTIVE(m_psFormPop,false);
		}
		else if (m_psFormList)
		{
			FORM_ACTIVE(m_psFormList,false);
		}
		else if (m_psFormNode)
		{
			FORM_ACTIVE(m_psFormNode,false);
		}
	}
	m_dwMessageCounter = timeout;
}

// 关闭消息对话框
void GuiMessageBoxClose()
{
	if (m_dwMessageCounter)
	{
		m_dwMessageCounter = 0;
		FormExit();
	}
}


