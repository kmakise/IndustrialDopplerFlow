
#ifndef __FM25CL64_H
#define __FM25CL64_H

//���ֽ�дFM
void Write_FM25CL64_Byte(unsigned int addr,char c);

//���ֽڶ�FM
unsigned char Read_FM25CL64_Byte(unsigned int addr);

//���ֽ�дFM
void WriteFM(unsigned int addr,void *pTx,unsigned int n);

//���ֽڶ�FM
void ReadFM(unsigned int addr,void *pRec,unsigned int n);

//FM ���
void WriteFMFill(unsigned int addr,char data,unsigned int n);


#endif /**/


