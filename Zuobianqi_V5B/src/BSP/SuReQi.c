//文件引用
#include <rtthread.h>
#include "Sensor.h"

#include "Common.h"
#include "SuReQi.H"
//#include <Sensor.h>

//u8 SetTempt=0x7C;  // 0X72为37度;0X7C 为38 度;0x90 为40度；0x5E 为35度
rt_device_t uart2;

#define SIZEOFONEFRAME  13 
void Dump(u8* addr,u32 len)
{
#if ENABLE_DEBUG
	char str[16];
	u32 i;
	rt_kprintf("内存数据打印，起始地址：0X%X , 长度 %d",addr,len);
	if ((((u32)addr)&(0XF)) != 0)
	{
		sprintf(str,"%08X: ",((u32)addr)&(~0XF));
		rt_kprintf("\r\n0X%s",str);
		for (i = ((u32)addr)&(~0XF); i < (u32)addr;i++)
		{
			rt_kprintf("   ");
		}
	}
	while(len--)
	{
		if ((((u32)addr)&(0XF)) == 0)
		{
			sprintf(str,"%08X: ",addr);
			rt_kprintf("\r\n0X%s",str);
		}
		rt_kprintf(" %c%c",C_aAsciiTable[*addr/16],C_aAsciiTable[*addr%16]);
		addr++;
	}
	rt_kprintf("\r\n打印完毕\r\n");
#endif
}


// 速热器通讯初始化
void SuReQiInit(void)
{
	uart2 = rt_device_find("uart2");
	rt_device_open(uart2, RT_DEVICE_OFLAG_RDWR);
}

// 设置速热器参数
// 水温的输出温度与设定温度、输入水温、输出流量有关系
// 速热器型号 IHM-2219SS C       1900W
// <20°为冷水浴
// 38--40°为热水浴
// 34°左右为温水浴
// 保存一年四季恒温在37°比较理想
void SuReQiSet(u8 work,u8 Tset)
{

	u8 TsetHigh=0,i,bufdata;
	u8 TsetLow=0;
	u16 tmp;
// buf[3]=work,buf[4]=Tset*10(H),buf[5]=Tset*10(L);
   u8 buf[SIZEOFONEFRAME] = {0x32,0xF0,0X81,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X34};
	u8 rcv;
	while(1==rt_device_read(uart2, 0, &rcv, 1))	// 读空
	{
		rt_kprintf("%X\t",(unsigned int)rcv);
	}
	rt_kprintf("uart2\r\n");
	
	rt_kprintf("\t工作模式: %d\r\n",(u32)work);
	rt_kprintf("\t设定水温: %d\r\n",(u32)Tset);
	if(0==Tset)
		work=0;
	tmp = Tset*10;
	TsetHigh= tmp>>8; 	// 取高8位
	tmp = tmp&0x00ff;		// 取低8位
	TsetLow = (u8)tmp;
	buf[3] = work;
	buf[4] = TsetHigh;
	buf[5] = TsetLow;
	bufdata = buf[1];
	for(i=2;i<10;i++)
	{
		bufdata =bufdata ^ buf[i];   // XOR 
	}
	buf[11] = bufdata;	

//	uart2 = rt_device_find("uart2");
//	rt_device_open(uart2, RT_DEVICE_OFLAG_RDWR);	
	rcv= rt_device_write(uart2, 0, buf,SIZEOFONEFRAME);
//	SuReQiRead(buf);
}


// 读取速热器参数
BOOL SuReQiRead(u8* canshu )
{
	u8 rcv,i;
	u8 buf[13]={0,};
	u32 tmptin=0;   // 输入水温
	u32 tmptout=0;  // 出水温度
	u8 tmp;
//	uart2 = rt_device_find("uart2");
//	rt_device_open(uart2, RT_DEVICE_OFLAG_RDWR);	
	rcv = rt_device_read(uart2, 0, buf, SIZEOFONEFRAME);   // 5mS内接收
//	DbgPrintf("打印接收数据\r\n");	
//	Dump(buf,rcv);
//	rt_kprintf("\r\n");
	if(!buf[3])		// 工作模式
	{
		rt_kprintf("Sureqi no work\r\n");
	}
	if(buf[4])		// 错误模式
	{
		rt_kprintf("Sureqi work error=%d\r\n",buf[4]);
	}
	while(1==rt_device_read(uart2, 0, &rcv, 1))	// 读空
	{
		rt_kprintf("%X\t",(unsigned int)rcv);
	}
//	rt_kprintf("uart2\r\n");
	tmp = 0;
	for(i=1;i<11;i++)
	{
		tmp =tmp ^ buf[i];   // XOR 
	}
	buf[11] = tmp;	
	if((buf[0]==0x32)&&(buf[1]==0x0f)&&(buf[2]==0x01)&&(buf[11]==tmp)&&(buf[12]==0x34))// 表示通讯成功
	{
		*canshu = buf[3];		// 1 工作模式
		canshu+=1;		
		*canshu = buf[4];		// 2 错误模式
		canshu+=1;
		tmptin=(buf[5]<<8);
		tmptin+=buf[6];
		tmptin=(int)(tmptin/10); 
		*canshu = tmptin;		// 3 读取进水温度
		canshu+=1;		
		tmptout=(buf[7]<<8);
		tmptout+=buf[8];
		tmptout=(int)(tmptout/10);	
		*canshu = tmptout;		// 4 读取出水温度
		canshu+=1;	
		*canshu = buf[9];		// 5 流量 高位
		canshu+=1;	
		*canshu = buf[10];		// 6 流量 低位
	
//		rt_kprintf("\t工作模式: %d\r\n",(u32)buf[3]);
//		rt_kprintf("\t速热器错误Error: %d\r\n",(u32)buf[4]);
		rt_kprintf("\t冷水水温: %d\r\n",(u32)tmptin);
		rt_kprintf("\t出水水温: %d\r\n",(u32)tmptout);	
		rt_kprintf("\t水流量: %d cc/min\r\n",(u32)(buf[9]<<8)+buf[10]); 

		return 1;
	}
	else
		return 0;
}

#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(SuReQiSet, "设置速热器参数[][]");
#endif
