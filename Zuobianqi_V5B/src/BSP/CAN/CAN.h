#ifndef	_CAB_H_
#define	_CAN_H_


// 打开CAN 设备
extern rt_err_t CanOpen(int num,rt_err_t (*rx_ind )(rt_device_t, rt_size_t));
// 发送CAN报文，参数分别表示端口和缓冲区，端口从0开始计算
// 返回0表示发送失败，否则发送成功
extern int CanWrite(int num,u32 id,u8 len,u8* buf);
// 读取CAN报文，参数分别表示端口和缓冲区，端口从0开始计算
// 返回读取的字节数
extern int CanRead(int num, stcRxBUF *buf);


#endif
