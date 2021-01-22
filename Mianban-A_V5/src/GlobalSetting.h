#ifndef __GLOBAL_SETTING_H
#define __GLOBAL_SETTING_H

#define  MSG_NUMBER_MAX_LENGTH	15

typedef struct{
	u8 HoldOn;					// 挂起
	u8 ShuiWenGaoDi ;			// 水温高低
	u8 NuanFengQiangRuo ;		// 暖风温度高低
	u8 QiangXiQiangRuo;         // 清洗强弱
}S_ZuobianqiSetA;

typedef struct{
	u8 HoldOn;					// 挂起
	u8 ShuiWen;					// 水温设置，30-41℃  *1°  0℃为不加热
	u8 NuanFeng;				// 暖风温度设置，40-65℃     0℃为不加热
	u8 QiangXiQiangRuo;         // 清洗强弱  1,2,3三个档位
	u8 ZhiNengModel;			// 智能默认
	u8 ZhiWuRenModel;			// 植物人模式
}S_ZuobianqiSetB;

typedef struct{
	u16 BianPenZiDongPaiWuYanShi;			// 便盆自动排污时间，单位秒，0-60000有效
	u16 BianPenZiDongPauWuJianCeYanShi;		// 便盆自动排污检测延时，单位秒，0-60000有效
	u16 BianPenShouDongQingJieShiJian;		// 便盆手动清洁时间，单位秒，0-60000有效
	u16 HongGanShiJian;						// 烘干时间，单位秒，范围0-60000有效
	u16 TunBuQingXiShiJian;					// 臀部清洗时间，单位秒，范围0-60000有效
	u8  WenShuiPenLinJiaoDu;				// 温水喷淋角度，单位°
	u8  ShuiWen;							// 设置水温，60-80有效，设置温度=值*0.5℃
	u8  AlarmEn : 1;						// 报警使能
	u8  AlarmShuiWeiBuZuEn : 1;			// 水位不足报警使能
	u8  AlarmChuBianXiangManEn : 1;		// 储便箱满报警使能
	u8  AlarmDaBianEn :1;					// 大便报警使能
	u8  AlarmXiaoBianEn : 1;				// 小便报警使能
	u8  rev :3;						// 保留
	u8 ShangBiYanShi;						// 上壁冲洗延时
	u8 BianBiYanShi;						// 边壁延时
}t_ToiletActionConfig;

typedef struct
{
	t_ToiletActionConfig ToiletCfg;			// 座便器配置
	char acMsgNumber[MSG_NUMBER_MAX_LENGTH+1];			// 短信号码，加1是因为最后一位需要填0

}S_GlobalSetting;

// 大小便记录
typedef struct
{
	u16 wYear;			// 年
	u8 bMonth;		// 月
	u8 bDay;			// 日
	u8 bHour;			// 时
	u8 bMinute;		// 分
	u8 bSecond;		// 秒
	u8 bEvent;		// 1=大便，0=小便
}S_DaXiaoBianJiLu;

extern S_ZuobianqiSetB g_ZuobianqiSetB;	// 全局变量
extern S_ZuobianqiSetA g_ZuobianqiSetA;	// 全局变量
extern S_GlobalSetting g_sGlobalSetting;	// 全局变量

// 从 EEPROM 中载入全局变量
extern void LoadGlobalSetting(void);
// 将全局变量保存的 EEPROM 中
extern void SaveGlobalSeeting(void);
// 大小便记录
//	参数 bDaXiaoBian 1=大便，0=小便
extern void DaXiaoBianJiLu(u8 bDaXiaoBian);
// 获取记录总数
extern u32 GetJiLuZongShu(void);
// 获取第一条记录地址
extern S_DaXiaoBianJiLu* GetFirstJiLu(void);
// 获取最后一条记录地址
extern S_DaXiaoBianJiLu* GetLastJiLu(void);
extern S_DaXiaoBianJiLu* GetNextJiLu(S_DaXiaoBianJiLu* current);
// 获取上一条纪录
extern S_DaXiaoBianJiLu* GetPreviousJiLu(S_DaXiaoBianJiLu* current);

extern void SetDefault(void);


#endif

