#ifndef __GOLBALDATA_H
#define __GOLBALDATA_H

#include "DataTypedef.h"
#include "main.h"


//--定义全局变量-----
#define EXT extern

//#define DEBUGGING

#define FALSE 0
#define TRUE 1
	
#define HSVERSION 3.08f		//版本号

//继电器
#define RELAY1(a) HAL_GPIO_WritePin(WPG1_GPIO_Port,WPG1_Pin,(a))
#define RELAY2(a) HAL_GPIO_WritePin(WPG2_GPIO_Port,WPG2_Pin,(a))

//通信端口定义
#define TXEN 1		//发送使能
#define RXEN 0		//接收使能
#define COMSA 0		//传感器A端口
#define COMSB 1		//传感器B端口
#define	COMPC 2		//PC端端口
#define COMDW 3		//迪文屏端口

//工作模式
#define MODEDIS 0x00 //无效模式
#define MODECHA 0x5A //单独A通道模式
#define MODECHB 0xA5 //单独B通道模式
#define MODEALL 0xAA //双通道模式

//FM存储器地址表
#define FM_ADDR_CFG_BASE  (8)
#define FM_ADDR_CFG_BKUP  (FM_ADDR_CFG_BASE + sizeof(ConfigTypdef) + 10)

//Modbus
#define ADDR_MAX_READ 160	//最大寻址范围
#define ADDR_UID_READ 101	//UID存储地址

//sd接入检测 低电平有效
#define SDCD_CHECK HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

//通信缓冲区 0-4
EXT ComBufTypedef ComTx[5];
EXT ComBufTypedef ComRx[5];

//传感器测量数据
EXT MsuDataTyoedef Sdata;

//配置参数
EXT ConfigTypdef Cfg;	

//时间日期
EXT TimeTpyedef Time;



#endif /*__GOLBALDATA_H*/
