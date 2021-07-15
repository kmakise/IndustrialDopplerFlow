
#ifndef __FM25CL64_H
#define __FM25CL64_H

//单字节写FM
void Write_FM25CL64_Byte(unsigned int addr,char c);

//单字节读FM
unsigned char Read_FM25CL64_Byte(unsigned int addr);

//多字节写FM
void WriteFM(unsigned int addr,void *pTx,unsigned int n);

//多字节读FM
void ReadFM(unsigned int addr,void *pRec,unsigned int n);

//FM 填充
void WriteFMFill(unsigned int addr,char data,unsigned int n);


#endif /**/


