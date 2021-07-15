/**
  ******************************************************************************
  * File Name          : Doppler.c
  * Description        : Code for Doppler sensor dirver and communication
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-2
  * @brief   ��ѧ���������ٲ���������
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


//���մ��� ���� 0���մ��� 1���ճɹ� 
int8_t getRecState(int p)
{ 
	uint8_t cnthist = 0;
	
	//�ȴ����ݿ�ʼ���� �ȴ�1000ms
	for(int i=100;i>0;i--) 
	{		   
		DELAY(10);
		if(ComRx[p].cnt != 0)
		{	   
			break;		
		}	
	}
	
	//�ȴ����ݽ������
	while(cnthist != ComRx[p].cnt)
	{
		cnthist = ComRx[p].cnt;
		DELAY(2);
	}
	
	//У��
  if(ComRx[p].cnt > 7)
	{			  
		if(CalCRC16(ComRx[p].buf,ComRx[p].cnt)==0)
		{
			return 1;
		}
	}
	return 0;	
}

//���� ���������ݵ�ȫ�����ݽṹ 0 ��ʼ
void bufToMsuStruct(uint8_t * buf,MsuDataTyoedef * st)
{
	st->flows 	= getFloat (&(buf[0 	* 2]));		//0		����
	st->levelsp = getFloat (&(buf[2 	* 2]));		//2		��ѹҺλ
	st->uid 		= getInt32 (&(buf[6 	* 2]));		//6		���������
	st->temp 		= getFloat (&(buf[38 	* 2]));		//38	�¶�
	st->levelus = getFloat (&(buf[40	* 2]));		//40	����Һλ
	st->flowdt 	= getFloat (&(buf[42 	* 2]));		//42	˲ʱ����
	st->flowit 	= getDouble(&(buf[44 	* 2]));		//44	�ۻ�����
	st->dir 		= getFloat (&(buf[48 	* 2]));		//48	���� 1����-1
}

//���� ���������ݵ�ȫ�����ݽṹ 100��ʼ
void bufToCfgStruct(uint8_t * buf,ConfigTypdef * st)
{
	//st->ptl 			= getInt16 (&(buf[0 	* 2]));			//100 Э������1 modbusRTU��2 ModbusASCII
	//st->addr 			= getInt16 (&(buf[1 	* 2]));			//101	�豸��ַ1~254
	st->fk 				= getInt16 (&(buf[2 	* 2]));				//102 �˲�ϵ��1~99
	//st->bps 			= getInt16 (&(buf[6 	* 2]));			//106 ������1~9��4����9600��Ĭ�ϣ�1-1200��2-2400��3-4800��4-9600��5-14400��6-19200��7-38400, 8-57600, 9-115200
	st->canal 		= getInt16 (&(buf[10 	* 2]));				//110	ˮ������ ��0���Ρ�1Բ�Ρ�2�������Ρ�3�����Σ�
	st->bcpt 			= getFloat (&(buf[12 	* 2]));				//112	�ײ�����
	st->angel 		= getFloat (&(buf[14 	* 2]));				//114	�Ƕ�
	st->wsize		 	= getFloat (&(buf[16 	* 2]));				//116	��ȳߴ�
	st->thd		 		= getFloat (&(buf[20 	* 2]));				//120	��������
	st->fsk 			= getFloat (&(buf[22 	* 2]));				//122 �����ʶ�ϵ����Ĭ��1
	st-> flowdtS 	= getInt16 (&(buf[28	* 2]));				//128 ����˲ʱ������Һλѡ��0������1ѹ��
	st->flowdtT 	= getInt16 (&(buf[30	* 2]));				//130 ˲ʱ������λѡ�� ��0 ������/s; 1������/h
	st->flowitSet = getDouble(&(buf[32	* 2]));				//132 �����ۼ�����
}


//���Ͷ�ȡָ��
void TxReadRegCmd(int p,int addr,int num)
{
	 uint16_t	SendCrc;
	
	//�Ĵ�����ȡָ��
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
	
	//��������
	TxData(p);
}

//д�뵥���Ĵ���
void TxWriteRegCmd(int p,int addr,char type,void * num)
{
	uint16_t SendCrc;
	
	//01���豸��ַ��10�������룩00 00����ʼ��ַ��XX XX���Ĵ���������
	//XX���ֽ�����XX XX������XX XX��2*�Ĵ����������ֽ����ݣ�XX XX��CRC16У���룩
	
	
	//�Ĵ���д��ָ��
	ComTx[p].cnt = 0;
	ComTx[p].buf[ComTx[p].cnt++] = 0x01;
	ComTx[p].buf[ComTx[p].cnt++] = 0x10;
  putInt16(&ComTx[p].buf[ComTx[p].cnt],addr);
	ComTx[p].cnt += 2;
	
	
	//��������ѡ��
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
	//��������
	TxData(p);
	
	//�ȴ��ظ�
	getRecState(p);
	ComRx[p].cnt = 0;
}


//��ȡ��������
void sensorMeasureDataRead(int p,MsuDataTyoedef * sr)
{
	TxReadRegCmd(p,0,50);
	
	//�ȴ����ݻش�
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

//��ȡ��������
void sensorConfigDataRead(int p,ConfigTypdef * cfg)
{
	TxReadRegCmd(p,100,40);
	
	//�ȴ����ݻش�
	if(getRecState(p))
	{
		//�޸��µȴ�ʱ��������
		if(cfg->update == 0)
		{
			bufToCfgStruct(&(ComRx[p].buf[3]),cfg);
		}
	}
	ComRx[p].cnt = 0;
}

//д�����ò��� ��������
void sensorConfigDataWrite(int p,ConfigTypdef * cfg)
{
	//ȫд
	if(cfg->update == 0xff)
	{
		
	}	
	//�����Ĵ���
	else if(cfg->update >= 100 && cfg->update <= 132)
	{
		//�����ʺ͵�ַ��д�봫��������
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




