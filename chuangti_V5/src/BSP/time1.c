#include "time1.h"
#include "Sensor.h"

//timer1��Դ����
void power_init(void)
{
	PCONP|=(1<<2);  //timer1��Դ
}
void __irq timer1_interrupt(void)
{

//	timer1_stop();
//	timer1_clear();
	
		T1IR |= 0x01;
	VICVectAddr=0x0;
}

//�жϳ�ʼ��
void timer1_int_init(void)
{
//	power_init();    //��ʱ�����ֹ���	
	VICIntSelect=0;      //������irq��ʽ
	VICVectAddr5=(unsigned int)timer1_interrupt;  //��ʱ��1���жϺ���
	VICVectCntl5=(0x01<<5)|0x05;   //ʹ��ͨ��5 ��ʹ���ж�
	VICIntEnable=1<<5;
}

//��ʱ��1 ��ʼ��    PCLK = 11.0592M/4  1ms=2765
void timer1_init(void)
{
	power_init();    //��ʱ��1 ����
	
	T1TCR=0x00;
	T1PR=0x00;	// 0��Ƶ
//	T1MR0 = 18589;	// 1.5ms ����һ�� 
	T1MR0 = 2765;	// 1ms ����һ�� 
	T1MCR|=  (0x01<<0);; 	//ʹ���жϸ�λֹͣ�������� ��ƥ��ͨ��0
	T1CCR=0x00;	//��ֹ�����˻���	

	timer1_int_init();
}

//timer1��ʱ����ʼ��ʱ
void timer1_start(void)
{
	T1TCR=0x01;			//ʹ�ܶ�ʱ������
}

//timer1��ʱ�������ʱ�Ĵ���
void timer1_clear(void)
{
	T1TC =0;
//	T0PC=0;

//	T0TCR|=(0x01<<1); //��ʱ����λ
//	delay(1);
//	__asm{nop;nop;nop;nop;}
//	T0TCR&=(~(0x01<<1));
}
//timer1��ʱ��ֹͣ��ʱ
void timer1_stop(void)
{
	T1TCR&=(~(0x01<<0));  //ֹͣ��ʱ������
	timer1_clear();
}


