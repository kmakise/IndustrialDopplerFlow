/**
  ******************************************************************************
  * File Name          : ModbusSlave.c
  * Description        : Code for Modbus Protocol slave 
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-6
  * @brief   Modbus 从机通信协议
	******************************************************************************
  * @attention
  * <h2><center>&copy; Copyright (c) kmakise
  * All rights reserved.
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "ModbusSlave.h"
#include "Modbus.h"
#include "golbaldata.h"
#include "SoftCRC.h"
#include "UserCom.h"
#include "string.h"

/*Golbal data space ----------------------------------------------------------*/
uint8_t tfbuf[256];
 
 //modbus 发送
 void ModbusTx(int p)
 {
	 //Modbus ASCII
	if(Cfg.ptl == 2)
	{
		//ASCII 转换成 RTU
		ComTx[p].cnt =  putRTUtoASCII(tfbuf,ComTx[p].buf,ComTx[p].cnt);
		if(ComTx[p].cnt != 0)
		{
			memcpy(ComTx[p].buf,tfbuf,ComTx[p].cnt);
			TxData(p);
		}
	}
	else
	{
		TxData(p);
	}
 }
 
//串口读命令返回
void readTxRTU(int p)
{
	int16_t SendCrc;
	ComTx[p].buf[0]=ComRx[p].buf[0];
	ComTx[p].buf[1]=ComRx[p].buf[1];
	SendCrc= CalCRC16(ComTx[p].buf,ComTx[p].buf[2]+3);
	putInt16(&ComTx[p].buf[ComTx[p].buf[2]+3],SendCrc);
	ComTx[p].cnt = ComTx[p].buf[2]+5;

	ModbusTx(p);
}
//串口写命令返回（返回接收到的命令）
void writeTxRTU(int p)
{
	int16_t SendCrc;
	memcpy(ComTx[p].buf,ComRx[p].buf,6);//复制接收到的内容
	SendCrc = CalCRC16(ComTx[p].buf,6);
	putInt16(&ComTx[p].buf[6],SendCrc); 
	ComTx[p].cnt = 8;//返回固定的8个字节
	
	ModbusTx(p);
}
//错误码返回
void  errorCodeTx(uint8_t p,uint8_t code)
{
	//01 83 03 01 31
	int16_t SendCrc;
	ComTx[p].buf[0] = Cfg.addr;
	ComTx[p].buf[1] = 0x83;
	ComTx[p].buf[1] = code;
	
	SendCrc= CalCRC16(ComTx[p].buf,3);
	putInt16(&ComTx[p].buf[ComTx[p].buf[2]+3],SendCrc);
	ComTx[p].cnt = 5;

	ModbusTx(p);
}

//寄存器单个读取
uint8_t MbRegReadRTU(int16_t addr,void *pp)
{
	char *p=pp;
	uint8_t fs  = 0;
	
	//位置连续时使用casebreak占位 其他地址均填充0
	switch(addr)
	{
		//Measure Data
		case 0	:putFloat(p,Sdata.flows		);break;		//0		流速
		case 1	:break;
		
		case 2	:putFloat(p,Sdata.levelsp	);break;		//2		静压液位
		case 3	:break;
		
		case 6	:putInt32(p,Sdata.uid			);break;		//6		传感器序号
		case 8	:break;
		
		case 38	:putFloat(p,Sdata.temp		);break;		//38	温度
		case 39 :break;
		
		case 40 :putFloat(p,Sdata.levelus	);break;		//40	超声液位
		case 41 :break;
		
		case 42 :putFloat(p,Sdata.flowdt	);break;		//42	瞬时流量
		case 43 :break;
		
		case 44 :putDouble(p,Sdata.flowit	);break;		//44	累积流量
		case 45 :break;
		case 46 :break;
		case 47 :break;
		
		case 48 :putFloat(p,Sdata.dir			);break;		//48	方向 1或者-1
		case 49 :break;
		
		//config Data
		case 100:putInt16(p,Cfg.ptl				);break;		//100 协议类型1 modbusRTU，2 ModbusASCII
		case 101:putInt16(p,Cfg.addr			);break;		//101	设备地址1~254
		case 102:putInt16(p,Cfg.fk				);break;		//102 滤波系数1~99
		case 106:putInt16(p,Cfg.bps				);break;		//106 波特率1~9，4代表9600（默认）1-1200，2-2400，3-4800，4-9600，5-14400，6-19200，7-38400, 8-57600, 9-115200
		case 110:putInt16(p,Cfg.canal			);break;		//110	水渠类型 （0矩形、1圆形、2等腰梯形、3三角形）
		
		case 112:putFloat(p,Cfg.bcpt			);break;		//112	底部补偿
		case 113:break;
		case 114:putFloat(p,Cfg.angel			);break;		//114 角度
		case 115:break;
		case 116:putFloat(p,Cfg.wsize			);break;		//116	宽度尺寸
		case 117:break;
		case 120:putFloat(p,Cfg.thd				);break;		//120 流速门限
		case 121:break;
		case 122:putFloat(p,Cfg.fsk				);break;		//122 流速率定系数，默认1
		case 123:break;
		
		case 128:putInt16(p,Cfg.flowdtS		);break;		//128 计算瞬时流量的液位选择0超声，1压力
		case 130:putInt16(p,Cfg.flowdtT		);break;		//130 瞬时流量单位选择 ，0 立方米/s; 1立方米/h
		
		case 132:putDouble(p,Sdata.flowit	);break;		//44	累积流量
		case 133:break;
		case 134:break;
		case 135:break;
		
		//this Config Data
		case 200:putFloat(p,Cfg.out4ma 		);break;		//200 4ma输出
		case 201:break;
		case 202:putFloat(p,Cfg.out20ma		);break;		//202 20ma输出
		case 203:break;
		case 204:putFloat(p,Cfg.relay1		);break;		//204 继电器1
		case 205:break;
		case 206:putFloat(p,Cfg.relay2		);break;		//206 继电器2
		case 207:break;
		case 208:putInt16(p,Cfg.dsecsd		);break;		//208 记录间隔

		default:putInt16(p,0x0000);break;
	}
	return fs;
}


//寄存器单个写入
void MbRegWriteRTU(int16_t addr,void *pp)
{
	char *p=pp;
	switch(addr)
	{
		
		case 100:Cfg.ptl	 		= getInt16(p); Cfg.update = 201; break; //100 通信协议
		case 101:Cfg.addr 		= getInt16(p); Cfg.update = 203; break; //101 通信地址
		case 102:Cfg.fk	 			= getInt16(p); Cfg.update = 102; break; //102 滤波系数
		case 106:Cfg.bps	    = getInt16(p); Cfg.update = 202; break; //106 波特率
		case 110:Cfg.canal 		= getInt16(p); Cfg.update = 110; break; //110 渠道类型
		
		case 112:Cfg.bcpt 		= getFloat(p); Cfg.update = 112; break; //112 底部补偿
		case 114:Cfg.angel 		= getFloat(p); Cfg.update = 114; break; //114 角度设置
		case 116:Cfg.wsize 		= getFloat(p); Cfg.update = 116; break; //116 渠道宽度
		case 120:Cfg.thd	 		= getFloat(p); Cfg.update = 120; break; //120 流速阀门
		case 122:Cfg.fsk	 		= getFloat(p); Cfg.update = 122; break; //122 标尺因子
		
		case 128:Cfg.flowdtS	= getInt16(p); Cfg.update = 128; break; //128 液位选择
		case 130:Cfg.flowdtT	= getInt16(p); Cfg.update = 130; break; //130 瞬时流量单位选择 
		
		case 132:Cfg.flowitSet= getDouble(p);Cfg.update = 132; break; //132 设置累计流量
		
		/*---------------------------------------------------------------------------------------------*/

		case 200:Cfg.out4ma 	= getFloat(p); Cfg.update = 0xff;break; //200 4ma对应值
		case 202:Cfg.out20ma 	= getFloat(p); Cfg.update = 0xff;break; //202 20ma对应值
		case 204:Cfg.relay1	 	= getFloat(p); Cfg.update = 0xff;break; //204 继电器输出1
		case 206:Cfg.relay2	 	= getFloat(p); Cfg.update = 0xff;break; //206 继电器输出2
		
		case 208:Cfg.dsecsd	 	= getInt16(p); Cfg.update = 0xff;break; //208 记录间隔
		
		default:break;
	}
}

//寄存器连续读取
void MbNregRead(uint16_t addr,uint16_t nreg,int p)
{
	uint8_t * pbuf = &(ComTx[p].buf[3]);
	ComTx[p].buf[2]=nreg*2;
	
	while(nreg>0)
	{
		 MbRegReadRTU(addr,pbuf);
		 addr += 1;
		 pbuf += 2;
		 nreg--;
	}
}

//寄存器连续写入
void MbNregWrite(uint16_t addr,uint16_t nreg,int p)
{
	uint8_t * pbuf = &(ComRx[p].buf[7]);
	
	while(nreg>0)
	{
		MbRegWriteRTU(addr,pbuf);
		addr += 1;
		pbuf += 2;
		nreg--;
	}
}


void ModBusRTUslave(int p)//ModBusRTU协议接收处理
{
	if((ComRx[p].buf[0] == Cfg.addr)&&
		 (CalCRC16(ComRx[p].buf,ComRx[p].cnt)==0))//是本机的地址 且接收数据CRC正确
	{
		int16_t addr = getInt16(&(ComRx[p].buf[2]));
		
		switch(ComRx[p].buf[1])
		{
			case 3://读寄存器
			case 4://读寄存器
			{
				int16_t nreg = getInt16(&(ComRx[p].buf[4]));//读取的寄存器个数
				
				MbNregRead(addr,nreg,p);
				readTxRTU(p);
			}
			break;

			case 6://写单个寄存器 
			{
				MbRegWriteRTU(addr,&(ComRx[p].buf[4]));
				writeTxRTU(p);
			}
			break;
			
			case 0x10://连续写寄存器
			{
				int16_t nreg = getInt16(&(ComRx[p].buf[4]));//写入的寄存器个数
				
				MbNregWrite(addr,nreg,p);
				writeTxRTU(p);
				break;
			}
			break;
			
			default:break;//不支持的命令
		}
	}
	else
	{
		//00 03/04 00 65 00 01 crcH crcL 
		if((ComRx[p].buf[0] == 0x00)&&
			((ComRx[p].buf[1] == 0x03)||(ComRx[p].buf[1] == 0x04))&&//判断是否是广播方式读地址
			(getInt16(&(ComRx[p].buf[2]))==101)&&
			(getInt16(&(ComRx[p].buf[4]))==1)&&
			(CalCRC16(ComRx[p].buf,ComRx[p].cnt)==0))
		{
			//回传本机地址
			uint8_t * pbuf = &(ComTx[p].buf[3]);
			ComTx[p].buf[2]= 1 * 2;
			MbRegReadRTU(ADDR_UID_READ,pbuf);
			readTxRTU(p);//串口读命令返回
		}
	}    
	ComRx[p].cnt = 0; //接收个数
}

/*------------------------------------------*/


//modbus 从机协议
void ModbusSlave(int p)
{
	//自动协议识别
	
	
	//Modbus ASCII
	if(Cfg.ptl == 2)
	{
		//ASCII 转换成 RTU
		ComRx[p].cnt = putASCIItoRTU(tfbuf,ComRx[p].buf,ComRx[p].cnt);
		if(ComRx[p].cnt != 0)
		{
			memcpy(ComRx[p].buf,tfbuf,ComRx[p].cnt);
			ModBusRTUslave(p);
		}
	}
	else
	{
		ModBusRTUslave(p);
	}
	
}










