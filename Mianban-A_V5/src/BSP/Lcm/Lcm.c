/********************************************************************
**   filename:	lcm.c	Copyright   (c)   2010
**   author:	Weijie Gu
**	E-mail:	guwj1984@qq.com
**   date:	2010.05.13
**   discription:LCM驱动文件
**   version:  1.0.0.1
********************************************************************/
/********************************************************************
**   modifier:
**	date:
**	discription:
**	version:
********************************************************************/
#define _LCM_GLOBAL_
#include "../Common.h"
#include "Lcm.h"

#define Dis_X_MAX		(128-1)
#define Dis_Y_MAX		(64-1)

#define LCD_Ctrl_GPIO()		PINSEL0 &= ~(0|BIT7|BIT8|BIT11|BIT12|BIT13|BIT14)
#define LCD_Ctrl_Out()		IO0DIR |= (LCD_CS+LCD_SDA+LCD_SCK)//+LCD_RST)
#define LCD_Ctrl_In()		//IO0DIR &= ~(SPI_BUSY)

#define LCD_CS			BIT20
#define LCD_CS_SET()		IO0SET = LCD_CS
#define LCD_CS_CLR()		IO0CLR = LCD_CS

#define LCD_SDA			BIT19
#define LCD_SDA_SET()		IO0SET = LCD_SDA
#define LCD_SDA_CLR()		IO0CLR = LCD_SDA

#define LCD_SCK			BIT17
#define LCD_SCK_SET()		IO0SET = LCD_SCK
#define LCD_SCK_CLR()		IO0CLR = LCD_SCK

//RST与系统板相连
#define LCD_RST			//BIT8
#define LCD_RST_SET()		//IO0SET = LCD_RST
#define LCD_RST_CLR()		//IO0CLR = LCD_RST

#define NOP()				__asm{nop}

//========================================================================
// 文件名:  LCD_Dis.c
// 作  者: xinqiang Zhang(小丑)  (email: xinqiang@Mzdesign.com.cn)
// 日  期: 2009/02/10
// 描  述: MzLH04-12864 LCD显示模块驱动V1.0版
//			基本功能接口函数集
//
// 参  考: 无
// 版  本:
//      2009/02/10      First version    Mz Design
//========================================================================
unsigned char X_Witch=6;
unsigned char Y_Witch=10;
unsigned char X_Witch_cn=16;
unsigned char Y_Witch_cn=16;
unsigned char Dis_Zero=0;
//========================================================================
// 函数: void LCM_Init(void)
// 描述: LCD初始化程序，主要在里面完成端口初始化以及LCD模块的复位
// 参数: 无
// 返回: 无
// 备注:
// 版本:
//      2009/02/10      First version    Mz Design
//========================================================================
void LCM_Init(void)
{
	LCD_Ctrl_GPIO();
	LCD_Ctrl_Out();
	LCD_Ctrl_In();
	//SS和SCK预先设置为高电平
	LCD_SCK_SET();
	LCD_CS_SET();

	//复位LCD模块
	LCD_RST_CLR();
	DelayXms(3);
	LCD_RST_SET();
	DelayXms(30);		// 这个延时是实测的稳定值，小于该值会导致后面的命令有问题

}
//========================================================================
// 函数: void SPI_SSSet(unsigned char Status)
// 描述: 置SS线状态
// 参数: Status   =1则置高电平，=0则置低电平
// 返回: 无
// 版本:
//      2009/02/10      First version    Mz Design
//========================================================================
void SPI_SSSet(unsigned char Status)
{
	NOP();
	NOP();
	NOP();
	if (Status)				//判断是要置SS为低还是高电平？//SS置高电平
		LCD_CS_SET();
	else   //SS置低电平
		LCD_CS_CLR();
	NOP();
	NOP();							//适当插入一些空操作以保证SPI时钟速度小于2MHz
	NOP();
	NOP();							//适当插入一些空操作以保证SPI时钟速度小于2MHz
}
//========================================================================
// 函数: void SPI_Send(unsigned char Data)
// 描述: 通过串行SPI口输送一个byte的数据置模组
// 参数: Data 要传送的数据
// 返回: 无
// 版本:
//      2007/07/17      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void SPI_Send(unsigned char Data)
{
	unsigned char i=0;
	for (i=0;i<8;i++)
	{
		//SCK置低
		LCD_SCK_CLR();
		if (Data&0x80)
			LCD_SDA_SET();
		else LCD_SDA_CLR();
		//SCK上升沿触发串行数据采样
		LCD_SCK_SET();							//适当插入一些空操作以保证SPI时钟速度小于2MHz
		Data = Data<<1;							//数据左移一位
	}
	NOP();
	NOP();
	NOP();
	NOP();
}
//========================================================================
// 函数: void FontSet(unsigned char Font_NUM,unsigned char Color)
// 描述: ASCII字符字体设置
// 参数: Font_NUM 字体选择,以驱动所带的字库为准
//		 Color  文本颜色,仅作用于ASCII字库
// 返回: 无
// 备注:
// 版本:
//      2007/07/19      First version
//========================================================================
void FontSet(unsigned char Font_NUM,unsigned char Color)
{
	unsigned char ucTemp=0;
	if (Font_NUM==0)
	{
		X_Witch = 6;//7;
		Y_Witch = 10;
	}
	else
	{
		X_Witch = 8;
		Y_Witch = 16;
	}
	ucTemp = (Font_NUM<<4)|Color;
	//设置ASCII字符的字型
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(0x81);					//传送指令0x81
	SPI_Send(ucTemp);				//选择8X16的ASCII字体,字符色为黑色
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void FontMode(unsigned char Cover,unsigned char Color)
// 描述: 设置字符显示覆盖模式
// 参数: Cover  字符覆盖模式设置，0或1
//		 Color 覆盖模式为1时字符显示时的背景覆盖色
// 返回: 无
// 备注:
// 版本:
//      2008/11/27      First version
//========================================================================
void FontMode(unsigned char Cover,unsigned char Color)
{
	unsigned char ucTemp=0;
	ucTemp = (Cover<<4)|Color;
	//设置ASCII字符的字型
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(0x89);					//传送指令0x81
	SPI_Send(ucTemp);				//选择8X16的ASCII字体,字符色为黑色
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void FontSet_cn(unsigned char Font_NUM,unsigned char Color)
// 描述: 汉字库字符字体设置
// 参数: Font_NUM 字体选择,以驱动所带的字库为准
//		 Color  文本颜色,仅作用于汉字库
// 返回: 无
// 备注:
// 版本:
//      2007/07/19      First version
//========================================================================
void FontSet_cn(unsigned char Font_NUM,unsigned char Color)
{
	unsigned char ucTemp=0;
	if (Font_NUM==0)
	{
		X_Witch_cn = 12;
		Y_Witch_cn = 12;
	}
	else
	{
		X_Witch_cn = 16;
		Y_Witch_cn = 16;
	}
	ucTemp = (Font_NUM<<4)|Color;
	//设置ASCII字符的字型
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(0x82);					//传送指令0x81
	SPI_Send(ucTemp);				//选择8X16的ASCII字体,字符色为黑色
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void PutChar(unsigned char x,unsigned char y,unsigned char a)
// 描述: 写入一个标准ASCII字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 a  要显示的字符在字库中的偏移量
// 返回: 无
// 备注: ASCII字符可直接输入ASCII码即可
// 版本:
//      2007/07/19      First version
//========================================================================
void PutChar(unsigned char x,unsigned char y,unsigned char a)
{
	//显示ASCII字符
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(7);					//传送指令0x07
	SPI_Send(x);					//要显示字符的左上角的X轴位置
	SPI_Send(y);					//要显示字符的左上角的Y轴位置
	SPI_Send(a);					//要显示字符ASCII字符的ASCII码值
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void PutString(int x,int y,char *p)
// 描述: 在x、y为起始坐标处写入一串标准ASCII字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 p  要显示的字符串
// 返回: 无
// 备注: 仅能用于自带的ASCII字符串显示
// 版本:
//      2007/07/19      First version
//========================================================================
void PutString(unsigned char x,unsigned char y,const unsigned char *p)
{
	while (*p!=0)
	{
		PutChar(x,y,*p);
		x += X_Witch;
		if ((x + X_Witch) > Dis_X_MAX)
		{
			x = Dis_Zero;
			if ((Dis_Y_MAX - y) < Y_Witch) break;
			else y += Y_Witch;
		}
		p++;
	}
}
//========================================================================
// 函数: void PutChar_cn(unsigned char x,unsigned char y,unsigned short * GB)
// 描述: 写入一个二级汉字库汉字
// 参数: x  X轴坐标     y  Y轴坐标
//		 a  GB码
// 返回: 无
// 备注:
// 版本:
//      2007/07/19      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void PutChar_cn(unsigned char x,unsigned char y,const unsigned char * GB)
{
	//显示ASCII字符
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(8);					//传送指令0x08
	SPI_Send(x);					//要显示字符的左上角的X轴位置
	SPI_Send(y);					//要显示字符的左上角的Y轴位置

	SPI_Send(*(GB++));	//传送二级字库中汉字GB码的高八位值
	SPI_Send(*GB);		//传送二级字库中汉字GB码的低八位值
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void PutString_cn(unsigned char x,unsigned char y,unsigned short *p)
// 描述: 在x、y为起始坐标处写入一串汉字字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 p  要显示的字符串
// 返回: 无
// 备注: 同PutChar_cn中的解释
// 版本:
//      2007/07/19      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void PutString_cn(unsigned char x,unsigned char y,const unsigned char *p)
{
	while (*p!=0)
	{
		if (*p<128)
		{
			PutChar(x,y,*p);
			x += X_Witch+1;
			if ((x/* + X_Witch*/) > Dis_X_MAX)
			{
				x = Dis_Zero;
				if ((Dis_Y_MAX - y) < Y_Witch) break;
				else y += Y_Witch_cn;
			}
			p+=1;
		}
		else
		{
			PutChar_cn(x,y,p);
			x += X_Witch_cn+1;
			if ((x/* + X_Witch_cn*/) > Dis_X_MAX)
			{
				x = Dis_Zero;
				if ((Dis_Y_MAX - y) < Y_Witch_cn) break;
				else y += Y_Witch_cn;
			}
			p+=2;
		}
	}
}
//========================================================================
// 函数: void SetPaintMode(unsigned char Mode,unsigned char Color)
// 描述: 绘图模式设置
// 参数: Mode 绘图模式    Color  像素点的颜色,相当于前景色
// 返回: 无
// 备注: Mode无效
// 版本:
//      2007/07/19      First version
//========================================================================
void SetPaintMode(unsigned char Mode,unsigned char Color)
{
	unsigned char ucTemp=0;
	ucTemp = (Mode<<4)|Color;
	//设置绘图模式
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(0x83);					//传送指令0x83
	SPI_Send(ucTemp);				//选择8X16的ASCII字体,字符色为黑色
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void PutPixel(unsigned char x,unsigned char y)
// 描述: 在x、y点上绘制一个前景色的点
// 参数: x  X轴坐标     y  Y轴坐标
// 返回: 无
// 备注: 使用前景色
// 版本:
//      2007/07/19      First version
//========================================================================
void PutPixel(unsigned char x,unsigned char y)
{
	//绘点操作
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(1);					//送指令0x01
	SPI_Send(x);					//送第一个数据,即设置点的X轴位置
	SPI_Send(y);					//点的Y轴位置
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void Line(unsigned char s_x,unsigned char  s_y,
//					unsigned char  e_x,unsigned char  e_y)
// 描述: 在s_x、s_y为起始坐标，e_x、e_y为结束坐标绘制一条直线
// 参数: x  X轴坐标     y  Y轴坐标
// 返回: 无
// 备注: 使用前景色
// 版本:
//      2007/07/19      First version
//========================================================================
void Line(unsigned char s_x,unsigned char  s_y,unsigned char  e_x,unsigned char  e_y)
{
	//绘制直线
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(2);					//送指令0x02
	SPI_Send(s_x);					//起点X轴坐标
	SPI_Send(s_y);					//起点Y轴坐标
	SPI_Send(e_x);					//终点X轴坐标
	SPI_Send(e_y);					//终点Y轴坐标
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void Circle(unsigned char x,unsigned char y,
//					unsigned char r,unsigned char mode)
// 描述: 以x,y为圆心R为半径画一个圆(mode = 0) or 圆面(mode = 1)
// 参数:
// 返回: 无
// 备注: 画圆函数执行较慢，如果MCU有看门狗，请作好清狗的操作
// 版本:
//      2007/07/19      First version
//========================================================================
void Circle(unsigned char x,unsigned char y,unsigned char r,unsigned char mode)
{
	SPI_SSSet(0);
	if (mode)
		SPI_Send(6);
	else
		SPI_Send(5);
	SPI_Send(x);
	SPI_Send(y);
	SPI_Send(r);
	SPI_SSSet(1);
}
//========================================================================
// 函数: void Rectangle(unsigned char left, unsigned char top, unsigned char right,
//				 unsigned char bottom, unsigned char mode)
// 描述: 以x,y为圆心R为半径画一个圆(mode = 0) or 圆面(mode = 1)
// 参数: left - 矩形的左上角横坐标，范围0到126
//		 top - 矩形的左上角纵坐标，范围0到62
//		 right - 矩形的右下角横坐标，范围1到127
//		 bottom - 矩形的右下角纵坐标，范围1到63
//		 Mode - 绘制模式，可以是下列数值之一：
//				0:	矩形框（空心矩形）
//				1:	矩形面（实心矩形）
// 返回: 无
// 备注: 画圆函数执行较慢，如果MCU有看门狗，请作好清狗的操作
// 版本:
//      2007/07/19      First version
//========================================================================
void Rectangle(unsigned char left, unsigned char top, unsigned char right,
			   unsigned char bottom, unsigned char mode)
{
	SPI_SSSet(0);
	if (mode)
		SPI_Send(4);
	else
		SPI_Send(3);
	SPI_Send(left);
	SPI_Send(top);
	SPI_Send(right);
	SPI_Send(bottom);
	SPI_SSSet(1);
}
//========================================================================
// 函数: void ClrScreen(void)
// 描述: 清屏函数，执行全屏幕清除
// 参数:
// 返回: 无
// 备注:
// 版本:
//      2007/07/19      First version
//========================================================================
void ClrScreen(void)
{
	//清屏操作
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(0x80);					//送指令0x80
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void PutBitmap(unsigned char x,unsigned char y,unsigned char width,
//						unsigned char high,unsigned char *p)
// 描述: 在x、y为起始坐标显示一个单色位图
// 参数: x  X轴坐标     y  Y轴坐标
//		 width 位图的宽度
//		 high  位图的高度
//		 p  要显示的位图的字模首地址
// 返回: 无
// 备注: 无
// 版本:
//      2008/11/27      First version
//========================================================================
void PutBitmap(unsigned char x,unsigned char y,unsigned char width,unsigned char high,const unsigned char *p)
{
	unsigned short Dat_Num;
	//unsigned short i;
	unsigned char ucTemp=0;
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(0x0e);
	SPI_Send(x);
	SPI_Send(y);
	SPI_Send(width);
	SPI_Send(high);

	width = width+0x07;
	Dat_Num = (width>>3)*high;
	while (Dat_Num--)
	{
		ucTemp++;
		SPI_Send(*p);
		if (ucTemp==255)				//如果改换别的MCU时,这里可能需要适当的调整
		{
			DelayXms(9);			//以及此处,以保证前面传送到LCD模块中的数据已被处理完
			ucTemp = 0;
		}
		p++;
	}
	SPI_SSSet(1);					//完成操作置SS高电平
}

//========================================================================
// 函数: void ShowChar(unsigned char x,unsigned char y,unsigned char Num,unsigned char type)
// 描述: 在x、y的位置为显示区域的左上角，显示一个unsigned char型数
// 参数: x  X轴坐标     y  Y轴坐标    Num  要显示的8位宽度的数据  type  显示特性(0,1,2)
// 返回: 无
// 备注:
// 版本:
//      2008/11/27      First version
//========================================================================
void ShowChar(unsigned char x,unsigned char y,unsigned char a,unsigned char type)
{
	//显示ASCII字符
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(11);					//传送指令0x0B
	SPI_Send(x);					//要显示字符的左上角的X轴位置
	SPI_Send(y);					//要显示字符的左上角的Y轴位置
	SPI_Send(a);					//要显示的数据
	SPI_Send(type);
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void ShowShort(unsigned char x,unsigned char y,unsigned short Num,unsigned char type)
// 描述: 在x、y的位置为显示区域的左上角，显示一个unsigned short型数（16位宽度）
// 参数: x  X轴坐标     y  Y轴坐标    Num  要显示的16位宽度的数据  type  显示特性(0,1,2)
// 返回: 无
// 备注:
// 版本:
//      2008/11/27      First version
//========================================================================
void ShowShort(unsigned char x,unsigned char y,unsigned short a,unsigned char type)
{
	//显示ASCII字符
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(12);					//传送指令0x0C
	SPI_Send(x);					//要显示字符的左上角的X轴位置
	SPI_Send(y);					//要显示字符的左上角的Y轴位置
	SPI_Send((unsigned char)(a>>8));
	SPI_Send((unsigned char)a);
	SPI_Send(type);					//要显示字符ASCII字符的ASCII码值
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void SetBackLight(unsigned char Deg)
// 描述: 设置背光亮度等级
// 参数: Grade  0~127的等级
// 返回: 无
// 备注:
// 版本:
//      2008/11/27      First version
//========================================================================
void SetBackLight(unsigned char Deg)
{
	//显示ASCII字符
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(0x8a);					//传送指令0x07
	SPI_Send(Deg);					//要显示字符的左上角的X轴位置
	SPI_SSSet(1);					//完成操作置SS高电平
}


// 打印格式化信息到LCD中
void LcmPrintString(int x,int y,const char *fmt,...)
{
	char inf[26];
	va_list a;											//说明变量x
	if (NULL == fmt)
	{
		return;
	}
	va_start(a,fmt);
	vsprintf(inf,fmt,a);
// 	SetPaintMode(0,0);
// 	Rectangle(0,y,127,y+12,1);
// 	SetPaintMode(0,1);
	PutString_cn(x,y,(u8 *)inf);
	va_end(a);
	return;
}

void LcmCenterString(int y,const char *info)
{
	int len;
	int x;
	if (NULL==info)
	{
		return;
	}
	len=xokostrlen(info);
	x=(128-(len*8))/2;
	if(x<0) x=0;
// 	SetPaintMode(0,0);
// 	Rectangle(0,y,127,y+12,1);
// 	SetPaintMode(0,1);
	//PutString(0,y,(u8 *)"　　　　　　　　　　　");
	PutString_cn(x,y,(u8 *)info);
	return;
}

// 对指定矩形区域清屏
void LcmClearRectangle(unsigned char x,unsigned char y,unsigned char w,unsigned h)
{
	SetPaintMode(0,0);
	Rectangle(x,y,x+w-1,y+h-1,1);
	SetPaintMode(0,1);
}
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

