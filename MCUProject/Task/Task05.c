/**
  ******************************************************************************
  * File Name          : Task05.c
  * Description        : Code for freeRTOS Task05 Application
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
#include "Task05.h"
#include "Task02.h"

#include "golbaldata.h"
#include "Dwin.h"


/*Golbal data space ----------------------------------------------------------*/

const uint16_t BPST[10] = {
	0,1200,2400,4800,9600,14400,19200,38400,57600
};

/*--------------------------------------------------------------------���Ͳ���*/

//���Ͳ�������
void putMsuDataToScreen(MsuDataTyoedef * msu)
{
	Disp_WrInt16(0x1100,(int32_t)msu->flowdt);												//˲ʱ���� �������� 1100
	Disp_WrInt16(0x1102,(((int32_t)(msu->flowdt*1000))%1000)+1000);		//˲ʱ���� С������ 1102
	Disp_WrInt32(0x1110,(int32_t)msu->flowit);												//�ۻ����� �������� 1110
	Disp_WrInt16(0x1114,((int32_t)(msu->flowit*100))%100+100);				//�ۻ����� С������ 1114
	Disp_WrInt16(0x1116,((int16_t)(msu->flows)));											//����		 �������� 1116
	Disp_WrInt16(0x1118,((int32_t)(msu->flows*1000))%1000+1000);			//����		 С������ 1118
	
	Disp_WrFloat(0x1106,msu->temp);																		//�¶� 1106
	
	if(Cfg.flowdtS == 0)
	{
		Disp_WrInt16(0x1104,msu->levelus * 1000);												//������Һλ 1104
	}
	else
	{
		Disp_WrInt16(0x1104,msu->levelsp * 1000);												//��ѹ��Һλ 1104
	}

}

//������������
void putCfgDataScreen(ConfigTypdef * cfg)
{
	//page 1
	Disp_WrInt16(0x1200,cfg->canal);			//01 ��������1200
	Disp_WrInt16(0x1202,cfg->wsize*1000);	//02 �������1202
	Disp_WrInt16(0x1206,cfg->bcpt*1000);	//03 �ײ�����1206
	Disp_WrInt16(0x1210,cfg->angel);			//04 �Ƕ�����1210
	Disp_WrInt16(0x1212,cfg->flowdtS);		//05 Һλѡ��1212
	
	//page2
	Disp_WrInt16(0x1300,cfg->fk);					//06 �˲�ϵ��1300
	Disp_WrFloat(0x1302,cfg->fsk);				//07 �������1302 F
	Disp_WrInt16(0x1306,cfg->thd);				//08 ���ٷ���1306 F
	Disp_WrInt16(0x1310,cfg->dsecsd);			//09 ��¼���1310
	
	//page3
	Disp_WrInt32(0x1400,cfg->out4ma);			//09 4ma��Ӧֵ   1400 F
	Disp_WrInt32(0x1404,cfg->out20ma);		//10 20ma��Ӧֵ  1404 F
	Disp_WrInt32(0x1408,cfg->relay1);			//11 �̵������1 1408 F
	Disp_WrInt32(0x1412,cfg->relay2);			//12 �̵������2 1412 F
	
	//page4
	Disp_WrInt16(0x1500,cfg->ptl-1);			//13 ͨ��Э��  1500
	Disp_WrInt16(0x1502,BPST[(int32_t)cfg->bps]);	//14 ������			1502
	Disp_WrInt16(0x1504,cfg->addr);				//15 ͨ�ŵ�ַ		1504
	//16 ��������	��1520 ��1522 ��1524
	//17 ʱ������ ʱ1530 ��1532 ��1523
	
	//page5
	Disp_WrInt16(0x1600,cfg->flowdtT == 0);//18 ˲ʱ������λѡ�� 1600
	Disp_WrFloat(0x1602,HSVERSION);	 //��Ӳ���汾�� 1602
	Disp_WrInt32(0x1606,Sdata.uid);	 //�������к� 1606
}

//������������
void putCfgData(void)
{
	//û�еȴ����µĲ���
	if(Cfg.update == 0)
	{
		putCfgDataScreen(&Cfg);
	}
}

//������״̬
void putSensorState(void)
{
	if(Sdata.state != MODEDIS)
	{
		Disp_WrBlock(0x5400,30,447,39,30,0xFFFF);
		Disp_WrBlock(0x5200,122,446,39,1,0xFFFF);
	}
	else
	{
		Disp_WrBlock(0x5400,30,447,39,1,0xFFFF);
		Disp_WrBlock(0x5200,122,446,39,30,0xFFFF);
	}
}

//���ݷ���
void putDataDiv(void)
{
	static uint16_t div = 20;
	div++;
	if(div > 20)
	{
		div = 0;
		
		//���Ͳ�������
		putMsuDataToScreen(&Sdata);
		//����������Ϣ
		putCfgData();
		//������״̬
		putSensorState();
			
		//д��RTC
		Disp_WrRtc();
	}
}
/*--------------------------------------------------------------------���ղ���*/
uint8_t bpsTF(uint16_t bps)
{
	//106 ������1~9��4����9600��Ĭ�ϣ�1-1200��2-2400��3-4800��4-9600��
	// 5-14400��6-19200��7-38400, 8-57600, 9-115200
	switch(bps)
	{
		case 1200:return 1;
		case 2400:return 2;
		case 4800:return 3;
		case 9600:return 4;
		case 14400:return 5;
		case 19200:return 6;
		case 38400:return 7;
		case 57600:return 8;
		//case 115200:return 9;
		default:return 4;
	}
}


//�������ݴ���
uint8_t RxDataAnalyze(uint8_t * data)
{
	//	UsConfigSet(0x002B,0x0008);
	// 	5A A5 06 83 11 00 01 00 78 
	if(data[0] == 0x83)
	{
		uint16_t addr 				= (data[1] << 8)|data[2];
		uint16_t dataInt16 		= (data[4] << 8)|data[5];
		int32_t  dataInt32		= dwinChToInt32(&data[4]);
		float 	 dataFloat 		= dwinChToInt32(&data[4]);
		switch(addr)// 0xff ֻ�����ڱ������޸Ĵ�����
		{
			//page1
			case 0x1200:Cfg.canal 		= dataInt16;			Cfg.update = 110; break; //110 ��������1200
			case 0x1202:Cfg.wsize 		= ((float)dataInt16)/1000;			
																									Cfg.update = 116; break; //116 �������1202 
			case 0x1206:Cfg.bcpt 			= ((float)dataInt16)/1000;			
																									Cfg.update = 112; break; //112 �ײ�����1206 
			case 0x1210:Cfg.angel 		= dataInt16;			Cfg.update = 114; break; //114 �Ƕ�����1210 
			case 0x1212:Cfg.flowdtS		= dataInt16;			Cfg.update = 128; break; //128 Һλѡ��1212
			
			//page2
			case 0x1300:Cfg.fk	 			= dataInt16;			Cfg.update = 102; break; //102 �˲�ϵ��1300
			case 0x1302:Cfg.fsk	 			= dataFloat/100;	Cfg.update = 122; break; //122 �������1302 F
			case 0x1306:Cfg.thd	 			= dataInt16;			Cfg.update = 120; break; //120 ���ٷ���1306 F
			case 0x1310:Cfg.dsecsd	 	= dataInt16;			Cfg.update = 0xff;break; //xxx ��¼���1310
 
			//page3
			case 0x1400:Cfg.out4ma 		= dataInt32;	Cfg.update = 0xff;break; //xxx 4ma��Ӧֵ 1400
			case 0x1404:Cfg.out20ma 	= dataInt32;	Cfg.update = 0xff;break; //xxx 20ma��Ӧֵ 1404
			case 0x1408:Cfg.relay1	 	= dataInt32;	Cfg.update = 0xff;break; //xxx �̵������1 1408
			case 0x1412:Cfg.relay2	 	= dataInt32;	Cfg.update = 0xff;break; //xxx �̵������2 1412

			//page4
			case 0x1500:Cfg.ptl	 		= dataInt16+1;			Cfg.update = 201;break; //100 ͨ��Э��  1500
			case 0x1502:Cfg.bps	= bpsTF(dataInt16);			Cfg.update = 202;break; //106 ������1502
			case 0x1504:Cfg.addr 			= dataInt16;			Cfg.update = 203;break; //101 ͨ�ŵ�ַ1504
			case 0x1520:Time.year = dataInt16;Time.update = 1;break; //xxx �� 1520
			case 0x1522:Time.mon  = dataInt16;Time.update = 1;break; //xxx �� 1522
			case 0x1524:Time.day  = dataInt16;Time.update = 1;break; //xxx �� 1524
			case 0x1530:Time.hour = dataInt16;Time.update = 1;break; //xxx ʱ 1530
			case 0x1532:Time.min  = dataInt16;Time.update = 1;break; //xxx �� 1532
			case 0x1534:Time.sec  = dataInt16;Time.update = 1;break; //xxx �� 1534
			
			//page5
			case 0x1600:Cfg.flowdtT	 	= (dataInt16 == 0);	Cfg.update = 130;break; //130 ˲ʱ������λѡ�� 1600

		}
		//�޸ĺ�ˢ��ҳ����ʾ
		putCfgDataScreen(&Cfg);
		return 1;
	}
	return 0;
}
//���ݷ��ؼ��
int8_t getBackCheck(void)
{
	static int16_t numhist = 0;
	
	if(ComRx[COMDW].cnt != 0)
	{
		if(numhist != ComRx[COMDW].cnt)
		{
			numhist = ComRx[COMDW].cnt;
		}
		else if(numhist == ComRx[COMDW].cnt)
		{
			if(numhist > 7)
			{
				if( (ComRx[COMDW].buf[0] == 0x5A)&&
						(ComRx[COMDW].buf[1] == 0xA5)&&
						((ComRx[COMDW].buf[2] == 0x06)||(ComRx[COMDW].buf[2] == 0x08))
					)
				{
					RxDataAnalyze(&ComRx[COMDW].buf[3]);
					numhist = 0;
					ComRx[COMDW].cnt = 0;
					return 1;
				}
				
			}
			numhist = 0;
			ComRx[COMDW].cnt = 0;
		}
	}
	return 0;
}

void Task05Main(void)
{

	//�ȴ�FM������ȡ
	waitCfgInitState();
	
	for(;;)
	{
		//���ݷ���
		putDataDiv();

		//��ȡ�޸�
		getBackCheck();
		
		osDelay(5);
	}
}



