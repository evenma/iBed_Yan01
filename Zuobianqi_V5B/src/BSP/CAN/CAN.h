#ifndef	_CAB_H_
#define	_CAN_H_


// ��CAN �豸
extern rt_err_t CanOpen(int num,rt_err_t (*rx_ind )(rt_device_t, rt_size_t));
// ����CAN���ģ������ֱ��ʾ�˿ںͻ��������˿ڴ�0��ʼ����
// ����0��ʾ����ʧ�ܣ������ͳɹ�
extern int CanWrite(int num,u32 id,u8 len,u8* buf);
// ��ȡCAN���ģ������ֱ��ʾ�˿ںͻ��������˿ڴ�0��ʼ����
// ���ض�ȡ���ֽ���
extern int CanRead(int num, stcRxBUF *buf);


#endif
