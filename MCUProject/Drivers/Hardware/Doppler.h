#ifndef __DOPPLER_H
#define __DOPPLER_H

#include "golbaldata.h"




//��ȡ��������
void sensorMeasureDataRead(int p,MsuDataTyoedef * sr);
//��ȡ��������
void sensorConfigDataRead(int p,ConfigTypdef * cfg);
//д�����ò��� ��������
void sensorConfigDataWrite(int p,ConfigTypdef * cfg);


#endif /*__DOPPLER_H*/
