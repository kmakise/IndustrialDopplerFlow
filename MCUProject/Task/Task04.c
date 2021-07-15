/**
  ******************************************************************************
  * File Name          : Task04.c
  * Description        : Code for freeRTOS Task04 Application
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-1
  * @brief   PCͨ��
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

//�˿ڲ������޸� 1-1200��2-2400��3-4800��4-9600��5-14400��6-19200��7-38400, 8-57600, 9-115200
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


//PCͨ�������޸�
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
			case 201://Э���޸�
			case 203://��ַ�޸�
				break;
		}
		cfg->update = 0xff;
	}
}


//���ռ��
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
	//�ȴ�FM������ȡ
	waitCfgInitState();
	//����˿�����
	baudRate(Cfg.bps);

	for(;;)
	{
		//modbus ͨ�ż��
		ModBusComCheck(COMPC);
		
		//PCͨ�������޸�
		pcComConfig(&Cfg);
			
		osDelay(10);
	}
}



