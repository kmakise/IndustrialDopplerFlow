#ifndef __GOLBALDATA_H
#define __GOLBALDATA_H

#include "DataTypedef.h"
#include "main.h"


//--����ȫ�ֱ���-----
#define EXT extern

//#define DEBUGGING

#define FALSE 0
#define TRUE 1
	
#define HSVERSION 3.08f		//�汾��

//�̵���
#define RELAY1(a) HAL_GPIO_WritePin(WPG1_GPIO_Port,WPG1_Pin,(a))
#define RELAY2(a) HAL_GPIO_WritePin(WPG2_GPIO_Port,WPG2_Pin,(a))

//ͨ�Ŷ˿ڶ���
#define TXEN 1		//����ʹ��
#define RXEN 0		//����ʹ��
#define COMSA 0		//������A�˿�
#define COMSB 1		//������B�˿�
#define	COMPC 2		//PC�˶˿�
#define COMDW 3		//�������˿�

//����ģʽ
#define MODEDIS 0x00 //��Чģʽ
#define MODECHA 0x5A //����Aͨ��ģʽ
#define MODECHB 0xA5 //����Bͨ��ģʽ
#define MODEALL 0xAA //˫ͨ��ģʽ

//FM�洢����ַ��
#define FM_ADDR_CFG_BASE  (8)
#define FM_ADDR_CFG_BKUP  (FM_ADDR_CFG_BASE + sizeof(ConfigTypdef) + 10)

//Modbus
#define ADDR_MAX_READ 160	//���Ѱַ��Χ
#define ADDR_UID_READ 101	//UID�洢��ַ

//sd������ �͵�ƽ��Ч
#define SDCD_CHECK HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

//ͨ�Ż����� 0-4
EXT ComBufTypedef ComTx[5];
EXT ComBufTypedef ComRx[5];

//��������������
EXT MsuDataTyoedef Sdata;

//���ò���
EXT ConfigTypdef Cfg;	

//ʱ������
EXT TimeTpyedef Time;



#endif /*__GOLBALDATA_H*/
