#include "adc.h"

#define 	AD_ADPTER  0			//AD�ɼ�ƫ��У׼ֵ
#define		ADC_CLK	3000000		//AD����ʱ�ӣ����ɳ���4.5MHz
#define		ADC_CLKS	0		//AD���ȣ�0=10λ��1=9λ...7=3λ

//ADģ���ʼ��
void AdcInit(void)
{
	PINSEL1 &= ~(BIT28|BIT29);
	PINSEL1 |= BIT28;	// AIN3;	  ���ڻ����ͱ������Ƹ˹������
	PINSEL1 &= ~(BIT26|BIT27);
	PINSEL1 |= BIT26;	// AIN2;	  �����Ȳ����ҷ����Ƹ˹������
	PINSEL1 &= ~(BIT25|BIT24);
	PINSEL1 |= BIT24;	// AIN1;   ���ڱ����������Ƹ˹������
	PINSEL1 &= ~(BIT22|BIT23);
	PINSEL1 |= BIT22;	// AIN0;  ���ڵ�ƽ�������
	
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
