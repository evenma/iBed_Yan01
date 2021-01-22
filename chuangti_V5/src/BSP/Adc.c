#include "adc.h"

#define 	AD_ADPTER  0			//AD采集偏移校准值
#define		ADC_CLK	3000000		//AD采样时钟，不可超过4.5MHz
#define		ADC_CLKS	0		//AD精度，0=10位，1=9位...7=3位

//AD模块初始化
void AdcInit(void)
{
	PINSEL1 &= ~(BIT28|BIT29);
	PINSEL1 |= BIT28;	// AIN3;	  用于滑背和便门三推杆过流检测
	PINSEL1 &= ~(BIT26|BIT27);
	PINSEL1 |= BIT26;	// AIN2;	  用于腿部和右翻身推杆过流检测
	PINSEL1 &= ~(BIT25|BIT24);
	PINSEL1 |= BIT24;	// AIN1;   用于背部和左翻身推杆过流检测
	PINSEL1 &= ~(BIT22|BIT23);
	PINSEL1 |= BIT22;	// AIN0;  用于电平电量检测
	
}

//执行AD转换
u32 Adc(u32 dwNum)
{
   	AD0CR = (1 << dwNum)                     |	// SEL = 1 ，选择通道num
      (((Fpclk+ADC_CLK-1) / ADC_CLK - 1) << 8)		| 	// CLKDIV 即转换时钟
      (0 << 16)                   				|	// BURST = 0 ，软件控制转换操作
      (ADC_CLKS << 17)                    		| 	// CLKS 精度
      (1 << 21)                    				| 	// PDN = 1 ， 正常工作模式(非掉电转换模式)
      (0 << 22)                    				| 	// TEST1:0 = 00 ，正常工作模式(非测试模式)
      (1 << 24)                    				| 	// START = 1 ，启动ADC转换
      (0 << 27);							// EDGE = 0 (CAP/MAT引脚下降沿触发ADC转换)
	while( (AD0GDR&0x80000000)==0 );	// 等待转换结束
	AD0CR &= ~(BIT24|BIT25|BIT26);	//停止转换
	return ((AD0GDR>>6)&0X3FF)+AD_ADPTER;
}

// 平滑方式获取ADC值
u32 GetAdcSmoothly(u32 ch)
{
	u32 max=0;
	u32 min=0xffffffff;
	u32 sum = 0;
	u32 i;
	u32 result;

	for(i=0;i<10;i++)
	{
		result = Adc(ch);
		if(result>max)
		{
			max = result;
		}
		if(result<min)
		{
			min = result;
		}
		sum += result;
	}
	sum = sum - max - min;
	return sum/8;

}
