/**
  ******************************************************************************
  * File Name          : Modbus.c
  * Description        : Code for modbus protocol
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-2
  * @brief   Modbus
	******************************************************************************
  * @attention
  * <h2><center>&copy; Copyright (c) kmakise
  * All rights reserved.
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "modbus.h"
#include "golbaldata.h"
#include "SoftCRC.h"

/*----------------------------------------------------------21*/

void putInt16(void *p,int16_t Int)
{
	char *pch = p;
	pch[0] = Int>>8;
	pch[1] = Int&0xff;
}           

int16_t getInt16(void *p)
{
    char *pch = p;
    return ((int16_t)pch[0]<<8)+pch[1];
} 

/*----------------------------------------------------------2143*/
   
         
void putInt32(void *p,int32_t l)
{
	char *pch = (char *)p;
	char *pl = (char *)&l;
	pl[0] = pch[1];
	pl[1] = pch[0];
	pl[2] = pch[3];
	pl[3] = pch[2];
} 
int32_t getInt32(void *p)
{
	int32_t l;
	char *pch = (char *)p;
	char *pl = (char *)&l;
	pch[0] = pl[1];
	pch[1] = pl[0];
	pch[2] = pl[3];
	pch[3] = pl[2];
	return l;
}  

void putFloat(void *p,float f)
{
	char *pch = (char *)p;
	char *pf = (char *)&f;
	pch[0] = pf[1];
	pch[1] = pf[0];
	pch[2] = pf[3];
	pch[3] = pf[2];
}          

float getFloat(void *p)
{
	float f;
	char *pch = (char *)p;
	char *pf = (char *)&f;
	pf[0] = pch[1];
	pf[1] = pch[0];
	pf[2] = pch[3];
	pf[3] = pch[2];
	return f;
}     

void putDouble(void *p,double d)
{
	char *pch = (char *)p;
	char *pd = (char *)&d;
	//21436587
	pch[0] = pd[1];
	pch[1] = pd[0];
	pch[2] = pd[3];
	pch[3] = pd[2];
	pch[4] = pd[5];
	pch[5] = pd[4];
	pch[6] = pd[7];
	pch[7] = pd[6];
}          

double getDouble(void *p)
{
	double f;
	char *pch = (char *)p;
	char *pd = (char *)&f;
	//21436587
	pd[0] = pch[1];
	pd[1] = pch[0];
	pd[2] = pch[3];
	pd[3] = pch[2];
	pd[4] = pch[5];
	pd[5] = pch[4];
	pd[6] = pch[7];
	pd[7] = pch[6];
	return f;
}  

/*Modbus RTU - ASCII 协议转换 ------------------------------------------------*/

const uint8_t CHAS[16] =
{
	'0','1','2','3','4','5','6','7','8','9',
	'A','B','C','D','E','F'
};

uint8_t ATC(uint8_t a)
{
	switch (a)
	{
	case '0':return 0x00;
	case '1':return 0x01;
	case '2':return 0x02;
	case '3':return 0x03;
	case '4':return 0x04;
	case '5':return 0x05;
	case '6':return 0x06;
	case '7':return 0x07;
	case '8':return 0x08;
	case '9':return 0x09;
	case 'A':return 0x0A;
	case 'B':return 0x0B;
	case 'C':return 0x0C;
	case 'D':return 0x0D;
	case 'E':return 0x0E;
	case 'F':return 0x0F;
	}
}

uint8_t getLRC(uint8_t * buf, uint16_t len)
{
	uint16_t lrc = 0;

	for (int i = 0; i < len; i++)
	{
		lrc += buf[i];
	}

	lrc = lrc % 256;
	lrc = (~lrc) + 1;
	return lrc;
}



void putChToASCII(uint8_t* p, uint8_t b)
{
	p[0] = CHAS[((b & 0xF0) >> 4)];
	p[1] = CHAS[((b & 0x0F) >> 0)];
}

uint8_t getASCIItoCh(uint8_t* p)
{
	uint8_t ch = 0;
	ch = (ATC(p[0]) << 4) | ATC(p[1]);
	return ch;
}

uint16_t putRTUtoASCII(uint8_t * abuf, uint8_t * rbuf,uint8_t len)
{
	uint16_t alen = 0;

	//翻译方式 开头添加':' CRC替换为LRC 结尾添加0D 0A
	abuf[alen++] = ':';

	//填充内容
	for (int i = 0; i < (len - 2); i++)
	{
		putChToASCII(&abuf[alen], rbuf[i]);
		alen += 2;
	}

	//LRC
	uint8_t lrc = getLRC(rbuf,len - 2);
	putChToASCII(&abuf[alen],lrc);
	alen += 2;


	//结束
	abuf[alen++] = 0x0D;
	abuf[alen++] = 0x0A;

	return alen;
}


uint16_t putASCIItoRTU(uint8_t* rbuf, uint8_t* abuf, uint8_t len)
{
	uint16_t acur = 0;
	uint16_t rlen = 0;
	
	if(len < 10)return 0;
	
	
	//翻译方式 去头':' 去尾 验证LRC 通过后替换为 CRC
	for (int i = 0; i < ((len - 3) / 2); i++)
	{
		rbuf[rlen++] = getASCIItoCh(&abuf[acur + 1]);
		acur += 2;
	}

	
	//LRC
	uint8_t lck = rbuf[rlen - 1];
	uint8_t lrc = getLRC(rbuf,rlen - 1);
	

	if (lrc == lck)
	{
		//crc
		uint16_t SendCrc = CalCRC16(rbuf,rlen - 1);
		putInt16(&rbuf[rlen - 1], SendCrc);
		rlen += 1;
		return rlen;
	}
	return 0;
}








