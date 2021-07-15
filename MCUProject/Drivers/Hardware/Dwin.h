#ifndef __DWIN_H
#define __DWIN_H
#include "main.h"

float    dwinChToFloat(void *p);
uint32_t dwinChToInt32(void *p);

//写RTC
void Disp_WrRtc(void);
//写16位整数
void Disp_WrInt16(uint16_t addr,uint16_t data);
//写32位整型
void Disp_WrInt32(uint16_t addr,int32_t ldata);
//写浮点数
void Disp_WrFloat(uint16_t addr,float fdata);
//写数组
void Disp_WrStr(uint16_t addr,char *p);
//色块填充
void Disp_WrBlock(uint16_t addr,uint16_t x0,uint16_t y0,uint16_t w,uint16_t h,uint16_t color);

#endif /*__DWIN_H*/
