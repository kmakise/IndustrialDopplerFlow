#ifndef __DATATYPEDEF_H
#define __DATATYPEDEF_H

#include "main.h"
#include "cmsis_os.h"

//时间类型
typedef struct
{
	uint8_t year;
	uint8_t mon;
	uint8_t day;
	uint8_t week;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	
	uint8_t update;
	
}TimeTpyedef;

//环形队列缓冲区
typedef struct
{
	uint8_t buf[256];	
	uint8_t cnt;			//写入下标
	uint8_t rd;				//读取下标
	
}ComBufTypedef;

//测量数据
typedef struct
{
	float 		flows;		//0		流速
	float 		levelsp;	//2		静压液位
	uint32_t	uid;			//6		传感器序号
	float 		temp;			//38	温度
	float 		levelus;	//40	超声液位
	float 		flowdt;		//42	瞬时流量
	double 		flowit;		//44	累积流量
	float 		dir;			//48	方向 1或者-1
	
	uint16_t  state;		// AB通道状态
	
}MsuDataTyoedef;


//配置参数
typedef struct
{
	//传感器配置
	uint16_t	ptl;			//100 协议类型1 modbusRTU，2 ModbusASCII
	uint16_t	addr;			//101	设备地址1~254
	uint16_t	fk;				//102 滤波系数1~99
	uint16_t	bps;			//106 波特率1~9，4代表9600（默认）1-1200，2-2400，3-4800，4-9600，5-14400，6-19200，7-38400, 8-57600, 9-115200
	uint16_t 	canal;		//110	水渠类型 （0矩形、1圆形、2等腰梯形、3三角形）
	float			bcpt;			//112	底部补偿
	float			angel;		//114	角度
	float			wsize;		//116	宽度尺寸
	float			thd;			//120 流速门限
	float			fsk;			//122 流速率定系数，默认1
	uint16_t	flowdtS;	//128 计算瞬时流量的液位选择0超声，1压力
	uint16_t	flowdtT;	//130 瞬时流量单位选择 ，0 立方米/s; 1立方米/h
	double		flowitSet;//132 设置累计流量
	
	float			out4ma;		//200 4ma输出
	float  		out20ma;	//202 20ma输出
	float			relay1;		//204 继电器1
	float			relay2;		//206 继电器2
	uint16_t	dsecsd;		//208 记录间隔
	
	uint32_t  uid;
	
	
	uint8_t		update;		//数据更新状态
	//step1：地址数值(100-132)时写入传感器配置 写入完成后置位FM写入状态
	//       特殊更新 201 协议修改 202 波特率修改 203本机地址修改
	//step2：FM写入状态(0xFF)时 写入FM存储器 写入完成后清除状态解除锁定
	
}ConfigTypdef;



#endif /*__DATATYPEDEF_H*/
