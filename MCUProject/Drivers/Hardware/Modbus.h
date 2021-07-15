#ifndef __MODBUS_H
#define __MODBUS_H

#include "golbaldata.h"

//21
void putInt16(void *p,int16_t Int);
int16_t getInt16(void *p);

//2143
void putFloat(void *p,float f);
float getFloat(void *p);
void putInt32(void *p,int32_t l);
int32_t getInt32(void *p);
void putDouble(void *p,double d);
double getDouble(void *p);

//RTU ×ª»» ASCII
uint16_t putRTUtoASCII(uint8_t * abuf, uint8_t * rbuf,uint8_t len);
//ASCII ×ª»» RTU
uint16_t putASCIItoRTU(uint8_t* rbuf, uint8_t* abuf, uint8_t len);

#endif /*__MOSDBUS_H*/
