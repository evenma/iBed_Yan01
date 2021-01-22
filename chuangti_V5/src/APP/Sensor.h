#ifndef _SENSOR_H_
#define	_SENSOR_H_

#include <Common.h>

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
	u8 FunctionWorkZuoYi    : 1;		// 功能运行状态 座椅
	u8 FunctionWorkTangYi   : 1;		// 功能运行状态 躺椅	
	u8 ChongDian            : 1;		// 充电状态
	
	u8 QiBeiJiaoDu;
	u8 XiaQuTuiJiaoDu;
	u8 ShangQuTuiJiaoDu;
	u8 ZuoFanShenJiaoDu;
	u8 YouFanShenJiaoDu;
	u8 DianChiDianYa;					// 	电池电压
}s_Sensor;

typedef struct{
	u32 BeiBuDianLiu;				// 背部电流值
	u32 TuiBuDianLiu;				// 腿部电流值
	u32 ZuoFanDianLiu;				// 左翻身电流值
	u32 YouFanDianLiu;				// 右翻身电流值
	u32 HuaBeiDianLiu;				// 滑背电流值
	u32 BianMenShengJiangDianLiu;	// 便门升降电流值
	u32 BianMenPingYiDianLiu;		// 便门平移电流值
	u32 ZuoBianQiShengJiangDianLiu;	// 座便器升降电流值
	u32 DianChiDianLiang;			// 电池电量
}ADS_Sensor;

typedef struct{
	u8 beibu 				: 2;		// 背部电机状态
	u8 tuibu			 	: 2;		// 腿部电机状态
	u8 zuofanshen			: 2;		// 左翻身电机状态
	u8 youfanshen			: 2;		// 右翻身电机状态
	
	u8 huabei				: 2;		// 滑背电机状态
	u8 bianmenshengjiang 	: 2;		// 便门升降电机状态
	u8 bianmenpingyi	 	: 2;		// 便门平移电机状态
	u8 zuobianqishengjiang	: 2;		// 座便器升降电机状态

	u16 DianChiDianLiang;			// 电池电量
	
	u8 leftG 				: 2;		// 左护栏电机状态
	u8 rightG			 	: 2;		// 右护栏电机状态	
	u8 dining				: 2;		// 餐桌电机状态
	u8 rv					: 2;		// 预留		
	
	u8 diningCnt;         // 餐桌推杆行程
	
}s_MT_work;	// 所有电机状态0:电机停; 1:电机升;2:电机降；


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
	
	//	u8 ChuFengWenDu				;		// 出风口温度  结构上做不了
	// 	u8 ChuanDianWenDu			;		//  床垫温度 与气垫冲突，气垫不能加热
	u8 rotateDiningCnt;				// 餐桌旋转电机
	
	u8 ReShuiWenDu				;		// 速热器热水温度
	u8 LengShuiWenDu			;       // 水箱冷水温度
	u8 FunctionWorkNuanFeng : 1	;		// 功能运行状态 暖风烘干
	u8 FunctionWorkQianBu	: 1	;		// 功能运行状态 前部冲洗
	u8 FunctionWorkHouBu	: 1	;		// 功能运行状态 后部冲洗
	u8 FunctionWorkMaTong 	: 1	;		// 功能运行状态 马桶冲洗
	u8 FunctionWorkAuto 	: 1	;		// 功能运行状态 智能模式
	u8 rv					: 3;		// 预留	
	u8 SuReQiError				;		// 速热器异常反馈
}s_ZuobianqiSensor;

// 传感器状态全局变量
extern volatile s_Sensor g_sensor;		
extern volatile ADS_Sensor g_AD_sensor;
extern volatile s_MT_work g_MT_work;
extern volatile s_ZuobianqiSensor g_ZuobianqiSensor;


// 参数配置信息 推杆码盘数
//#define	BEI_MAPAN_MAX	7850//7350//7445//7500 //7525  // 8000  实测数据      7.2mm.s
//#define	BEI_MAPAN_MIDDLE	105*BEI_MAPAN_MAX/265   //250   //3160 // 4000 
//#define	TUI_MAPAN_MAX	5950//5700//4450//4500 //3826	//4600    5540   1112  4.2mm/s   40脉冲/mm
//#define	TUI_MAPAN_SHUI_PING	(u32)122*TUI_MAPAN_MAX/150	//3111	3434    5986
//#define	FANSHEN_MAPAN_MAX	2950//3220
#define	BEI_MAPAN_MAX	 3850//3952		//1976*2  130mm 3845 3849 3846 3844 3850 3855 3855
#define	BEI_MAPAN_MIDDLE	BEI_MAPAN_MAX/2   
#define	TUI_MAPAN_MAX	4826//5429	// 2736*2 180mm       5417 5429 5429    // 腿部推杆被抬高20mm 码盘数减掉603
#define	TUI_MAPAN_SHUI_PING	3921//4524   //2280*2 150mm     4509   -100
#define	FANSHEN_MAPAN_MAX	2950//3220

#define WUCHAJINGDU 50    // 误差范围100mS采样周期+ 电机停惯性
// 床体旋转角度
#define BEI_JIAODU_MAX 	72 //
#define TUI_XIA_JIAODU_MAX 66//55
#define TUI_SHANG_JIAODU_MAX 14
#define FANSHEN_JIAODU_MAX 50



// 传感器初始化
extern void SensorInit(void);
extern void __irq timer1_interrupt(void);

extern u16 m_beibu_max ;
extern u16 m_beibu_middle ;
extern u16 m_tuibu_max ;
extern u16 m_tuibu_middle ;
extern u16 m_fanshen_max ;
extern u16 m_wucha ;
extern u16 beibumapancount;
extern u16 tuibumapancount; 

extern u8 m_TuiBuStatus;
extern u8 c_TuiBuStatus;

extern u8 m_bei_hall;
extern u16 beibumapancount;
extern u8 m_tui_hall;
extern u16 tuibumapancount;
extern u8 m_zuofan_hall;
extern u16 zuofanmapancount;
extern u8 m_youfan_hall;
extern u16 youfanmapancount;
extern u32 g_diancidianliangAd;
#endif

