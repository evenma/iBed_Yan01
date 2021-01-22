#ifndef _CAN_ANALYZER_H_
#define _CAN_ANALYZER_H_

typedef struct{
	u8 WuWuXiangGuan		: 1;		// 污物箱管路连接状态
	u8 DaBian				: 1;		// 大便
	u8 ManXiaoBian			: 1;		// 男士小便
	u8 WomenXiaoBian		: 1;		// 女士小便
	u8 SuReQiWork           : 1;		// 速热器工作标志 
	u8 ShuiXiangHigh		: 1;		// 水箱满标志
	u8 ShuiXiangLow			: 1;		// 水箱低标志
	u8 rev					: 1;		// 预留
	u16 WuWuXiangZhongLiang		;		// 污物箱重量
	u8 ChuFengWenDu				;		// 出风口温度
	u8 ReShuiWenDu				;		// 速热器热水温度
	u8 LengShuiWenDu			;       // 水箱冷水温度
	u8 FunctionWorkNuanFeng : 1	;		// 功能运行状态 暖风烘干
	u8 FunctionWorkQianBu	: 1	;		// 功能运行状态 前部冲洗
	u8 FunctionWorkHouBu	: 1	;		// 功能运行状态 后部冲洗
	u8 FunctionWorkMaTong 	: 1	;		// 功能运行状态 马桶冲洗
	u8 FunctionWorkAuto 	: 1	;		// 功能运行状态 智能模式
	u8 rv					: 3;		// 预留	
	u8 SuReQiError				;		// 速热器异常反馈
}s_ZuobianSensor;

typedef struct{
	u8 BeiBuFangPing	 	: 1;		// 背部放平
	u8 TuiBuFangPing		: 1;		// 腿部放平
	u8 ZuoFanFangPing	 	: 1;		// 左翻放平
	u8 YouFanFangPing		: 1;		// 右翻放平
	u8 FangJia				: 1;		// 防夹检测
	u8 BianMenKaiDaoDing    : 1;		// 便门开到顶
	u8 BianMenGuanDaoDi    : 1;        // 便门关到底
	u8 BianMenPingYiDaoDing	: 1;		// 便门平移到顶

	u8 BianMenPingYiDaoDi	: 1;		// 便门平移到底
	u8 DianLiangBuZu        : 1;		// 电量不足
	u8 DianJiGuoLiuBao		: 1;		// 电机过流保护
	u8 TingDian				: 1;		// 停电
	u8 DianChiZhuOn			: 1;		// 是否有电池组
	u8 rev                  : 3;		
	
	u8 QiBeiJiaoDu;
	u8 XiaQuTuiJiaoDu;
	u8 ShangQuTuiJiaoDu;
	u8 ZuoFanShenJiaoDu;
	u8 YouFanShenJiaoDu;
}s_ChuangTiSensor;

// 传感器状态全局变量
extern volatile s_ChuangTiSensor g_ChuangTiSensor;
extern volatile s_ZuobianSensor g_ZuobianSensor;


#define WU_WU_XIANG_BORDER 8000	// 污物箱临界值 g
//#define WU_WU_XIANG_BU_ZAI_WEI 1000     // 污物箱不在位

// 接收到CAN回调函数
extern rt_err_t CanRxInd(rt_device_t dev, rt_size_t size);
extern void CanSetKeyCmd(u32 msg);
extern void LEDSetInit(void);
#endif



