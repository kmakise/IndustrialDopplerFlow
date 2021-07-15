/**
  ******************************************************************************
  * File Name          : Task05.c
  * Description        : Code for freeRTOS Task05 Application
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-1
  * @brief   迪文屏通信
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

/*--------------------------------------------------------------------发送部分*/

//推送测量数据
void putMsuDataToScreen(MsuDataTyoedef * msu)
{
	Disp_WrInt16(0x1100,(int32_t)msu->flowdt);												//瞬时流量 整数部分 1100
	Disp_WrInt16(0x1102,(((int32_t)(msu->flowdt*1000))%1000)+1000);		//瞬时流量 小数部分 1102
	Disp_WrInt32(0x1110,(int32_t)msu->flowit);												//累积流量 整数部分 1110
	Disp_WrInt16(0x1114,((int32_t)(msu->flowit*100))%100+100);				//累积流量 小数部分 1114
	Disp_WrInt16(0x1116,((int16_t)(msu->flows)));											//流速		 整数部分 1116
	Disp_WrInt16(0x1118,((int32_t)(msu->flows*1000))%1000+1000);			//流速		 小数部分 1118
	
	Disp_WrFloat(0x1106,msu->temp);																		//温度 1106
	
	if(Cfg.flowdtS == 0)
	{
		Disp_WrInt16(0x1104,msu->levelus * 1000);												//超声力液位 1104
	}
	else
	{
		Disp_WrInt16(0x1104,msu->levelsp * 1000);												//静压力液位 1104
	}

}

//推送配置数据
void putCfgDataScreen(ConfigTypdef * cfg)
{
	//page 1
	Disp_WrInt16(0x1200,cfg->canal);			//01 渠道类型1200
	Disp_WrInt16(0x1202,cfg->wsize*1000);	//02 渠道宽度1202
	Disp_WrInt16(0x1206,cfg->bcpt*1000);	//03 底部补偿1206
	Disp_WrInt16(0x1210,cfg->angel);			//04 角度设置1210
	Disp_WrInt16(0x1212,cfg->flowdtS);		//05 液位选择1212
	
	//page2
	Disp_WrInt16(0x1300,cfg->fk);					//06 滤波系数1300
	Disp_WrFloat(0x1302,cfg->fsk);				//07 标尺因子1302 F
	Disp_WrInt16(0x1306,cfg->thd);				//08 流速阀门1306 F
	Disp_WrInt16(0x1310,cfg->dsecsd);			//09 记录间隔1310
	
	//page3
	Disp_WrInt32(0x1400,cfg->out4ma);			//09 4ma对应值   1400 F
	Disp_WrInt32(0x1404,cfg->out20ma);		//10 20ma对应值  1404 F
	Disp_WrInt32(0x1408,cfg->relay1);			//11 继电器输出1 1408 F
	Disp_WrInt32(0x1412,cfg->relay2);			//12 继电器输出2 1412 F
	
	//page4
	Disp_WrInt16(0x1500,cfg->ptl-1);			//13 通信协议  1500
	Disp_WrInt16(0x1502,BPST[(int32_t)cfg->bps]);	//14 波特率			1502
	Disp_WrInt16(0x1504,cfg->addr);				//15 通信地址		1504
	//16 日期设置	年1520 月1522 日1524
	//17 时间设置 时1530 分1532 秒1523
	
	//page5
	Disp_WrInt16(0x1600,cfg->flowdtT == 0);//18 瞬时流量单位选择 1600
	Disp_WrFloat(0x1602,HSVERSION);	 //软硬件版本号 1602
	Disp_WrInt32(0x1606,Sdata.uid);	 //出厂序列号 1606
}

//推送配置数据
void putCfgData(void)
{
	//没有等待更新的参数
	if(Cfg.update == 0)
	{
		putCfgDataScreen(&Cfg);
	}
}

//传感器状态
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

//数据发送
void putDataDiv(void)
{
	static uint16_t div = 20;
	div++;
	if(div > 20)
	{
		div = 0;
		
		//推送测量数据
		putMsuDataToScreen(&Sdata);
		//推送配置信息
		putCfgData();
		//传感器状态
		putSensorState();
			
		//写入RTC
		Disp_WrRtc();
	}
}
/*--------------------------------------------------------------------接收部分*/
uint8_t bpsTF(uint16_t bps)
{
	//106 波特率1~9，4代表9600（默认）1-1200，2-2400，3-4800，4-9600，
	// 5-14400，6-19200，7-38400, 8-57600, 9-115200
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


//返回数据处理
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
		switch(addr)// 0xff 只保存在本机不修改传感器
		{
			//page1
			case 0x1200:Cfg.canal 		= dataInt16;			Cfg.update = 110; break; //110 渠道类型1200
			case 0x1202:Cfg.wsize 		= ((float)dataInt16)/1000;			
																									Cfg.update = 116; break; //116 渠道宽度1202 
			case 0x1206:Cfg.bcpt 			= ((float)dataInt16)/1000;			
																									Cfg.update = 112; break; //112 底部补偿1206 
			case 0x1210:Cfg.angel 		= dataInt16;			Cfg.update = 114; break; //114 角度设置1210 
			case 0x1212:Cfg.flowdtS		= dataInt16;			Cfg.update = 128; break; //128 液位选择1212
			
			//page2
			case 0x1300:Cfg.fk	 			= dataInt16;			Cfg.update = 102; break; //102 滤波系数1300
			case 0x1302:Cfg.fsk	 			= dataFloat/100;	Cfg.update = 122; break; //122 标尺因子1302 F
			case 0x1306:Cfg.thd	 			= dataInt16;			Cfg.update = 120; break; //120 流速阀门1306 F
			case 0x1310:Cfg.dsecsd	 	= dataInt16;			Cfg.update = 0xff;break; //xxx 记录间隔1310
 
			//page3
			case 0x1400:Cfg.out4ma 		= dataInt32;	Cfg.update = 0xff;break; //xxx 4ma对应值 1400
			case 0x1404:Cfg.out20ma 	= dataInt32;	Cfg.update = 0xff;break; //xxx 20ma对应值 1404
			case 0x1408:Cfg.relay1	 	= dataInt32;	Cfg.update = 0xff;break; //xxx 继电器输出1 1408
			case 0x1412:Cfg.relay2	 	= dataInt32;	Cfg.update = 0xff;break; //xxx 继电器输出2 1412

			//page4
			case 0x1500:Cfg.ptl	 		= dataInt16+1;			Cfg.update = 201;break; //100 通信协议  1500
			case 0x1502:Cfg.bps	= bpsTF(dataInt16);			Cfg.update = 202;break; //106 波特率1502
			case 0x1504:Cfg.addr 			= dataInt16;			Cfg.update = 203;break; //101 通信地址1504
			case 0x1520:Time.year = dataInt16;Time.update = 1;break; //xxx 年 1520
			case 0x1522:Time.mon  = dataInt16;Time.update = 1;break; //xxx 月 1522
			case 0x1524:Time.day  = dataInt16;Time.update = 1;break; //xxx 日 1524
			case 0x1530:Time.hour = dataInt16;Time.update = 1;break; //xxx 时 1530
			case 0x1532:Time.min  = dataInt16;Time.update = 1;break; //xxx 分 1532
			case 0x1534:Time.sec  = dataInt16;Time.update = 1;break; //xxx 秒 1534
			
			//page5
			case 0x1600:Cfg.flowdtT	 	= (dataInt16 == 0);	Cfg.update = 130;break; //130 瞬时流量单位选择 1600

		}
		//修改后刷新页面显示
		putCfgDataScreen(&Cfg);
		return 1;
	}
	return 0;
}
//数据返回检查
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

	//等待FM参数读取
	waitCfgInitState();
	
	for(;;)
	{
		//数据发送
		putDataDiv();

		//获取修改
		getBackCheck();
		
		osDelay(5);
	}
}



