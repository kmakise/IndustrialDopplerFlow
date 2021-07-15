/**
  ******************************************************************************
  * File Name          : Task03.c
  * Description        : Code for freeRTOS Task03 Application
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-1
  * @brief   ������ͨ��
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


/*---------------------------------------------------------------�������ݶ�ȡ*/
MsuDataTyoedef Sensor[2];//�������ڵ�����
MsuDataTyoedef Sdata;		 //��������

//���ݾ�ֵ
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

//˫ͨ����������ȡ
void readMsuData(MsuDataTyoedef * sd)
{
	static uint16_t div = 0;
	div++;
	if(div > 30)//3S����һ��
	{
		div = 0;
		
		//��ȡ��������
		sensorMeasureDataRead(COMSA,&Sensor[0]);
		sensorMeasureDataRead(COMSB,&Sensor[1]);
		
		//sensorConfigDataRead(COMSA,&Cfg);

		//���ݺϳ�
		
		//����Aͨ��ģʽ ֱ�����ݸ���
		if(Sensor[0].state != 0 && Sensor[1].state == 0)
		{
			memcpy(sd,&Sensor[0],sizeof(MsuDataTyoedef));
			sd->state = MODECHA;
			sd->uid = Sensor[0].uid;
		}
		//����Bͨ��ģʽ ֱ�����ݸ���
		if(Sensor[0].state == 0 && Sensor[1].state != 0)
		{
			memcpy(sd,&Sensor[1],sizeof(MsuDataTyoedef));
			sd->state = MODECHB;
			sd->uid = Sensor[1].uid;
		}
		//˫ͨ��ģʽ ���ݾ�ֵд��
		if(Sensor[0].state != 0 && Sensor[1].state != 0)
		{
			msuDataAverage(&Sensor[0],&Sensor[1],sd);
			sd->state = MODEALL;
			sd->uid = Sensor[0].uid;
		}
		//������ʧЧ
		if(Sensor[0].state == 0 && Sensor[1].state == 0)
		{
			sd->state = MODEDIS;
		}
	}
}

/*---------------------------------------------------------------��������ͬ��*/
//���ò�����ʱ�����
ConfigTypdef CfgTemp;	

//���ò���ͬ�� �Ա���Ϊ��׼ �򴫸������� ����FM��������
// cfgb ��׼���� cfgs���������� st������״̬
void cfgDataSync(int st,ConfigTypdef * cfgb,ConfigTypdef * cfgs)
{
	if(cfgs->fk != cfgb->fk)
	{	
		//102 �˲�ϵ��1~99
		cfgs->fk =	cfgb->fk;		
		cfgs->update = 102;
		sensorConfigDataWrite(st,cfgs);
	}			
	if(cfgs->canal != cfgb->canal)
	{	
		//110	ˮ������ ��0���Ρ�1Բ�Ρ�2�������Ρ�3�����Σ�
		cfgs->canal =	cfgb->canal;		
		cfgs->update = 110;
		sensorConfigDataWrite(st,cfgs);
	}		                     
	if(cfgs->bcpt != cfgb->bcpt)
	{	
		//112	�ײ�����
		cfgs->bcpt =	cfgb->bcpt;		
		cfgs->update = 112;
		sensorConfigDataWrite(st,cfgs);
	}		
	if(cfgs->angel != cfgb->angel)
	{	
		//114	�Ƕ�
		cfgs->angel =	cfgb->angel;		
		cfgs->update = 114;
		sensorConfigDataWrite(st,cfgs);
	}
	if(cfgs->wsize != cfgb->wsize)
	{	
		//116	��ȳߴ�
		cfgs->wsize =	cfgb->wsize;		
		cfgs->update = 116;
		sensorConfigDataWrite(st,cfgs);
	}
	if(cfgs->fsk != cfgb->fsk)
	{	
		//122 �����ʶ�ϵ����Ĭ��1
		cfgs->fsk =	cfgb->fsk;		
		cfgs->update = 122;
		sensorConfigDataWrite(st,cfgs);
	}                     
	if(cfgs->flowdtS != cfgb->flowdtS)
	{	
		//128 ����˲ʱ������Һλѡ��0������1ѹ��
		cfgs->flowdtS =	cfgb->flowdtS;		
		cfgs->update = 128;
		sensorConfigDataWrite(st,cfgs);
	}
	if(cfgs->flowdtT != cfgb->flowdtT)
	{	
		//130 ˲ʱ������λѡ�� ��0 ������/s; 1������/h
		cfgs->flowdtT =	cfgb->flowdtT;		
		cfgs->update = 130;
		sensorConfigDataWrite(st,cfgs);
	}  

	
	//�����ۻ�����ͬ����˵����
	//�ۻ��������ӱ���ͬ���������� ���� Ҳ���洢�ۻ���������
	//ֻ�е�����PC�޸��ۻ�����ʱ���޸����ݲ����µ�������
	
//	if(cfgs->flowitSet	!= cfgb->flowitSet) 
//	{	
//		//132 �����ۼ�����
//		cfgs->flowitSet =	cfgb->flowitSet;		
//		cfgs->update = 132;
//		sensorConfigDataWrite(st,cfgs);
//	}
	
}

//��ȡ ��֤ �� �����������뱾����������Ϣͬ��
void readCfgData(ConfigTypdef * cfg,int st)
{
	static uint16_t div = 0;
	div++;
	if(div > 500)//5sͬ��һ��
	{
		div = 0;
		
		//�ڷǸ���״̬ʱ ��ȡ ����������
		if(cfg->update == 0)
		{
			if(st == MODECHA)//CHAģʽ
			{
				sensorConfigDataRead(COMSA,&CfgTemp);
				cfgDataSync(COMSA,cfg,&CfgTemp);
				
			}
			else if(st == MODECHB)//CHBģʽ
			{
				sensorConfigDataRead(COMSB,&CfgTemp);
				cfgDataSync(COMSB,cfg,&CfgTemp);
			}
			else if(st == MODEALL)//˫ͨ��ģʽ
			{
				sensorConfigDataRead(COMSA,&CfgTemp);
				cfgDataSync(COMSA,cfg,&CfgTemp);
				sensorConfigDataRead(COMSB,&CfgTemp);
				cfgDataSync(COMSB,cfg,&CfgTemp);
			}
		}
	}
}

/*---------------------------------------------------------------�������ݸ���*/

//д�봫��������
void writeCfgToSensor(ConfigTypdef * cfg,int st)
{
	//����1�����²���Ϊ��ַ״̬ʱд����´���������
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
		//����2��׼��д��FM �洢��
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
	
	//�ȴ�FM������ȡ
	waitCfgInitState();
	
	for(;;)
	{
		//��ȡ��������
		readMsuData(&Sdata);
		
		//��ȡ�������ݲ�ͬ�� �����򴫸�����дFM (����������ʱ�Żᷢ��)
		readCfgData(&Cfg,Sdata.state);
		
		//����д�봫�������� PC��dw�޸� ������������дFM
		writeCfgToSensor(&Cfg,Sdata.state);
		
		osDelay(10);
	}
}



