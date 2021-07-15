/**
  ******************************************************************************
  * File Name          : Dwin.c
  * Description        : Code for dwin display driver
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-3
  * @brief   Dwin显示驱动
	******************************************************************************
  * @attention
  * <h2><center>&copy; Copyright (c) kmakise
  * All rights reserved.
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "Dwin.h"
#include "golbaldata.h"
#include "UserCom.h"
#include "string.h"

const uint8_t DispRst[] = {0x82,0x00,0x04,0x55,0xAA,0x5A,0xA5,};

void Disp_SendArr(uint8_t *p,uint8_t n)//发送数组(自动添加帧头)
{
	uint8_t str[3] = {0x5A,0xA5,n};
	memcpy(ComTx[COMDW].buf , str, 3);
	memcpy(&ComTx[COMDW].buf[3],p,n);
	ComTx[COMDW].cnt = n + 3;
	TxData(COMDW);
}

void dwinFloatToCh(void *p,float f)
{
	char *pch = (char *)p;
	char *pf = (char *)&f;
	pch[0] = pf[3];
	pch[1] = pf[2];
	pch[2] = pf[1];
	pch[3] = pf[0];
}

float dwinChToFloat(void *p)
{
	float f;
	char *pch = (char *)p;
	char *pf = (char *)&f;
	pf[3] = pch[0];
	pf[2] = pch[1];
	pf[1] = pch[2];
	pf[0] = pch[3];
	return f;
}

void dwinInt32ToCh(void *p,int32_t l)
{
	char *pch = (char *)p;
	char *pl = (char *)&l;
	pch[0] = pl[3];
	pch[1] = pl[2];
	pch[2] = pl[1];
	pch[3] = pl[0];
}
uint32_t dwinChToInt32(void *p)
{
	int32_t f;
	char *pch = (char *)p;
	char *pf = (char *)&f;
	pf[3] = pch[0];
	pf[2] = pch[1];
	pf[1] = pch[2];
	pf[0] = pch[3];
	return f;
}
//复位显示屏
void Disp_Rst(void) 
{	
	//发送数组(自动添加帧头
	Disp_SendArr((uint8_t*)DispRst,sizeof(DispRst));
}

//设置要显示的图片
void Disp_SetPic(uint8_t pic,ComBufTypedef * com)
{
	uint8_t arr[20],cnt;
	cnt = 0;
	arr[cnt++] = 0x82; 	
	arr[cnt++] = 0x00; 	
	arr[cnt++] = 0x84; 	
	arr[cnt++] = 0x5a; 	
	arr[cnt++] = 0x01; 	
	arr[cnt++] = 0x00; 	
	arr[cnt++] = pic; 	
	Disp_SendArr(arr,cnt);
}
 //读取当前显示的图片
int16_t Disp_ReadPic(void)
{
	int16_t pic = -1;
	uint8_t arr[20],cnt;
	ComRx[COMDW].cnt = 0;
	
	cnt = 0;
	arr[cnt++] = 0x83; 	
	arr[cnt++] = 0x00; 	
	arr[cnt++] = 0x14; 
	arr[cnt++] = 0x01; 
	Disp_SendArr(arr,cnt);
  osDelay(5);
	if(	(ComRx[COMDW].buf[0] == 0x5A)&&
			(ComRx[COMDW].buf[1] == 0xA5)&&
			(ComRx[COMDW].buf[2] == 0x06))
	{
		pic = ComRx[COMDW].buf[8];
	}
	return pic;
}
//写RTC
void Disp_WrRtc(void)
{
	uint8_t arr[20],cnt;
	cnt = 0;
	arr[cnt++] = 0x82; 	
	arr[cnt++] = 0x00; 	
	arr[cnt++] = 0x10; 	
	arr[cnt++] = Time.year;//年
	arr[cnt++] = Time.mon;//月 	
	arr[cnt++] = Time.day;//日 		
	arr[cnt++] = 0;//星期
	arr[cnt++] = Time.hour;//时	
	arr[cnt++] = Time.min;//分 		
	arr[cnt++] = Time.sec;//秒
	arr[cnt++] = 0;//预留0 
	Disp_SendArr(arr,cnt);
}
//写16位整数
void Disp_WrInt16(uint16_t addr,uint16_t data)
{
	uint8_t arr[20],cnt;
	cnt = 0;
	arr[cnt++] = 0x82; 	
	arr[cnt++] = addr>>8; 	
	arr[cnt++] = addr&0xff; 	
	arr[cnt++] = data>>8; 	
	arr[cnt++] = data&0xff; 	
	Disp_SendArr(arr,cnt);
}
//写浮点数
void Disp_WrFloat(uint16_t addr,float fdata)
{
	uint8_t arr[20],cnt;
	cnt = 0;
	arr[cnt++] = 0x82; 	
	arr[cnt++] = addr>>8; 	
	arr[cnt++] = addr&0xff; 	
	dwinFloatToCh(&arr[cnt],fdata); 
	cnt+=4;
	Disp_SendArr(arr,cnt);
}
//写32位整型
void Disp_WrInt32(uint16_t addr,int32_t ldata)
{
	uint8_t arr[20],cnt;
	cnt = 0;
	arr[cnt++] = 0x82; 	
	arr[cnt++] = addr>>8; 	
	arr[cnt++] = addr&0xff; 	
	dwinInt32ToCh(&arr[cnt],ldata); 
	cnt+=4;
	Disp_SendArr(arr,cnt);
}
//写数组
void Disp_WrStr(uint16_t addr,char *p)
{
	uint8_t arr[100],cnt;
	cnt = 0;
	arr[cnt++] = 0x82; 	
	arr[cnt++] = addr>>8; 	
	arr[cnt++] = addr&0xff; 	
	while(*p)
	{
		arr[cnt++] = *p; 	
		p++;
	}	
	Disp_SendArr(arr,cnt);
}

//色块填充
void Disp_WrBlock(uint16_t addr,uint16_t x0,uint16_t y0,uint16_t w,uint16_t h,uint16_t color)
{
	uint8_t arr[30],cnt;
	cnt = 0;
	arr[cnt++] = 0x82; 	
	arr[cnt++] = addr>>8; 	
	arr[cnt++] = addr&0xff;
	arr[cnt++] = 0x00;
	arr[cnt++] = 0x04;//mod
	arr[cnt++] = 0x00;
	arr[cnt++] = 0x01;//num
	arr[cnt++] = x0>>8;
	arr[cnt++] = x0&0xff;//x0
	arr[cnt++] = y0>>8;
	arr[cnt++] = y0&0xff;//y0
	arr[cnt++] = (x0+w)>>8;
	arr[cnt++] = (x0+w)&0xff;//x1
	arr[cnt++] = (y0+h)>>8;
	arr[cnt++] = (y0+h)&0xff;//y1
	arr[cnt++] = color>>8;
	arr[cnt++] = color&0xff;//y1
	arr[cnt++] = 0xff;
	arr[cnt++] = 0x00;
	Disp_SendArr(arr,cnt);
}








