//�ļ�����
#include <rtthread.h>

#include "Common.h"
#include "SuReQi.H"
#include <Sensor.h>

static u8 SetTempt=0x90;  // 0x90 Ϊ40�ȣ�0x5E Ϊ35��


// ��������ʼ��
void SuReQiInit()
{

}


void Dump(u8* addr,u32 len)
{
#if ENABLE_DEBUG
	char str[16];
	u32 i;
	rt_kprintf("�ڴ����ݴ�ӡ����ʼ��ַ��0X%X , ���� %d",addr,len);
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
	rt_kprintf("\r\n��ӡ���\r\n");
#endif
}

// ��������¶�
// ˮ�µ�����¶����趨�¶ȡ�����ˮ�¡���������й�ϵ
// �������ͺ� IHM-2216SS C       1600W
void SuReQiSet(u8 bTemp)
{
	rt_device_t uart;
	u8 rcv;
	u8 i,tmp;
	u32 tmptin=0;   // ����ˮ��
	u32 tmptout=0;  // ��ˮ�¶�
//	u8 buf[8] = {0x33,0x01,37,0,0,0,0};
//	buf[2] = bTemp;			// �¶�ֵ
//	buf[7] = 1+bTemp;		// У��
//   25�ȶ�Ӧ 01 01     70
//   28�ȶ�Ӧ��  01 1E    6F
//   35�� ��Ӧ�� 01 7C  0D
	u8 buf[13] = {0x32,0xF0,0X81,0X01,0X01,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X34};
//	buf[5] = (bTemp*10)&255;			// �¶�ֵ
//	buf[4] = (bTemp*10)>>8;
//	buf[5] = 0x7C;			// �¶�ֵ ��λ38��
	 tmptout=(1<<8);
		tmptout+=SetTempt;
		tmptout=(int)(tmptout/10); 
	rt_kprintf("\t�趨ˮ��: %d\r\n",(u32)tmptout);
	buf[5] = SetTempt;		// �¶�ֵ ��λ40��
//	buf[5] = 0x90;		// �¶�ֵ ��λ40��
	buf[4] = 0x01;          // ��λ
	tmp = buf[1];
	for(i=1;i<8;i++)
	{
		tmp ^= buf[i+1];
	}
	buf[11] = tmp;
	uart = rt_device_find("uart2");
	rt_device_open(uart, RT_DEVICE_OFLAG_RDWR);
//	while(1==rt_device_read(uart, 0, &rcv, 1))	// ����
//	{
//		rt_kprintf("%X\t",(u32)rcv);
//	}
//	rt_kprintf("\r\n");
//	rt_device_write(uart, 0, buf, sizeof(buf));
	rt_device_write(uart, 0, buf, sizeof(buf));
//	DbgPrintf("��ӡ��������\r\n");
//	Dump(buf,13);
//	rt_kprintf("\r\n");

	rt_thread_delay(RT_TICK_PER_SECOND*0.3f);
	for(i=0;i<13;i++)
	{
		buf[i]=0;
	}
	rcv = rt_device_read(uart, 0, buf, sizeof(buf));
	g_sensor.SuReQiWork=((buf[3]>0)?1:0);
	if((buf[0]==0x32)&&(buf[1]==0x0f)&&(buf[12]==0x34))// ��ʾͨѶ�ɹ�
	{
		tmptin=(buf[5]<<8);
		tmptin+=buf[6];
		tmptin=(int)(tmptin/10);  
		tmptout=(buf[7]<<8);
		tmptout+=buf[8];
		tmptout=(int)(tmptout/10);  
		rt_kprintf("\t��ˮˮ��: %d\r\n",(u32)tmptin);
		rt_kprintf("\t��ˮˮ��: %d\r\n",(u32)tmptout);	
		
	    if(tmptin>20)  // �������ˮ�´���20 ��Ϊ����ģʽ
	       SetTempt=0x4A;  // �趨ˮ��Ϊ33��
		else      // Ϊ����ģʽ
		{
		  if(tmptout<32)
		     SetTempt = 0xC2;// 45��
		  else
		  	SetTempt = 0x90; // 40��
//		   SetTempt = 0xAE;// 43��
//		   SetTempt = 0x90; // �趨ˮ��Ϊ40��
		}
//			DbgPrintf("��ӡ��������\r\n");
			Dump(buf,rcv);
	}

#if ENABLE_DEBUG
//#if 1
//	DbgPrintf("��ӡ��������\r\n");
//	Dump(buf,13);
//	rt_kprintf("\r\n");
//	DbgPrintf("��ӡ��������\r\n");
//	rt_thread_delay(RT_TICK_PER_SECOND*0.1f);
//	buf[3]=0x00;
//	rcv = rt_device_read(uart, 0, buf, sizeof(buf));
//	g_sensor.SuReQiWork=((buf[3]>0)?1:0);
//	Dump(buf,rcv);
//	rt_kprintf("\t������: %s\r\n",g_sensor.SuReQiWork?"Y":"");	
#endif
}


#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(SuReQiSet, "����ˮ��[0]");
#endif
