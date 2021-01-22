// CAN 设备驱动

#include "LPC214x.h"
#include "board.h"

#include <rthw.h>
#include <rtthread.h>
#include <finsh.h>
#include "IncludeCan.h"

#define	RT_USING_CAN1

#define	CAN_RX_BUFFER_SIZE	3	// CAN 环形缓冲区尺寸

/* LPC CAN device */
typedef struct _rt_lpccan
{
	/* inherit from device */
	struct rt_device parent;

	rt_uint32_t hw_base;
	rt_uint32_t irqno;
	rt_uint32_t baudrate;
	eCANNUM eCanNum;			// CAN 号
	/* 环形接收缓冲区 */
	rt_uint16_t save_index, read_index;
	stcRxBUF  rx_buffer[CAN_RX_BUFFER_SIZE];		// 接收缓冲区
}rt_lpccan;

#ifdef RT_USING_CAN1
rt_lpccan can1;
#endif

// 打开CAN 设备
rt_err_t CanOpen(int num,rt_err_t (*rx_ind )(rt_device_t, rt_size_t))
{
	if(rx_ind != RT_NULL)
	{
		rt_device_set_rx_indicate(&can1.parent,rx_ind );
	}
	return rt_device_open(&can1.parent,RT_DEVICE_OFLAG_RDWR);
}

// 发送CAN报文，参数分别表示端口和缓冲区，端口从0开始计算
// 返回0表示发送失败，否则发送成功
int CanWrite(int num, u32 id,u8 len,u8* buf)
{
	stcTxBUF can = {0};
	can.TxCANID.Word = id;
	can.TxFrameInfo.Word = len<<16;
	memcpy(&can.CANTDA,buf,len);
	
	if(num == 0)
	{
		return rt_device_write(&can1.parent, 0, &can, 1);
	}
	else
	{
		return 0;
	}
}

// 读取CAN报文，参数分别表示端口和缓冲区，端口从0开始计算
// 返回读取的字节数
int CanRead(int num, stcRxBUF *buf)
{
	if(num == 0)
	{
		return rt_device_read(&can1.parent, 0, buf, 1);
	}
	else
	{
		return 0;
	}
}

/* can hardware register */
#define REG8(d)			(*((volatile unsigned char *)(d)))
#define REG32(d)		(*((volatile unsigned long *)(d)))

#define	CAN_MOD(base)	REG8(base+0X00)
#define CAN_CMR(base) 	REG8(base+0X04)
#define CAN_GSR(base) 	REG8(base+0X08)
#define CAN_ICR(base) 	REG8(base+0X0C)
#define CAN_IER(base) 	REG8(base+0X10)
#define CAN_BTR(base) 	REG8(base+0X14)
#define CAN_EWL(base) 	REG8(base+0X18)
#define CAN_SR(base) 	REG8(base+0X1C)
#define CAN_RFS(base) 	REG8(base+0X20)
#define CAN_RID(base) 	REG8(base+0X24)
#define CAN_RDA(base) 	REG8(base+0X28)
#define CAN_RDB(base) 	REG8(base+0X2C)
#define CAN_TFI1(base) 	REG8(base+0X30)
#define CAN_TID1(base) 	REG8(base+0X34)
#define CAN_TDA1(base) 	REG8(base+0X38)
#define CAN_TDB1(base) 	REG8(base+0X3C)
#define CAN_TFI2(base) 	REG8(base+0X40)
#define CAN_TID2(base) 	REG8(base+0X44)
#define CAN_TDA2(base) 	REG8(base+0X48)
#define CAN_TDB2(base) 	REG8(base+0X4C)
#define CAN_TFI3(base) 	REG8(base+0X50)
#define CAN_TID3(base) 	REG8(base+0X54)
#define CAN_TDA3(base) 	REG8(base+0X58)
#define CAN_TDB3(base) 	REG8(base+0X5C)

static rt_err_t rt_can_init (rt_device_t dev)
{
	eCANNUM CanNum = ((rt_lpccan*)dev)->eCanNum;
	HwEnCAN(CanNum);
	SoftRstCAN(CanNum);
	CANEWL(CanNum).Bits.EWL_BIT = USE_EWL_CAN[CanNum];										
	return RT_EOK;
}

// CAN1 接收中断
void rt_hw_can_rx_isr(int irqno)
{
	u32 	j;
	uCANICR k;
	rt_base_t level;
	rt_lpccan* lpc_can;
	if(CANLUTerr.Word != 0 )	//LUT Error Program
	{
		//gprintf("111\r\n");
		//add or modify code
		j=CANLUTerrAd.Word;	
	}
	for(j=0;j<CAN_MAX_NUM;j++)
	{
		k=CANICR(j);
		if(k.Bits.RI_BIT != 0)
		{
			if(j ==0)
			{
				lpc_can = &can1;
			}
			//add code
			//gprintf("2接收中断\r\n");
			/* disable interrupt */
			level = rt_hw_interrupt_disable();

			/* read character */
			lpc_can->rx_buffer[lpc_can->save_index] = RxBUF(j);
			RelCanRecBuf(j);			// 释放缓冲区
			lpc_can->save_index ++;
			if (lpc_can->save_index >= CAN_RX_BUFFER_SIZE)
				lpc_can->save_index = 0;
			
			/* if the next position is read index, discard this 'read char' */
			if (lpc_can->save_index == lpc_can->read_index)
			{
				lpc_can->read_index ++;
				if (lpc_can->read_index >= CAN_RX_BUFFER_SIZE)
					lpc_can->read_index = 0;
			}

			/* enable interrupt */
			rt_hw_interrupt_enable(level);
			
			/* invoke callback */
			if(lpc_can->parent.rx_indicate != RT_NULL)
			{
				lpc_can->parent.rx_indicate(&lpc_can->parent, 1);
			}

		}
		if(k.Bits.TI1_BIT != 0)
		{
			//add code
			//rt_kprintf("3发送中断1\r\n");
		}
		if(k.Bits.TI2_BIT != 0)
		{
			//add code
			//gprintf("4发送中断2\r\n");
		}
		if(k.Bits.TI3_BIT != 0)
		{
			//add code
			//gprintf("5发送中断3\r\n");
		}
		if(k.Bits.BEI_BIT != 0)
		{
			//add code
//			DbgPrintf("6总线错误\r\n");
			CanBufOffLinePrg(j);
		}
		if(k.Bits.ALI_BIT != 0)
		{
			//add code
//			DbgPrintf("7仲裁丢失\r\n");
		}
		if(k.Bits.EPI_BIT != 0)
		{
			//add code
//			DbgPrintf("8错误认可\r\n");
		}
		if(k.Bits.WUI_BIT != 0)
		{
			//add code
//			DbgPrintf("9唤醒中断\r\n");
		}
		if(k.Bits.DOI_BIT != 0)
		{
			//add code
//			DbgPrintf("0数据溢出中断\r\n");
			ClrCanDataOver(j);
			SoftRstCAN(j);
			SoftEnCAN(j);
		}
	}
	VICVectAddr = 0;
}

static rt_err_t rt_can_open (rt_device_t dev, rt_uint16_t oflag)
{
	rt_lpccan* can = (rt_lpccan*)dev;
	eCANNUM CanNum = can->eCanNum;
	//初始化波特率
	CANBTR(CanNum).Word = USE_BTR_CAN[CanNum];											
	//配置验收滤波器(旁路状态)
	CANAFMR.Bits.AccBP_BIT =1;	
	//初始化模式
	CANMOD(CanNum).Bits.TPM_BIT = USE_TPM_CAN[CanNum];										
	CANMOD(CanNum).Bits.LOM_BIT = USE_MOD_CAN[CanNum];
	
	if(dev->flag & RT_DEVICE_FLAG_INT_RX)
	{
		if (can->irqno == CAN1_RX_INT)
		{
#ifdef RT_USING_CAN1
//		    rt_hw_interrupt_install(can->irqno, rt_hw_can1_rx_isr, RT_NULL);
			VICDefVectAddr =(u32)rt_hw_can_rx_isr;			// 使用非向量方式
			CANIER(CanNum).Word= USE_INT_CAN[CanNum];												
#endif
		}
		else if(can->irqno == CAN2_RX_INT)
		{
#ifdef RT_USING_CAN2
//		    rt_hw_interrupt_install(can->irqno, rt_hw_can2_rx_isr, RT_NULL);
			VICDefVectAddr =(u32)rt_hw_can_rx_isr;
			CANIER(CanNum).Word= 1;												
#endif
		}
	}
	// 启动CAN
	SoftEnCAN(CanNum);
	// 使能中断
	rt_hw_interrupt_umask(can->irqno);
	
	return RT_EOK;
}

static rt_err_t rt_can_close(rt_device_t dev)
{
	rt_lpccan* lpc_can;
	lpc_can = (rt_lpccan*) dev;
	
	RT_ASSERT(lpc_can != RT_NULL);

	if (dev->flag & RT_DEVICE_FLAG_INT_RX)
	{
		/* disable CAN rx interrupt */
		CANIER(lpc_can->eCanNum).Word= 0;												
	}

	return RT_EOK;
}

static rt_err_t rt_can_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
	return RT_EOK;
}

// 读函数，一次只能读取一个帧，因此size参数无效，返回数量是帧长度
static rt_size_t rt_can_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	stcRxBUF* ptr;
	rt_lpccan *lpc_can = (rt_lpccan*)dev;
	RT_ASSERT(lpc_can != RT_NULL);
	RT_ASSERT(buffer != NULL);

	/* point to buffer */
	ptr = (stcRxBUF*) buffer;

	if (dev->flag & RT_DEVICE_FLAG_INT_RX)
	{
		size = 1;
		while (size)
		{
			/* interrupt receive */
			rt_base_t level;

			/* disable interrupt */
			level = rt_hw_interrupt_disable();
			if (lpc_can->read_index != lpc_can->save_index)
			{
				*ptr = lpc_can->rx_buffer[lpc_can->read_index];

				lpc_can->read_index ++;
				if (lpc_can->read_index >= CAN_RX_BUFFER_SIZE)
					lpc_can->read_index = 0;
			}
			else
			{
				/* no data in rx buffer */

				/* enable interrupt */
				rt_hw_interrupt_enable(level);
				break;
			}

			/* enable interrupt */
			rt_hw_interrupt_enable(level);

			ptr ++; size --;
		}

		return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
	}
	else if (dev->flag & RT_DEVICE_FLAG_DMA_RX)
	{
		/* not support right now */
		RT_ASSERT(0);
	}
	else		/* polling mode */

	{
		/* not support right now */
		RT_ASSERT(0);
	}
	return ((rt_size_t)ptr - (rt_size_t)buffer);
}

// 一次只能发送一个报文，因此size必须为1，返回数量是写入数据帧长度
static rt_size_t rt_can_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	rt_lpccan* lpc_can;

	RT_ASSERT(buffer != NULL);
	
	lpc_can = (rt_lpccan*) dev;
	if (dev->flag & RT_DEVICE_FLAG_INT_TX)
	{
		/* not support */
		RT_ASSERT(0);
	}
	else if (dev->flag & RT_DEVICE_FLAG_DMA_TX)
	{
		/* not support */
		RT_ASSERT(0);
	}

	/* polling write */	
	if(0 == CANSendData(lpc_can->eCanNum,0,(P_stcTxBUF)buffer))
	{
		return sizeof(stcTxBUF);		// 成功发送完一个报文
	}
	else
	{
		static u32 e = 0;
		if(0==(e++%100))
		{
			DbgPrintf("CAN 发送失败 %d\r\n",e);
		}
		return 0;		// 发送失败
	}
}



void rt_hw_can_init(void)
{
	rt_lpccan* lpc_can;
	
#ifdef RT_USING_CAN1
	lpc_can = &can1;
	
	lpc_can->parent.type = RT_Device_Class_CAN;
	
	lpc_can->hw_base = 0xE0044000;
	lpc_can->baudrate = 250000;
	lpc_can->eCanNum = CAN1;
	lpc_can->irqno = CAN1_RX_INT;
	
	rt_memset(lpc_can->rx_buffer, 0, sizeof(lpc_can->rx_buffer));
	lpc_can->read_index = lpc_can->save_index = 0;

	lpc_can->parent.init 	= rt_can_init;
	lpc_can->parent.open 	= rt_can_open;
	lpc_can->parent.close    = rt_can_close;
	lpc_can->parent.read 	= rt_can_read;
	lpc_can->parent.write    = rt_can_write;
	lpc_can->parent.control  = rt_can_control;
	lpc_can->parent.private  = RT_NULL;

	rt_device_register(&lpc_can->parent, 
		"CAN1", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
#endif

}


