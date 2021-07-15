#ifndef __DWIN_H
#define __DWIN_H
#include "main.h"

float    dwinChToFloat(void *p);
uint32_t dwinChToInt32(void *p);

//дRTC
void Disp_WrRtc(void);
//д16λ����
void Disp_WrInt16(uint16_t addr,uint16_t data);
//д32λ����
void Disp_WrInt32(uint16_t addr,int32_t ldata);
//д������
void Disp_WrFloat(uint16_t addr,float fdata);
//д����
void Disp_WrStr(uint16_t addr,char *p);
//ɫ�����
void Disp_WrBlock(uint16_t addr,uint16_t x0,uint16_t y0,uint16_t w,uint16_t h,uint16_t color);

#endif /*__DWIN_H*/
