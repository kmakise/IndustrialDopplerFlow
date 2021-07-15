/**
  ******************************************************************************
  * File Name          : Task02.c
  * Description        : Code for freeRTOS Task02 Application
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-1
  * @brief   时间管理，铁电数据
	******************************************************************************
  * @attention
  * <h2><center>&copy; Copyright (c) kmakise
  * All rights reserved.
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "Task02.h"

#include "SoftCRC.h"
#include "golbaldata.h"
#include "pcf8563.h"
#include "FM25CL64.h"
#include "stdio.h"
#include "string.h"
#include "Modbus.h"

/*Golbal data space ----------------------------------------------------------*/
//配置参数
ConfigTypdef Cfg;
TimeTpyedef Time;
uint8_t  CfgRdInit = 0;

//软件时间
void timeRunning(void)
{
	Time.sec++;
	if(Time.sec > 59)
	{
		Time.sec = 0;
		Time.min++;
		if(Time.min > 59)
		{
			ReadTime();
		}
	}
	//time update 
	if(Time.update != 0)
	{
		Time.update = 0;
		WriteTime();
	}
}

//LED状态灯
void LEDstate(void)
{
	GPIOC->BSRR = GPIO_PIN_2;
	osDelay(500);
	GPIOC->BRR = GPIO_PIN_2;
	osDelay(500);
}

/*--------------------------------------------------------------铁电存储器部分*/
uint8_t FMemBuf[128];//铁电缓冲区

//写入配置信息到铁电存储器
void writeCfgToFM(ConfigTypdef * cfg)
{
	if(cfg->update == 0xFF)
	{
		uint16_t crc;
		uint16_t len = sizeof(ConfigTypdef);
		
		cfg->update = 0x00;
		
		memcpy(FMemBuf,cfg,len);
		crc = CalCRC16(FMemBuf,len);
		putInt16(&FMemBuf[len],crc);
		//写入本区
		WriteFM(FM_ADDR_CFG_BASE,FMemBuf,len + 2);
		//写入备份
		WriteFM(FM_ADDR_CFG_BKUP,FMemBuf,len + 2);
	} 
}

//载入默认参数
void loadDefaultCfg(ConfigTypdef * cfg)
{
	cfg->ptl			= 1;	//100 协议类型1 modbusRTU，2 ModbusASCII
	cfg->addr 		= 1;	//101	设备地址1~254
	cfg->angel 		= 45;	//114	角度
	cfg->bcpt 		= 0;	//112	底部补偿
	cfg->bps			= 4;	//106 波特率1~9，4代表9600（默认）1-1200，2-2400，3-4800，4-9600，5-14400，6-19200，7-38400, 8-57600, 9-115200
	cfg->canal  	= 0;	//110	水渠类型 （0矩形、1圆形、2等腰梯形、3三角形）

	cfg->fk				= 10;	//102 滤波系数1~99
	cfg->flowdtS	=	0;	//128 计算瞬时流量的液位选择0超声，1压力
	cfg->flowdtT 	= 0;	//130 瞬时流量单位选择 ，0 立方米/s; 1立方米/h
	cfg->flowitSet= 0;	//132 设置累计流量
	cfg->fsk			= 1;	//122 流速率定系数，默认1
	
	cfg->out20ma 	= 0;	//4ma输出
	cfg->out4ma		= 10;	//20ma输出
	cfg->relay1		= 0;	//继电器1
	cfg->relay2		= 0;	//继电器2
	cfg->dsecsd 	= 10;	//记录间隔
	
	cfg->update 	= 0;
}


//从铁电读取配置
void readFMtoCfg(ConfigTypdef * cfg)
{
	//读取内容到缓冲区
	ReadFM(FM_ADDR_CFG_BASE,FMemBuf,sizeof(ConfigTypdef) + 16);
	//本区CRC校验
	if(CalCRC16(FMemBuf,sizeof(ConfigTypdef)+2)==0)
	{
		memcpy(cfg,FMemBuf,sizeof(ConfigTypdef));
	}
	//本区校验失败 读取备份区
	else
	{
		//读取内容到缓冲区
		ReadFM(FM_ADDR_CFG_BKUP,FMemBuf,sizeof(ConfigTypdef) + 16);
		//备份区CRC校验
		if(CalCRC16(FMemBuf,sizeof(ConfigTypdef)+2)==0)
		{
			memcpy(cfg,FMemBuf,sizeof(ConfigTypdef));
		}
		else//备份区失败时 载入默认参数并写入FM
		{
			//加载
			loadDefaultCfg(cfg);
			//写入FM
			cfg->update = 0xFF;
			writeCfgToFM(cfg);
		}
	}
	cfg->update = 0;
	CfgRdInit = 1;
}

//获得参数初始化状态
void waitCfgInitState(void)
{
	while(CfgRdInit == 0);
}

void Task02Main(void)
{
	//读取时间
	Time.update = 0;
	ReadTime();
	
	//参数读取
	readFMtoCfg(&Cfg);
	
	for(;;)
	{
		//软件计时
		timeRunning();
		
		//更新参数写入FM
		writeCfgToFM(&Cfg);
		
		//状态灯 1s周期控制
		LEDstate();
	}
}



