/********************************************************************
	Gui �û�����ܹ����Ĵ���
*********************************************************************/

#include "GuiCore.h"
#include "Key/Key.h"

u32 g_dwGuiCounter;		// һ�����õļ�������ÿ�ν��붼���1����ͬ��Ҳû���Ը��ݸ�ֵ������Ӧ�Ķ������Ĳ���

#define GUI_TICK (SYS_TIMER_CYCLE*10)	// ���ͽ����ˢ���ʿ���Ч����CPUռ����

static PS_FormNode m_psRootFrom;		// �����棬����ҳ�泬ʱ�󶼻᷵�ص�������
static CB_FormEvent m_cbDefaultEvent;	// Ĭ�ϻص���������ҳ�治��Ӧ�¼���Ϣʱ����

// ��Ϣ�Ի��������Ӧȷ���Լ����ذ�����Ϣ��ʵ���ϵ���Ϣ�Ի���������ȫ�ɵ����߻��ƣ��߼����ֲ����κλ�ͼ����
static u32 m_dwMessageCounter;		// ��Ϣ�Ի���!0=��ǰ����Ϣ�Ի���-1��ʾ���Զ��˳���ҳ��
// ��ʾ����ָ��
static PS_FormPop	m_psFormPop;		// ��ǰ����ʽҳ��
static PS_FormList	m_psFormList;		// ��ǰѭ��ҳ��
static PS_FormNode	m_psFormNode;		// ��ǰ��Ϣҳ��

// ��ʼ��
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

// Ѱ�ҵ�һ���˵��ע�������и������ǽ�ֹ�γɻ���
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

// Ѱ�����һ���˵��ע�������и������ǽ�ֹ�γɻ���
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

// ����ڵ㵽���������У������л��Զ��жϸýڵ��Ƿ�����ӵ�������
// �������������Ϊ��
// ����true��ʾ����ɹ�
bool GuiInsertList(PS_FormList head,PS_FormList node)
{
	PS_FormList nd = head;
	// ������Ч���ж�
	if ((NULL == head) || (NULL == node))
	{
		return false;
	}

	// ����Ƿ��Ѿ����б���
	do 
	{
		if (nd == node)
		{
			return false;
		}
		nd = nd->pNextNode;
	} while (nd != head);
	
	// ���뵽��ǰ�ڵ����һ�����
	node->pNextNode = head->pNextNode;
	head->pNextNode = node;
	node->dwTimeoutCounter = node->pForm->dwTimeout;	// �¼���Ľڵ�Ӧ�����ó�ʱ������
	return true;
}


// �ӻ���������ɾ���ڵ�
// �������������Ϊ��
// �����µ��б�ͷ
PS_FormList GuiDelList(PS_FormList head,PS_FormList node)
{
	PS_FormList nd = head;
	if ((NULL == head)||(NULL == node))
	{
		return head;
	}
	
	// ��Ҫɾ���б�ͷ�����
	if (node == head)
	{
		// ���ڵ�
		if (head->pNextNode == head)
		{
			head->pNextNode = NULL;
			return NULL;
		}

		// ��ȡ���һ���ڵ�
		while(nd->pNextNode!=head)
		{
			nd = nd->pNextNode;
		}
		nd->pNextNode = head->pNextNode;
		head->pNextNode = NULL;
		return nd->pNextNode;
	}
	else	// ����Ҫɾ���б�ͷ
	{
		//���Ҷ�Ӧ�Ľڵ�
		PS_FormList nd = head;
		// ��ȡ���һ���ڵ�
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

// ���ڵ�ѹ���ջ�������л��Զ��жϸýڵ��Ƿ�����ӵ�������
// �������������Ϊ��
// �����µĶ�ջ����
PS_FormPop GuiInsertPop(PS_FormPop head,PS_FormPop node)
{
	PS_FormPop nd = head;
	// ������Ч���ж�
	if ((NULL == head) || (NULL == node))
	{
		return head;
	}

	// ����Ƿ��Ѿ����б���
	do 
	{
		if (nd == node)
		{
			return head;
		}
		nd = nd->pPreviousNode;
	} while (NULL != nd);

	// ���뵽��ǰ�ڵ����һ�����
	node->pPreviousNode = head;
	node->dwTimeoutCounter = node->pForm->dwTimeout;	// �¼���Ľڵ�Ӧ�����ó�ʱ������
	return node;
}


// �Ӷ�ջ��ɾ���ڵ�
// �������������Ϊ��
// �����µĶ�ջͷ
PS_FormPop GuiDelPop(PS_FormPop head,PS_FormPop node)
{
	if ((NULL == head)||(NULL == node))
	{
		return head;
	}

	// ��Ҫɾ���б�ͷ�����
	if (node == head)
	{
		// ��ʾ�õ�������Ҫ�˳���
		head = head->pPreviousNode;
		node->pPreviousNode = NULL;
		return head;
	}
	else	// ����Ҫɾ���б�ͷ
	{
		//���Ҷ�Ӧ�Ľڵ�
		PS_FormPop nd = head;
		// ��ȡ���һ���ڵ�
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

// �����˳������Ļص�
static void FormExit()
{
	// Ҫ���˳�����
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

// ѭ����ʾ���������
static u32 m_dwRepeatFormCounter;
// ��Ϣҳ�������
static u32 m_dwFormCounter;

// GUI ��ʱ��������
void GuiTick()
{
	g_dwGuiCounter++;
	// ��������������߼�
	if (m_dwMessageCounter)
	{
		if ((m_dwMessageCounter!=-1 )&& (0==--m_dwMessageCounter))
		{
			FormExit();
		}
	}
	else if (NULL != m_psFormPop)
	{
		// ����ʽҳ����ʾ
		if (m_psFormPop->dwTimeoutCounter && (0 == --m_psFormPop->dwTimeoutCounter))
		{
			FORM_ACTIVE(m_psFormPop,false);
			FORM_EXIT(m_psFormPop,MSG_TIMEOUT_EXIT);
			// ɾ����Ӧ�Ĵ���
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
		// ѭ��ҳ����ʾ
		if (0==--m_dwRepeatFormCounter)
		{
			m_dwRepeatFormCounter = FORM_REPEAT_CYCLE_COUNT;	// ���ؼ���ֵ
			if (m_psFormList->dwTimeoutCounter && (0 == --m_psFormList->dwTimeoutCounter))
			{
				// ��ʾ����ˣ���Ҫɾ����ѭ��ҳ��
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
			else if (m_psFormList != m_psFormList->pNextNode)	// �ж���ڵ�����л������
			{
				FORM_ACTIVE(m_psFormList,false);
				m_psFormList = m_psFormList->pNextNode;
				FORM_ACTIVE(m_psFormList,true);
				FORM_DISPLAY(m_psFormList);
			}
			else
			{
				FORM_DISPLAY(m_psFormList);		// Ψһ��һ���ڵ㣬ֱ����ʾ����
			}
		}
		else
		{
			FORM_DISPLAY(m_psFormList);
		}
	}
	else if (NULL != m_psFormNode)
	{
		// ��Ϣҳ����ʾ
		if (m_dwFormCounter && (0 == --m_dwFormCounter))
		{
			if (m_psFormNode == m_psRootFrom)
			{
				// ���������棬ֱ�ӽ���ʱʱ����λ����󣬲������˳�
				m_dwFormCounter = 0;
				FORM_DISPLAY(m_psFormNode);
			}
			else
			{
				FORM_ACTIVE(m_psFormNode,false);
				FORM_EXIT(m_psFormNode,MSG_TIMEOUT_EXIT);
				// ��ʱ�˳���������
				m_psFormNode = m_psRootFrom;
				if (m_psFormNode)
				{
					m_dwFormCounter = 0;	// ������ĳ�ʱʱ��Ϊ�����
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
		m_dwFormCounter = 0;	// ������ĳ�ʱʱ��Ϊ�����
		FORM_CREATE(m_psFormNode,MSG_AUTO_CREATE);
		FORM_ACTIVE(m_psFormNode,true);
		FORM_DISPLAY(m_psFormNode);
	}
	else
	{
//		DbgPrintf("û����Ҫ��ʾ�Ĵ���\r\n");
	}
	// û���κ�ҳ����Ҫ��ʾ������²����κβ�������Һ�������ϻ���ʾ���һ����ʾ������
}

// ��Ϣҳ�浼����ָ��ҳ��
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
// Ĭ����Ϣҳ�浼����ť
static bool DefaultNodeHandle(u32 msg)
{
	DbgPrintf("DefaultNodeHandle,msg=%d\r\n",msg);
	// ���ȸ��³�ʱʱ��
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
PageSwitch:			// ������ҳ���л���Ļص�
	m_dwFormCounter = m_psFormNode->pForm->dwTimeout;
	FORM_CREATE(m_psFormNode,MSG_AUTO_CREATE);
	FORM_ACTIVE(m_psFormNode,true);
	FORM_DISPLAY(m_psFormNode);
	return true;
}

// ��Ϣ������
/*
	������ƣ���Ϣ���ݸ���㴰�壬�����㴰��û���¼���Ӧ��������Ӧ��������false������뵽Ĭ�ϻص�����
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

// ��ʾ��Ϣ�Ի���
// ���� timeout
//	0=�˳�
//	-1= ���Զ��˳�
//	����=�˳���ʱ����
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

// �ر���Ϣ�Ի���
void GuiMessageBoxClose()
{
	if (m_dwMessageCounter)
	{
		m_dwMessageCounter = 0;
		FormExit();
	}
}


