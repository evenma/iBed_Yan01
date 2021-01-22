#include "Config.h"
#include "Common.h"


//模块初始化
extern void AdcInit(void);

//执行AD转换
extern u32 Adc(u32 dwNum);

// 平滑方式获取ADC值
extern u32 GetAdcSmoothly(u32 ch);

extern u8 ReadWenDu(void);

extern u8 ReadTemperature(void);