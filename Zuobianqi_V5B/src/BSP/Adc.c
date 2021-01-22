#include "adc.h"

#define 	AD_ADPTER  0			//AD采集偏移校准值
#define		ADC_CLK	3000000		//AD采样时钟，不可超过4.5MHz
#define		ADC_CLKS	0		//AD精度，0=10位，1=9位...7=3位

// 建立1-70°对应的 10K B=3950 1% NTC电阻对应表，已经转换为寄存器对应表
u16 WenDuBuff[71]={
946,927,908,889,870,851,832,813,794,775,
757,738,720,702,684,666,648,631,614,597,
580,564,548,533,517,502,487,473,459,445,
432,419,406,393,381,369,358,347,336,325,
315,305,296,286,277,268,260,252,244,236,
228,221,214,207,201,194,188,182,176,171,
165,160,155,150,146,141,137,132,128,124,
};


//AD模块初始化
void AdcInit(void)
{
	PINSEL1 &= ~(BIT25|BIT24);
	PINSEL1 |= BIT24;	// AIN1;   用于加热片的温度检测
	PINSEL1 &= ~(BIT22|BIT23);
	PINSEL1 |= BIT22;	// AIN0;  用于排污泵过流检测
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

	for(i=0;i<7;i++)
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
	return sum/5;

} 


// 读取加热片温度值 10K,1%,B=3950,NTC，20K上拉5V，系统3.3V
u8 ReadTemperature(void)
{
	u32 WenduReg=0;
//	u16 ToWendudianzu=0;
//	u16 tmp=0;
	u8 m=0;
	u8 r_wendu;
	 WenduReg = GetAdcSmoothly(1);
	 if(WenduReg>946)	// 床垫温度过低，低于0°
	 	r_wendu=0;
	 if(WenduReg>=124)	// 床垫温度超过70°
	 	r_wendu=70;
	 for(m=0;m<70;m++)
	 {
		 if((WenduReg<=WenDuBuff[m])&&(WenduReg>WenDuBuff[m+1]))
			 {r_wendu =m+1;break;  }		 
	 }
	return r_wendu;	
}




