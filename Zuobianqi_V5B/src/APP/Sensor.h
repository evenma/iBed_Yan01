#ifndef _SENSOR_H_
#define	_SENSOR_H_

#include <Common.h>
#include "Pwm.h"
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
	u8 MaTongFull			: 1 ;		// 马桶内液位满 
	u8 rv					: 2 ;		// 预留	

	u8 SuReQiError				;		// 速热器异常反馈
}s_Sensor;

typedef struct{
	u8 BeiBuFangPing	 	: 1;		// 背部放平
	u8 TuiBuFangPing		: 1;		// 腿部放平
	u8 ZuoFanFangPing	 	: 1;		// 左翻放平
	u8 YouFanFangPing		: 1;		// 右翻放平
	u8 FangJia				: 1;		// 防夹检测
	u8 BianMenKaiDaoDing    : 1;		// 便门开到顶
	u8 BianMenGuanDaoDi     : 1;        // 便门关到底
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
}s_BedSensor;

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
}s_BedSensor_MT_work;	// 所有电机状态0:电机停; 1:电机升;2:电机降；

// 传感器状态全局变量
extern volatile s_Sensor g_sensor;
// 传感器状态全局变量
extern volatile s_BedSensor g_bedSensor;		
extern volatile s_BedSensor_MT_work g_bedsensor_MT_work;

// 传感器初始化
extern void SensorInit(void);

typedef struct{
	u8 HoldOn;								// 挂起
	u8  ShuiWen;							// 设置水温，30-41有效，设置温度=值*1℃
	u8  NuanFen;							// 设置暖风温度，40-65有效，或者0有效，设置温度=值*1℃
	u8  ShuiLiu;							// 设置水流强度
}s_SetAConfig;

typedef struct{
	u8 HoldOn;								// 挂起
	u8  ShuiWen;							// 设置水温，30-41有效，设置温度=值*1℃
	u8  NuanFen;							// 设置暖风温度，40-65有效，或者0有效，设置温度=值*1℃
	u8  ShuiLiu;							// 设置水流强度
//	u8  ZhiNengmodel;						// 智能模式
//	u8 ZhiWuRenModel;						// 植物人模式	
//	u8 ChuangDianPower;						// 床垫加热开关
	u8 ChuangDianJiaRe;						// 床垫加热   25-50有效，设置温度=值*1℃
	u8 ZhiNengModel			: 1;			// 智能模式
	u8 ZhiWuRenModel		: 1;			// 植物人模式
	u8 ChuangDianPower	: 1;			// 床垫加热开关
	u8 NightLampSwitch  : 1;			// 小夜灯开关
	u8 ToiletLampSwitch : 1;			// 马桶内可视灯开关
	u8 rev					: 3;		// 预留
}s_SetBConfig;


typedef struct{	
	u16 BianPenZiDongPaiWuYanShi;			// 便盆自动排污时间，单位秒，0-60000有效
	u16 BianPenZiDongPauWuJianCeYanShi;		// 便盆自动排污检测延时，单位秒，0-60000有效
	u16 BianPenShouDongQingJieShiJian;		// 便盆手动清洁时间，单位秒，0-60000有效
	u16 HongGanShiJian;						// 烘干时间，单位秒，范围0-60000有效

	u16 TunBuQingXiShiJian;					// 臀部清洗时间，单位秒，范围0-60000有效
	u16 QianBuYanShi;						// 前部冲洗延时
	u16 MaTongQuanYanShi;					// 马桶圈冲洗延时
}t_ActionConfig;


extern volatile s_SetAConfig  Set_A_Config;
extern volatile s_SetBConfig  Set_B_Config;

extern volatile t_ActionConfig m_ActionCfg ;

extern u16 m_XSBDuty;
// 热水放水水阀
#define RE_SHUI_FANG_SHUI_FA(on)	if(on) {IO0SET = BIT15;} else {IO0CLR = BIT15;}
// 热水小水泵 全开 1800cc/min =1.8L/MIN =30mL/S
#define XIAO_SHUI_BENG(on)	if(on) {	ZENG_YA_BENG_ZHU_SHUI_FA(1);PwmChange(2, m_XSBDuty);} else {PwmChange(2,0);	ZENG_YA_BENG_ZHU_SHUI_FA(0);}


// sensor.c 和 actionthread.c共用
// 冲洗座便圈水阀
//#define CHONG_XI_SHUI_FA_ZUOBIANQUAN(on)	if(on) {IO0SET = BIT13;} else {IO0CLR = BIT13;}
#define CHONG_XI_SHUI_FA_ZUOBIANQUAN(on)	if(on) {IO0CLR = BIT13;} else {IO0CLR = BIT13;}
// 清水冲洗增压泵  20L/MIN	=0.33L/S  实际 0.138 L/S
#define QING_SHUI_CHONG_XI_ZENG_YA_BENG(on)	if(on) {IO0SET = BIT6;} else {IO0CLR = BIT6;}
// 排污泵，碎物泵，污水泵 水量45L/MIN  = 0.75L/S
#define PAI_WU_SUI_WU_BENG(on)  	if(on) {IO0SET = BIT21;} else {IO0CLR = BIT21;}					
#define WU_WU_XIANG_MAN		8000	// 污物箱满的重量 g
// 注水水泵
#define ZHU_SHUI_SHUI_BENG(on)	if(on) {IO0SET = BIT11;} else {IO0CLR = BIT11;}
// 增压泵注水阀
#define ZENG_YA_BENG_ZHU_SHUI_FA(on)	if(on) {IO1SET = BIT30;} else {IO1CLR = BIT30;}
// 马桶内可视灯开关
#define TOILET_LAMP_WORK(on) if(on) {IO0SET = BIT22;} else {IO0CLR = BIT22;}

#define TIME_ROTATE_DINING	6			// 6秒
#endif

