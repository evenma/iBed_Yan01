#ifndef __KEY_H
#define __KEY_H

#include "../Common.h"
#include "../Gui/GuiCore.h"


//��������
typedef enum
{
	// ���ּ�
	KEY_UP = MSG_KEY,	 // ȡ�� ���ü�����ѭ��������		
	KEY_LEFT,	  // = + ��
	KEY_ENTER,    // = ���ü�
	KEY_RIGHT,	  // = - ��
	KEY_DOWN,	  // ȡ�� ���ü�����ѭ��������
	KEY_SET,	  // = ȷ����
	KEY_RETURN,   // �Զ�����
	
	KEY_BEIBAN_UP,
	KEY_BEIBAN_DOWN,
	KEY_TUIBU_UP,
	KEY_TUIBU_DOWN,
	KEY_FANSHEN_ZUO_UP,   
	KEY_FANSHEN_ZUO_DOWN,
	KEY_FANSHEN_YOU_UP,   
	KEY_FANSHEN_YOU_DOWN,	
	KEY_BIANMEN_KAI,
	KEY_BIANMEN_GUAN,
	KEY_QIZUO,
	KEY_PINGTANG,
	KEY_XINZANGTANGWEI,
	
	KEY_CHONGXIBIANPEN,
	KEY_GANZAO,
	KEY_QINGXITUNBU,
	KEY_QINGXISHENGZHIQI,
	KEY_ZUOBIAN_KAISHI,
	KEY_ZUOBIAN_JIESHU,
	
	KEY_PAD_UP,
	KEY_PAD_DOWN,
		
	KEY_SET_SHUIWEN,
	KEY_SET_NUANFEN_SUB,
	KEY_SET_NUANFEN_ADD,

	KEY_LOCK,	// ������
	KEY_CANCEL,	// ֹͣ������ȡ������
	KEY_RELEASE,		// �����ɿ�
}E_KeyMessage;

extern s32 g_KeyMsgDown;	// ��ǰ�����µİ�����Ϣ

extern void KeyInit(void);	// ������ʼ��
extern void KeyTick(void);	// ����ɨ�趨ʱ��ʾ


#endif
