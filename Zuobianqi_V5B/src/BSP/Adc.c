#include "adc.h"

#define 	AD_ADPTER  0			//AD�ɼ�ƫ��У׼ֵ
#define		ADC_CLK	3000000		//AD����ʱ�ӣ����ɳ���4.5MHz
#define		ADC_CLKS	0		//AD���ȣ�0=10λ��1=9λ...7=3λ

// ����1-70���Ӧ�� 10K B=3950 1% NTC�����Ӧ���Ѿ�ת��Ϊ�Ĵ�����Ӧ��
u16 WenDuBuff[71]={
946,927,908,889,870,851,832,813,794,775,
757,738,720,702,684,666,648,631,614,597,
580,564,548,533,517,502,487,473,459,445,
432,419,406,393,381,369,358,347,336,325,
315,305,296,286,277,268,260,252,244,236,
228,221,214,207,201,194,188,182,176,171,
165,160,155,150,146,141,137,132,128,124,
};


//ADģ���ʼ��
void AdcInit(void)
{
	PINSEL1 &= ~(BIT25|BIT24);
	PINSEL1 |= BIT24;	// AIN1;   ���ڼ���Ƭ���¶ȼ��
	PINSEL1 &= ~(BIT22|BIT23);
	PINSEL1 |= BIT22;	// AIN0;  �������۱ù������
}

//ִ��ADת��
u32 Adc(u32 dwNum)
{
   	AD0CR = (1 << dwNum)                     |	// SEL = 1 ��ѡ��ͨ��num
      (((Fpclk+ADC_CLK-1) / ADC_CLK - 1) << 8)		| 	// CLKDIV ��ת��ʱ��
      (0 << 16)                   				|	// BURST = 0 ���������ת������
      (ADC_CLKS << 17)                    		| 	// CLKS ����
      (1 << 21)                    				| 	// PDN = 1 �� ��������ģʽ(�ǵ���ת��ģʽ)
      (0 << 22)                    				| 	// TEST1:0 = 00 ����������ģʽ(�ǲ���ģʽ)
      (1 << 24)                    				| 	// START = 1 ������ADCת��
      (0 << 27);							// EDGE = 0 (CAP/MAT�����½��ش���ADCת��)
	while( (AD0GDR&0x80000000)==0 );	// �ȴ�ת������
	AD0CR &= ~(BIT24|BIT25|BIT26);	//ֹͣת��
	return ((AD0GDR>>6)&0X3FF)+AD_ADPTER;
}

// ƽ����ʽ��ȡADCֵ
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


// ��ȡ����Ƭ�¶�ֵ 10K,1%,B=3950,NTC��20K����5V��ϵͳ3.3V
u8 ReadTemperature(void)
{
	u32 WenduReg=0;
//	u16 ToWendudianzu=0;
//	u16 tmp=0;
	u8 m=0;
	u8 r_wendu;
	 WenduReg = GetAdcSmoothly(1);
	 if(WenduReg>946)	// �����¶ȹ��ͣ�����0��
	 	r_wendu=0;
	 if(WenduReg>=124)	// �����¶ȳ���70��
	 	r_wendu=70;
	 for(m=0;m<70;m++)
	 {
		 if((WenduReg<=WenDuBuff[m])&&(WenduReg>WenDuBuff[m+1]))
			 {r_wendu =m+1;break;  }		 
	 }
	return r_wendu;	
}




