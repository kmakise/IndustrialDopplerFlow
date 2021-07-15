/**
  ******************************************************************************
  * File Name          : ModbusSlave.c
  * Description        : Code for Modbus Protocol slave 
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-6
  * @brief   Modbus �ӻ�ͨ��Э��
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
 
 //modbus ����
 void ModbusTx(int p)
 {
	 //Modbus ASCII
	if(Cfg.ptl == 2)
	{
		//ASCII ת���� RTU
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
 
//���ڶ������
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
//����д����أ����ؽ��յ������
void writeTxRTU(int p)
{
	int16_t SendCrc;
	memcpy(ComTx[p].buf,ComRx[p].buf,6);//���ƽ��յ�������
	SendCrc = CalCRC16(ComTx[p].buf,6);
	putInt16(&ComTx[p].buf[6],SendCrc); 
	ComTx[p].cnt = 8;//���ع̶���8���ֽ�
	
	ModbusTx(p);
}
//�����뷵��
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

//�Ĵ���������ȡ
uint8_t MbRegReadRTU(int16_t addr,void *pp)
{
	char *p=pp;
	uint8_t fs  = 0;
	
	//λ������ʱʹ��casebreakռλ ������ַ�����0
	switch(addr)
	{
		//Measure Data
		case 0	:putFloat(p,Sdata.flows		);break;		//0		����
		case 1	:break;
		
		case 2	:putFloat(p,Sdata.levelsp	);break;		//2		��ѹҺλ
		case 3	:break;
		
		case 6	:putInt32(p,Sdata.uid			);break;		//6		���������
		case 8	:break;
		
		case 38	:putFloat(p,Sdata.temp		);break;		//38	�¶�
		case 39 :break;
		
		case 40 :putFloat(p,Sdata.levelus	);break;		//40	����Һλ
		case 41 :break;
		
		case 42 :putFloat(p,Sdata.flowdt	);break;		//42	˲ʱ����
		case 43 :break;
		
		case 44 :putDouble(p,Sdata.flowit	);break;		//44	�ۻ�����
		case 45 :break;
		case 46 :break;
		case 47 :break;
		
		case 48 :putFloat(p,Sdata.dir			);break;		//48	���� 1����-1
		case 49 :break;
		
		//config Data
		case 100:putInt16(p,Cfg.ptl				);break;		//100 Э������1 modbusRTU��2 ModbusASCII
		case 101:putInt16(p,Cfg.addr			);break;		//101	�豸��ַ1~254
		case 102:putInt16(p,Cfg.fk				);break;		//102 �˲�ϵ��1~99
		case 106:putInt16(p,Cfg.bps				);break;		//106 ������1~9��4����9600��Ĭ�ϣ�1-1200��2-2400��3-4800��4-9600��5-14400��6-19200��7-38400, 8-57600, 9-115200
		case 110:putInt16(p,Cfg.canal			);break;		//110	ˮ������ ��0���Ρ�1Բ�Ρ�2�������Ρ�3�����Σ�
		
		case 112:putFloat(p,Cfg.bcpt			);break;		//112	�ײ�����
		case 113:break;
		case 114:putFloat(p,Cfg.angel			);break;		//114 �Ƕ�
		case 115:break;
		case 116:putFloat(p,Cfg.wsize			);break;		//116	��ȳߴ�
		case 117:break;
		case 120:putFloat(p,Cfg.thd				);break;		//120 ��������
		case 121:break;
		case 122:putFloat(p,Cfg.fsk				);break;		//122 �����ʶ�ϵ����Ĭ��1
		case 123:break;
		
		case 128:putInt16(p,Cfg.flowdtS		);break;		//128 ����˲ʱ������Һλѡ��0������1ѹ��
		case 130:putInt16(p,Cfg.flowdtT		);break;		//130 ˲ʱ������λѡ�� ��0 ������/s; 1������/h
		
		case 132:putDouble(p,Sdata.flowit	);break;		//44	�ۻ�����
		case 133:break;
		case 134:break;
		case 135:break;
		
		//this Config Data
		case 200:putFloat(p,Cfg.out4ma 		);break;		//200 4ma���
		case 201:break;
		case 202:putFloat(p,Cfg.out20ma		);break;		//202 20ma���
		case 203:break;
		case 204:putFloat(p,Cfg.relay1		);break;		//204 �̵���1
		case 205:break;
		case 206:putFloat(p,Cfg.relay2		);break;		//206 �̵���2
		case 207:break;
		case 208:putInt16(p,Cfg.dsecsd		);break;		//208 ��¼���

		default:putInt16(p,0x0000);break;
	}
	return fs;
}


//�Ĵ�������д��
void MbRegWriteRTU(int16_t addr,void *pp)
{
	char *p=pp;
	switch(addr)
	{
		
		case 100:Cfg.ptl	 		= getInt16(p); Cfg.update = 201; break; //100 ͨ��Э��
		case 101:Cfg.addr 		= getInt16(p); Cfg.update = 203; break; //101 ͨ�ŵ�ַ
		case 102:Cfg.fk	 			= getInt16(p); Cfg.update = 102; break; //102 �˲�ϵ��
		case 106:Cfg.bps	    = getInt16(p); Cfg.update = 202; break; //106 ������
		case 110:Cfg.canal 		= getInt16(p); Cfg.update = 110; break; //110 ��������
		
		case 112:Cfg.bcpt 		= getFloat(p); Cfg.update = 112; break; //112 �ײ�����
		case 114:Cfg.angel 		= getFloat(p); Cfg.update = 114; break; //114 �Ƕ�����
		case 116:Cfg.wsize 		= getFloat(p); Cfg.update = 116; break; //116 �������
		case 120:Cfg.thd	 		= getFloat(p); Cfg.update = 120; break; //120 ���ٷ���
		case 122:Cfg.fsk	 		= getFloat(p); Cfg.update = 122; break; //122 �������
		
		case 128:Cfg.flowdtS	= getInt16(p); Cfg.update = 128; break; //128 Һλѡ��
		case 130:Cfg.flowdtT	= getInt16(p); Cfg.update = 130; break; //130 ˲ʱ������λѡ�� 
		
		case 132:Cfg.flowitSet= getDouble(p);Cfg.update = 132; break; //132 �����ۼ�����
		
		/*---------------------------------------------------------------------------------------------*/

		case 200:Cfg.out4ma 	= getFloat(p); Cfg.update = 0xff;break; //200 4ma��Ӧֵ
		case 202:Cfg.out20ma 	= getFloat(p); Cfg.update = 0xff;break; //202 20ma��Ӧֵ
		case 204:Cfg.relay1	 	= getFloat(p); Cfg.update = 0xff;break; //204 �̵������1
		case 206:Cfg.relay2	 	= getFloat(p); Cfg.update = 0xff;break; //206 �̵������2
		
		case 208:Cfg.dsecsd	 	= getInt16(p); Cfg.update = 0xff;break; //208 ��¼���
		
		default:break;
	}
}

//�Ĵ���������ȡ
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

//�Ĵ�������д��
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


void ModBusRTUslave(int p)//ModBusRTUЭ����մ���
{
	if((ComRx[p].buf[0] == Cfg.addr)&&
		 (CalCRC16(ComRx[p].buf,ComRx[p].cnt)==0))//�Ǳ����ĵ�ַ �ҽ�������CRC��ȷ
	{
		int16_t addr = getInt16(&(ComRx[p].buf[2]));
		
		switch(ComRx[p].buf[1])
		{
			case 3://���Ĵ���
			case 4://���Ĵ���
			{
				int16_t nreg = getInt16(&(ComRx[p].buf[4]));//��ȡ�ļĴ�������
				
				MbNregRead(addr,nreg,p);
				readTxRTU(p);
			}
			break;

			case 6://д�����Ĵ��� 
			{
				MbRegWriteRTU(addr,&(ComRx[p].buf[4]));
				writeTxRTU(p);
			}
			break;
			
			case 0x10://����д�Ĵ���
			{
				int16_t nreg = getInt16(&(ComRx[p].buf[4]));//д��ļĴ�������
				
				MbNregWrite(addr,nreg,p);
				writeTxRTU(p);
				break;
			}
			break;
			
			default:break;//��֧�ֵ�����
		}
	}
	else
	{
		//00 03/04 00 65 00 01 crcH crcL 
		if((ComRx[p].buf[0] == 0x00)&&
			((ComRx[p].buf[1] == 0x03)||(ComRx[p].buf[1] == 0x04))&&//�ж��Ƿ��ǹ㲥��ʽ����ַ
			(getInt16(&(ComRx[p].buf[2]))==101)&&
			(getInt16(&(ComRx[p].buf[4]))==1)&&
			(CalCRC16(ComRx[p].buf,ComRx[p].cnt)==0))
		{
			//�ش�������ַ
			uint8_t * pbuf = &(ComTx[p].buf[3]);
			ComTx[p].buf[2]= 1 * 2;
			MbRegReadRTU(ADDR_UID_READ,pbuf);
			readTxRTU(p);//���ڶ������
		}
	}    
	ComRx[p].cnt = 0; //���ո���
}

/*------------------------------------------*/


//modbus �ӻ�Э��
void ModbusSlave(int p)
{
	//�Զ�Э��ʶ��
	
	
	//Modbus ASCII
	if(Cfg.ptl == 2)
	{
		//ASCII ת���� RTU
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










