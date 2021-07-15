/**
  ******************************************************************************
  * File Name          : Task04.c
  * Description        : Code for freeRTOS Task04 Application
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-1
  * @brief   PC通信
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
#include "Task04.h"
#include "Task02.h"
#include "usart.h"

#include "golbaldata.h"
#include "ModbusSlave.h"

/*Golbal data space ----------------------------------------------------------*/

//端口波特率修改 1-1200，2-2400，3-4800，4-9600，5-14400，6-19200，7-38400, 8-57600, 9-115200
void baudRate(uint16_t bps)
{
	switch(bps)
	{
		case 2: 	bps = 2400;  	break;
		case 3: 	bps = 4800;  	break;
		case 4: 	bps = 9600;  	break;
		case 6:		bps = 19200; 	break;
		default:	bps = 9600;		break;
	}
  huart3.Init.BaudRate = bps;
  HAL_UART_Init(&huart3);
}


//PC通信配置修改
void pcComConfig(ConfigTypdef * cfg)
{
	if(cfg->update != 0)
	{
		switch(cfg->update)
		{
			case 202:
			{
				baudRate(cfg->bps);
				break;
			}
			case 201://协议修改
			case 203://地址修改
				break;
		}
		cfg->update = 0xff;
	}
}


//接收检查
void ModBusComCheck(int p)
{
	static int8_t lastRxnum = 0;

	if((lastRxnum == ComRx[p].cnt)&&
		 (ComRx[p].cnt != 0))
	{
		lastRxnum = 0;
		ModbusSlave(p);
	}
	else
	{
		lastRxnum = ComRx[p].cnt;
	}
}


void Task04Main(void)
{
	//等待FM参数读取
	waitCfgInitState();
	//载入端口配置
	baudRate(Cfg.bps);

	for(;;)
	{
		//modbus 通信检查
		ModBusComCheck(COMPC);
		
		//PC通信配置修改
		pcComConfig(&Cfg);
			
		osDelay(10);
	}
}



