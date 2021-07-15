#ifndef __DOPPLER_H
#define __DOPPLER_H

#include "golbaldata.h"




//读取测量数据
void sensorMeasureDataRead(int p,MsuDataTyoedef * sr);
//读取配置数据
void sensorConfigDataRead(int p,ConfigTypdef * cfg);
//写入配置参数 到传感器
void sensorConfigDataWrite(int p,ConfigTypdef * cfg);


#endif /*__DOPPLER_H*/
