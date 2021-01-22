#include "74HC165.h"
#include "Common.h"

#define HC165_EN_MASK		BIT6		// P0.6	������ͱ�ʾʹ��
#define HC165_CLK_MASK		BIT4		//P0.4	������������Ƴ�����
#define HC165_LOAD_MASK	BIT20//BIT7		//P0.7	������͵�ƽ���벢�����ݣ��ߵ�ƽ����
#define HC165_DATA_MASK	BIT5		//P0.5	���룬������������

#define SET_HC165_EN()		IO0SET = HC165_EN_MASK
#define CLR_HC165_EN()		IO0CLR = HC165_EN_MASK
#define SET_HC165_LOAD()	IO0SET = HC165_LOAD_MASK
#define CLR_HC165_LOAD()	IO0CLR = HC165_LOAD_MASK
#define GET_HC165_DATA()	(IO0PIN&HC165_DATA_MASK)
#define SET_HC165_CLK()		IO0SET = HC165_CLK_MASK
#define CLR_HC165_CLK()		IO0CLR = HC165_CLK_MASK


// ��ʼ�� 74HC165
void Hc165Init(void)
{
	// IO ����ѡ��
//	PINSEL1 &= ~(HC165_EN_MASK|HC165_CLK_MASK|HC165_LOAD_MASK|HC165_DATA_MASK);
	// IO ����ѡ��
	IO0DIR &= ~(HC165_DATA_MASK);
	IO0DIR |= (HC165_EN_MASK|HC165_CLK_MASK|HC165_LOAD_MASK);
	//����IO��ʼֵ
	CLR_HC165_LOAD();		//����ر�
	CLR_HC165_EN();		//ʹ��
}

/*  ��ȡ 74HC165 ������
	���� ��
		output: ���������
		len : ��Ҫ��ȡ�������ֽ���(������)
*/
void GetHc165Input(u8* output, u8 len)
{
	// ʱ��: 
	// 1�����棬�ߵ�ƽ����
	// 2��������λ����
	u8 i;
	u8 value = 0;
//	CLR_HC165_EN();		//ʹ��
	SET_HC165_LOAD();		//����
	while(len--)
	{
		for(i=0;i<8;i++)
		{
			if(GET_HC165_DATA())
			{
				value = (value<<1)+1;
			}
			else
			{
				value <<= 1;
			}
			//����ͳ�ʱ�ӣ���һ�ζ���ʱ��Ӧ���ͳ�ʱ��
			CLR_HC165_CLK();
			SET_HC165_CLK();
		}
		*output++ = value;
	}
//	SET_HC165_EN();		//ʹ�ܹر�
	CLR_HC165_LOAD();	// ����ر�
}








