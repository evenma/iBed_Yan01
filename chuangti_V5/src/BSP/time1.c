#include "time1.h"
#include "Sensor.h"

//timer1电源供电
void power_init(void)
{
	PCONP|=(1<<2);  //timer1电源
}
void __irq timer1_interrupt(void)
{

//	timer1_stop();
//	timer1_clear();
	
		T1IR |= 0x01;
	VICVectAddr=0x0;
}

//中断初始化
void timer1_int_init(void)
{
//	power_init();    //定时器部分供电	
	VICIntSelect=0;      //工作在irq方式
	VICVectAddr5=(unsigned int)timer1_interrupt;  //定时器1的中断函数
	VICVectCntl5=(0x01<<5)|0x05;   //使用通道5 并使能中断
	VICIntEnable=1<<5;
}

//定时器1 初始化    PCLK = 11.0592M/4  1ms=2765
void timer1_init(void)
{
	power_init();    //定时器1 供电
	
	T1TCR=0x00;
	T1PR=0x00;	// 0分频
//	T1MR0 = 18589;	// 1.5ms 采样一次 
	T1MR0 = 2765;	// 1ms 采样一次 
	T1MCR|=  (0x01<<0);; 	//使能中断复位停止三个功能 的匹配通道0
	T1CCR=0x00;	//禁止所有扑获功能	

	timer1_int_init();
}

//timer1定时器开始计时
void timer1_start(void)
{
	T1TCR=0x01;			//使能定时器计数
}

//timer1定时器清除定时寄存器
void timer1_clear(void)
{
	T1TC =0;
//	T0PC=0;

//	T0TCR|=(0x01<<1); //定时器复位
//	delay(1);
//	__asm{nop;nop;nop;nop;}
//	T0TCR&=(~(0x01<<1));
}
//timer1定时器停止计时
void timer1_stop(void)
{
	T1TCR&=(~(0x01<<0));  //停止定时器计数
	timer1_clear();
}


