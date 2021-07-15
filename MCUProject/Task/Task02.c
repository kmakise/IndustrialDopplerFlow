/**
  ******************************************************************************
  * File Name          : Task02.c
  * Description        : Code for freeRTOS Task02 Application
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-1
  * @brief   ʱ�������������
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
//���ò���
ConfigTypdef Cfg;
TimeTpyedef Time;
uint8_t  CfgRdInit = 0;

//���ʱ��
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

//LED״̬��
void LEDstate(void)
{
	GPIOC->BSRR = GPIO_PIN_2;
	osDelay(500);
	GPIOC->BRR = GPIO_PIN_2;
	osDelay(500);
}

/*--------------------------------------------------------------����洢������*/
uint8_t FMemBuf[128];//���绺����

//д��������Ϣ������洢��
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
		//д�뱾��
		WriteFM(FM_ADDR_CFG_BASE,FMemBuf,len + 2);
		//д�뱸��
		WriteFM(FM_ADDR_CFG_BKUP,FMemBuf,len + 2);
	} 
}

//����Ĭ�ϲ���
void loadDefaultCfg(ConfigTypdef * cfg)
{
	cfg->ptl			= 1;	//100 Э������1 modbusRTU��2 ModbusASCII
	cfg->addr 		= 1;	//101	�豸��ַ1~254
	cfg->angel 		= 45;	//114	�Ƕ�
	cfg->bcpt 		= 0;	//112	�ײ�����
	cfg->bps			= 4;	//106 ������1~9��4����9600��Ĭ�ϣ�1-1200��2-2400��3-4800��4-9600��5-14400��6-19200��7-38400, 8-57600, 9-115200
	cfg->canal  	= 0;	//110	ˮ������ ��0���Ρ�1Բ�Ρ�2�������Ρ�3�����Σ�

	cfg->fk				= 10;	//102 �˲�ϵ��1~99
	cfg->flowdtS	=	0;	//128 ����˲ʱ������Һλѡ��0������1ѹ��
	cfg->flowdtT 	= 0;	//130 ˲ʱ������λѡ�� ��0 ������/s; 1������/h
	cfg->flowitSet= 0;	//132 �����ۼ�����
	cfg->fsk			= 1;	//122 �����ʶ�ϵ����Ĭ��1
	
	cfg->out20ma 	= 0;	//4ma���
	cfg->out4ma		= 10;	//20ma���
	cfg->relay1		= 0;	//�̵���1
	cfg->relay2		= 0;	//�̵���2
	cfg->dsecsd 	= 10;	//��¼���
	
	cfg->update 	= 0;
}


//�������ȡ����
void readFMtoCfg(ConfigTypdef * cfg)
{
	//��ȡ���ݵ�������
	ReadFM(FM_ADDR_CFG_BASE,FMemBuf,sizeof(ConfigTypdef) + 16);
	//����CRCУ��
	if(CalCRC16(FMemBuf,sizeof(ConfigTypdef)+2)==0)
	{
		memcpy(cfg,FMemBuf,sizeof(ConfigTypdef));
	}
	//����У��ʧ�� ��ȡ������
	else
	{
		//��ȡ���ݵ�������
		ReadFM(FM_ADDR_CFG_BKUP,FMemBuf,sizeof(ConfigTypdef) + 16);
		//������CRCУ��
		if(CalCRC16(FMemBuf,sizeof(ConfigTypdef)+2)==0)
		{
			memcpy(cfg,FMemBuf,sizeof(ConfigTypdef));
		}
		else//������ʧ��ʱ ����Ĭ�ϲ�����д��FM
		{
			//����
			loadDefaultCfg(cfg);
			//д��FM
			cfg->update = 0xFF;
			writeCfgToFM(cfg);
		}
	}
	cfg->update = 0;
	CfgRdInit = 1;
}

//��ò�����ʼ��״̬
void waitCfgInitState(void)
{
	while(CfgRdInit == 0);
}

void Task02Main(void)
{
	//��ȡʱ��
	Time.update = 0;
	ReadTime();
	
	//������ȡ
	readFMtoCfg(&Cfg);
	
	for(;;)
	{
		//�����ʱ
		timeRunning();
		
		//���²���д��FM
		writeCfgToFM(&Cfg);
		
		//״̬�� 1s���ڿ���
		LEDstate();
	}
}



