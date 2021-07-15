/**
  ******************************************************************************
  * File Name          : Doppler.c
  * Description        : Code for Doppler sensor dirver and communication
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-2
  * @brief   声学多普勒流速测量仪驱动
	******************************************************************************
  * @attention
  * <h2><center>&copy; Copyright (c) kmakise
  * All rights reserved.
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "Doppler.h"
#include "golbaldata.h"
#include "UserCom.h"
#include "Modbus.h"
#include "SoftCRC.h"

#define DELAY osDelay

/*Golblla data space ------------------------------------------------------------*/


//接收处理 返回 0接收错误 1接收成功 
int8_t getRecState(int p)
{ 
	uint8_t cnthist = 0;
	
	//等待数据开始接收 等待1000ms
	for(int i=100;i>0;i--) 
	{		   
		DELAY(10);
		if(ComRx[p].cnt != 0)
		{	   
			break;		
		}	
	}
	
	//等待数据接收完成
	while(cnthist != ComRx[p].cnt)
	{
		cnthist = ComRx[p].cnt;
		DELAY(2);
	}
	
	//校验
  if(ComRx[p].cnt > 7)
	{			  
		if(CalCRC16(ComRx[p].buf,ComRx[p].cnt)==0)
		{
			return 1;
		}
	}
	return 0;	
}

//测量 缓冲区数据到全局数据结构 0 开始
void bufToMsuStruct(uint8_t * buf,MsuDataTyoedef * st)
{
	st->flows 	= getFloat (&(buf[0 	* 2]));		//0		流速
	st->levelsp = getFloat (&(buf[2 	* 2]));		//2		静压液位
	st->uid 		= getInt32 (&(buf[6 	* 2]));		//6		传感器序号
	st->temp 		= getFloat (&(buf[38 	* 2]));		//38	温度
	st->levelus = getFloat (&(buf[40	* 2]));		//40	超声液位
	st->flowdt 	= getFloat (&(buf[42 	* 2]));		//42	瞬时流量
	st->flowit 	= getDouble(&(buf[44 	* 2]));		//44	累积流量
	st->dir 		= getFloat (&(buf[48 	* 2]));		//48	方向 1或者-1
}

//配置 缓冲区数据到全局数据结构 100开始
void bufToCfgStruct(uint8_t * buf,ConfigTypdef * st)
{
	//st->ptl 			= getInt16 (&(buf[0 	* 2]));			//100 协议类型1 modbusRTU，2 ModbusASCII
	//st->addr 			= getInt16 (&(buf[1 	* 2]));			//101	设备地址1~254
	st->fk 				= getInt16 (&(buf[2 	* 2]));				//102 滤波系数1~99
	//st->bps 			= getInt16 (&(buf[6 	* 2]));			//106 波特率1~9，4代表9600（默认）1-1200，2-2400，3-4800，4-9600，5-14400，6-19200，7-38400, 8-57600, 9-115200
	st->canal 		= getInt16 (&(buf[10 	* 2]));				//110	水渠类型 （0矩形、1圆形、2等腰梯形、3三角形）
	st->bcpt 			= getFloat (&(buf[12 	* 2]));				//112	底部补偿
	st->angel 		= getFloat (&(buf[14 	* 2]));				//114	角度
	st->wsize		 	= getFloat (&(buf[16 	* 2]));				//116	宽度尺寸
	st->thd		 		= getFloat (&(buf[20 	* 2]));				//120	流速门限
	st->fsk 			= getFloat (&(buf[22 	* 2]));				//122 流速率定系数，默认1
	st-> flowdtS 	= getInt16 (&(buf[28	* 2]));				//128 计算瞬时流量的液位选择0超声，1压力
	st->flowdtT 	= getInt16 (&(buf[30	* 2]));				//130 瞬时流量单位选择 ，0 立方米/s; 1立方米/h
	st->flowitSet = getDouble(&(buf[32	* 2]));				//132 设置累计流量
}


//发送读取指令
void TxReadRegCmd(int p,int addr,int num)
{
	 uint16_t	SendCrc;
	
	//寄存器读取指令
	ComTx[p].cnt = 0;
	ComTx[p].buf[ComTx[p].cnt++] = 0x01;
	ComTx[p].buf[ComTx[p].cnt++] = 0x03;
  putInt16(&ComTx[p].buf[ComTx[p].cnt],addr);
	ComTx[p].cnt += 2;
  putInt16(&ComTx[p].buf[ComTx[p].cnt],num);
	ComTx[p].cnt += 2;
	SendCrc= CalCRC16(ComTx[p].buf,ComTx[p].cnt);
	putInt16(&ComTx[p].buf[ComTx[p].cnt],SendCrc);
	ComTx[p].cnt += 2;   
	
	//发送数据
	TxData(p);
}

//写入单个寄存器
void TxWriteRegCmd(int p,int addr,char type,void * num)
{
	uint16_t SendCrc;
	
	//01（设备地址）10（功能码）00 00（起始地址）XX XX（寄存器数量）
	//XX（字节数）XX XX···XX XX（2*寄存器数量个字节内容）XX XX（CRC16校验码）
	
	
	//寄存器写入指令
	ComTx[p].cnt = 0;
	ComTx[p].buf[ComTx[p].cnt++] = 0x01;
	ComTx[p].buf[ComTx[p].cnt++] = 0x10;
  putInt16(&ComTx[p].buf[ComTx[p].cnt],addr);
	ComTx[p].cnt += 2;
	
	
	//数据类型选择
	if(type == 'S')
	{
		putInt16(&ComTx[p].buf[ComTx[p].cnt],1);
		ComTx[p].cnt += 2;
		ComTx[p].buf[ComTx[p].cnt++] = 2;
		putInt16(&ComTx[p].buf[ComTx[p].cnt],*((int16_t *)num));
		ComTx[p].cnt += 2;
	}
	else if(type == 'L')
	{
		putInt16(&ComTx[p].buf[ComTx[p].cnt],2);
		ComTx[p].cnt += 2;
		ComTx[p].buf[ComTx[p].cnt++] = 4;
		putInt32(&ComTx[p].buf[ComTx[p].cnt],*((int32_t *)num));
		ComTx[p].cnt += 4;
	}
	else if(type == 'F')
	{
		putInt16(&ComTx[p].buf[ComTx[p].cnt],2);
		ComTx[p].cnt += 2;
		ComTx[p].buf[ComTx[p].cnt++] = 4;
		putFloat(&ComTx[p].buf[ComTx[p].cnt],*((float *)num));
		ComTx[p].cnt += 4;
	}
	else if(type == 'D')
	{
		putInt16(&ComTx[p].buf[ComTx[p].cnt],4);
		ComTx[p].cnt += 2;
		ComTx[p].buf[ComTx[p].cnt++] = 8;
		putDouble(&ComTx[p].buf[ComTx[p].cnt],*((double *)num));
		ComTx[p].cnt += 8;
	}
	SendCrc= CalCRC16(ComTx[p].buf,ComTx[p].cnt);
	putInt16(&ComTx[p].buf[ComTx[p].cnt],SendCrc);
	ComTx[p].cnt += 2;   
	//发送数据
	TxData(p);
	
	//等待回复
	getRecState(p);
	ComRx[p].cnt = 0;
}


//读取测量数据
void sensorMeasureDataRead(int p,MsuDataTyoedef * sr)
{
	TxReadRegCmd(p,0,50);
	
	//等待数据回传
	if(getRecState(p))
	{
		bufToMsuStruct(&(ComRx[p].buf[3]),sr);
		sr->state = 0xFF;
	}
	else
	{
		sr->state = 0;
	}
	ComRx[p].cnt = 0;
}

//读取配置数据
void sensorConfigDataRead(int p,ConfigTypdef * cfg)
{
	TxReadRegCmd(p,100,40);
	
	//等待数据回传
	if(getRecState(p))
	{
		//无更新等待时更新数据
		if(cfg->update == 0)
		{
			bufToCfgStruct(&(ComRx[p].buf[3]),cfg);
		}
	}
	ComRx[p].cnt = 0;
}

//写入配置参数 到传感器
void sensorConfigDataWrite(int p,ConfigTypdef * cfg)
{
	//全写
	if(cfg->update == 0xff)
	{
		
	}	
	//单个寄存器
	else if(cfg->update >= 100 && cfg->update <= 132)
	{
		//波特率和地址不写入传感器配置
		switch(cfg->update)
		{
			//case 100:TxWriteRegCmd(p,cfg->update,'S',&(cfg->ptl			));break;
			//case 101:TxWriteRegCmd(p,cfg->update,'S',&(cfg->addr		));break;
			case 102:TxWriteRegCmd(p,cfg->update,'S',&(cfg->fk			));break;
			//case 106:TxWriteRegCmd(p,cfg->update,'S',&(cfg->bps			));break;
			case 110:TxWriteRegCmd(p,cfg->update,'S',&(cfg->canal		));break;
			
			case 112:TxWriteRegCmd(p,cfg->update,'F',&(cfg->bcpt		));break;
			case 114:TxWriteRegCmd(p,cfg->update,'F',&(cfg->angel		));break;
			case 116:TxWriteRegCmd(p,cfg->update,'F',&(cfg->wsize		));break;
			case 122:TxWriteRegCmd(p,cfg->update,'F',&(cfg->fsk			));break;
			
			case 128:TxWriteRegCmd(p,cfg->update,'S',&(cfg->flowdtS	));break;
			case 130:TxWriteRegCmd(p,cfg->update,'S',&(cfg->flowdtT	));break;
			
			case 132:TxWriteRegCmd(p,cfg->update,'D',&(cfg->flowitSet));break;
			default:break;
		}
	}
}




