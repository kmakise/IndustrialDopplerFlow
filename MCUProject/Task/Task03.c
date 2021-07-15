/**
  ******************************************************************************
  * File Name          : Task03.c
  * Description        : Code for freeRTOS Task03 Application
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-1
  * @brief   传感器通信
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
#include "Task03.h"
#include "Task02.h"

#include "usart.h"
#include "golbaldata.h"
#include "Doppler.h"
#include "string.h"
 
/*Golbal data space ----------------------------------------------------------*/


/*---------------------------------------------------------------测量数据读取*/
MsuDataTyoedef Sensor[2];//传感器节点数据
MsuDataTyoedef Sdata;		 //汇总数据

//数据均值
void msuDataAverage(MsuDataTyoedef * m1,MsuDataTyoedef * m2,MsuDataTyoedef * m)
{
	m->dir			= (m1->dir			+ m2->dir			) / 2;
	m->flowdt		= (m1->flowdt		+ m2->flowdt	) / 2;
	m->flowit		= (m1->flowit		+ m2->flowit	) / 2;
	m->flows		= (m1->flows		+ m2->flows		) / 2;
	m->levelsp	= (m1->levelsp	+ m2->levelsp	) / 2;
	m->levelus	= (m1->levelus	+ m2->levelus	) / 2;
	m->state		= (m1->state		+ m2->state		) / 2;
	m->temp			= (m1->temp			+ m2->temp		) / 2;
}

//双通道传感器读取
void readMsuData(MsuDataTyoedef * sd)
{
	static uint16_t div = 0;
	div++;
	if(div > 30)//3S更新一次
	{
		div = 0;
		
		//读取测量数据
		sensorMeasureDataRead(COMSA,&Sensor[0]);
		sensorMeasureDataRead(COMSB,&Sensor[1]);
		
		//sensorConfigDataRead(COMSA,&Cfg);

		//数据合成
		
		//单独A通道模式 直接数据复制
		if(Sensor[0].state != 0 && Sensor[1].state == 0)
		{
			memcpy(sd,&Sensor[0],sizeof(MsuDataTyoedef));
			sd->state = MODECHA;
			sd->uid = Sensor[0].uid;
		}
		//单独B通道模式 直接数据复制
		if(Sensor[0].state == 0 && Sensor[1].state != 0)
		{
			memcpy(sd,&Sensor[1],sizeof(MsuDataTyoedef));
			sd->state = MODECHB;
			sd->uid = Sensor[1].uid;
		}
		//双通道模式 数据均值写入
		if(Sensor[0].state != 0 && Sensor[1].state != 0)
		{
			msuDataAverage(&Sensor[0],&Sensor[1],sd);
			sd->state = MODEALL;
			sd->uid = Sensor[0].uid;
		}
		//传感器失效
		if(Sensor[0].state == 0 && Sensor[1].state == 0)
		{
			sd->state = MODEDIS;
		}
	}
}

/*---------------------------------------------------------------配置数据同步*/
//配置参数临时保存点
ConfigTypdef CfgTemp;	

//配置参数同步 以本机为基准 向传感器更新 不向FM推送数据
// cfgb 基准配置 cfgs传感器配置 st传感器状态
void cfgDataSync(int st,ConfigTypdef * cfgb,ConfigTypdef * cfgs)
{
	if(cfgs->fk != cfgb->fk)
	{	
		//102 滤波系数1~99
		cfgs->fk =	cfgb->fk;		
		cfgs->update = 102;
		sensorConfigDataWrite(st,cfgs);
	}			
	if(cfgs->canal != cfgb->canal)
	{	
		//110	水渠类型 （0矩形、1圆形、2等腰梯形、3三角形）
		cfgs->canal =	cfgb->canal;		
		cfgs->update = 110;
		sensorConfigDataWrite(st,cfgs);
	}		                     
	if(cfgs->bcpt != cfgb->bcpt)
	{	
		//112	底部补偿
		cfgs->bcpt =	cfgb->bcpt;		
		cfgs->update = 112;
		sensorConfigDataWrite(st,cfgs);
	}		
	if(cfgs->angel != cfgb->angel)
	{	
		//114	角度
		cfgs->angel =	cfgb->angel;		
		cfgs->update = 114;
		sensorConfigDataWrite(st,cfgs);
	}
	if(cfgs->wsize != cfgb->wsize)
	{	
		//116	宽度尺寸
		cfgs->wsize =	cfgb->wsize;		
		cfgs->update = 116;
		sensorConfigDataWrite(st,cfgs);
	}
	if(cfgs->fsk != cfgb->fsk)
	{	
		//122 流速率定系数，默认1
		cfgs->fsk =	cfgb->fsk;		
		cfgs->update = 122;
		sensorConfigDataWrite(st,cfgs);
	}                     
	if(cfgs->flowdtS != cfgb->flowdtS)
	{	
		//128 计算瞬时流量的液位选择0超声，1压力
		cfgs->flowdtS =	cfgb->flowdtS;		
		cfgs->update = 128;
		sensorConfigDataWrite(st,cfgs);
	}
	if(cfgs->flowdtT != cfgb->flowdtT)
	{	
		//130 瞬时流量单位选择 ，0 立方米/s; 1立方米/h
		cfgs->flowdtT =	cfgb->flowdtT;		
		cfgs->update = 130;
		sensorConfigDataWrite(st,cfgs);
	}  

	
	//关于累积流量同步的说明：
	//累积流量不从本机同步到传感器 本机 也不存储累积流量数据
	//只有当发生PC修改累积流量时才修改数据并更新到传感器
	
//	if(cfgs->flowitSet	!= cfgb->flowitSet) 
//	{	
//		//132 设置累计流量
//		cfgs->flowitSet =	cfgb->flowitSet;		
//		cfgs->update = 132;
//		sensorConfigDataWrite(st,cfgs);
//	}
	
}

//读取 验证 与 各个传感器与本机的配置信息同步
void readCfgData(ConfigTypdef * cfg,int st)
{
	static uint16_t div = 0;
	div++;
	if(div > 500)//5s同步一次
	{
		div = 0;
		
		//在非更新状态时 读取 各个传感器
		if(cfg->update == 0)
		{
			if(st == MODECHA)//CHA模式
			{
				sensorConfigDataRead(COMSA,&CfgTemp);
				cfgDataSync(COMSA,cfg,&CfgTemp);
				
			}
			else if(st == MODECHB)//CHB模式
			{
				sensorConfigDataRead(COMSB,&CfgTemp);
				cfgDataSync(COMSB,cfg,&CfgTemp);
			}
			else if(st == MODEALL)//双通道模式
			{
				sensorConfigDataRead(COMSA,&CfgTemp);
				cfgDataSync(COMSA,cfg,&CfgTemp);
				sensorConfigDataRead(COMSB,&CfgTemp);
				cfgDataSync(COMSB,cfg,&CfgTemp);
			}
		}
	}
}

/*---------------------------------------------------------------配置数据更新*/

//写入传感器配置
void writeCfgToSensor(ConfigTypdef * cfg,int st)
{
	//步骤1：更新参数为地址状态时写入更新传感器配置
	if(Cfg.update >= 100 && Cfg.update <= 132)
	{
		if(st == MODECHA)
		{
			sensorConfigDataWrite(COMSA,&Cfg);
		}
		else if(st == MODECHB)
		{
			sensorConfigDataWrite(COMSB,&Cfg);
		}
		else if(st == MODEALL)
		{
			sensorConfigDataWrite(COMSA,&Cfg);
			sensorConfigDataWrite(COMSB,&Cfg);
		}
		//步骤2：准备写入FM 存储器
		cfg->update = 0xFF;
	}	
}

void Task03Main(void)
{
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart4,UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart5,UART_IT_RXNE);
	
	//等待FM参数读取
	waitCfgInitState();
	
	for(;;)
	{
		//获取测量数据
		readMsuData(&Sdata);
		
		//获取配置数据并同步 本机向传感器不写FM (更换传感器时才会发生)
		readCfgData(&Cfg,Sdata.state);
		
		//更新写入传感器配置 PC或dw修改 本机到传感器写FM
		writeCfgToSensor(&Cfg,Sdata.state);
		
		osDelay(10);
	}
}



