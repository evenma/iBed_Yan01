#ifndef _CAN_ANALYZER_H_
#define _CAN_ANALYZER_H_

// CANAnalyzer 初始化
extern void CanAnalyzerInit(void);

typedef struct{
	u8 PingTang : 1;		// 平躺
	u8 ZuoYi 	: 1;		// 座椅
	u8 TangYi   : 1;        // 躺椅
	u8 ZuoBian  : 1;        // 座便
	u8 FuYuan   : 1;		// 复原
	u8 BianMenKai:1;		// 便门开
	u8 BianMenGuan:1;		// 便门关
	u8 Anything		:1;		// 任意键暂停
}s_FlagCmd;

extern s_FlagCmd g_FlagCmd;	
//extern u16 YanShiCount;		// 便门联动延时计数器

#endif



