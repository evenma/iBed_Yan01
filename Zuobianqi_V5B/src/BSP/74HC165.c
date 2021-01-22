#include "74HC165.h"
#include "Common.h"

#define HC165_EN_MASK		BIT6		// P0.6	输出，低表示使能
#define HC165_CLK_MASK		BIT4		//P0.4	输出，上升沿移出数据
#define HC165_LOAD_MASK	BIT20//BIT7		//P0.7	输出，低电平载入并行数据，高电平锁存
#define HC165_DATA_MASK	BIT5		//P0.5	输入，串行输入数据

#define SET_HC165_EN()		IO0SET = HC165_EN_MASK
#define CLR_HC165_EN()		IO0CLR = HC165_EN_MASK
#define SET_HC165_LOAD()	IO0SET = HC165_LOAD_MASK
#define CLR_HC165_LOAD()	IO0CLR = HC165_LOAD_MASK
#define GET_HC165_DATA()	(IO0PIN&HC165_DATA_MASK)
#define SET_HC165_CLK()		IO0SET = HC165_CLK_MASK
#define CLR_HC165_CLK()		IO0CLR = HC165_CLK_MASK


// 初始化 74HC165
void Hc165Init(void)
{
	// IO 功能选择
//	PINSEL1 &= ~(HC165_EN_MASK|HC165_CLK_MASK|HC165_LOAD_MASK|HC165_DATA_MASK);
	// IO 方向选择
	IO0DIR &= ~(HC165_DATA_MASK);
	IO0DIR |= (HC165_EN_MASK|HC165_CLK_MASK|HC165_LOAD_MASK);
	//设置IO初始值
	CLR_HC165_LOAD();		//锁存关闭
	CLR_HC165_EN();		//使能
}

/*  获取 74HC165 的输入
	参数 ：
		output: 输出缓冲区
		len : 需要获取的数据字节数(级联数)
*/
void GetHc165Input(u8* output, u8 len)
{
	// 时序: 
	// 1、锁存，高电平锁存
	// 2、串行移位输入
	u8 i;
	u8 value = 0;
//	CLR_HC165_EN();		//使能
	SET_HC165_LOAD();		//锁存
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
			//最后送出时钟，第一次读的时候不应该送出时钟
			CLR_HC165_CLK();
			SET_HC165_CLK();
		}
		*output++ = value;
	}
//	SET_HC165_EN();		//使能关闭
	CLR_HC165_LOAD();	// 锁存关闭
}








