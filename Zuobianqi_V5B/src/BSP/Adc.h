#include "Config.h"
#include "Common.h"


//ģ���ʼ��
extern void AdcInit(void);

//ִ��ADת��
extern u32 Adc(u32 dwNum);

// ƽ����ʽ��ȡADCֵ
extern u32 GetAdcSmoothly(u32 ch);

extern u8 ReadWenDu(void);

extern u8 ReadTemperature(void);