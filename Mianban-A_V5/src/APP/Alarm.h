#ifndef _ALARM_H_
#define _ALARM_H_

#define	ALARM_ERROR_LED(on)	    if(on){IO1SET = BIT17;}else{IO1CLR = BIT17;}
#define	ALARM_RUN_LED(on)		if(on){IO1SET= BIT18;}else{IO1CLR= BIT18;}

//#define	ALARM_ERROR_LED(on)	if(on){IO1CLR = BIT30;}else{IO1SET = BIT30;}
//#define	ALARM_RUN_LED(on)		if(on){IO1CLR= BIT25;}else{IO1SET= BIT25;}
#define	ALARM_BUZZER(on)		if(on){IO0CLR= BIT21;}else{IO0SET = BIT21;}
#define ALARM_SHUIWEI_LOW(on)   if(on){IO1SET= BIT16;}else{IO1CLR= BIT16;}
#define ALARM_WUWU_HIGN(on)   if(on){IO1SET= BIT19;}else{IO1CLR= BIT19;}

#define SET_SHUIWEN_LOW(on)  if(on){IO0SET= BIT24;}else{IO0CLR= BIT24;}
#define SET_SHUIWEN_MIDDLE(on)  if(on){IO0SET= BIT23;}else{IO0CLR= BIT23;}
#define SET_SHUIWEN_HIGH(on)  if(on){IO0SET= BIT22;}else{IO0CLR= BIT22;}

#define SET_NUANFEN_LOW(on)  if(on){IO0SET= BIT29;}else{IO0CLR= BIT29;}
#define SET_NUANFEN_MIDDLE(on)  if(on){IO0SET= BIT28;}else{IO0CLR= BIT28;}
#define SET_NUANFEN_HIGH(on)  if(on){IO0SET= BIT27;}else{IO0CLR= BIT27;}



// ±¨¾¯Âß¼­ ³õÊ¼»¯
void AlarmInit(void);
void WDInit(void);
u32 WDtimers(void);
void feedWD(void);



#endif



