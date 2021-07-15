#ifndef __DATATYPEDEF_H
#define __DATATYPEDEF_H

#include "main.h"
#include "cmsis_os.h"

//ʱ������
typedef struct
{
	uint8_t year;
	uint8_t mon;
	uint8_t day;
	uint8_t week;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	
	uint8_t update;
	
}TimeTpyedef;

//���ζ��л�����
typedef struct
{
	uint8_t buf[256];	
	uint8_t cnt;			//д���±�
	uint8_t rd;				//��ȡ�±�
	
}ComBufTypedef;

//��������
typedef struct
{
	float 		flows;		//0		����
	float 		levelsp;	//2		��ѹҺλ
	uint32_t	uid;			//6		���������
	float 		temp;			//38	�¶�
	float 		levelus;	//40	����Һλ
	float 		flowdt;		//42	˲ʱ����
	double 		flowit;		//44	�ۻ�����
	float 		dir;			//48	���� 1����-1
	
	uint16_t  state;		// ABͨ��״̬
	
}MsuDataTyoedef;


//���ò���
typedef struct
{
	//����������
	uint16_t	ptl;			//100 Э������1 modbusRTU��2 ModbusASCII
	uint16_t	addr;			//101	�豸��ַ1~254
	uint16_t	fk;				//102 �˲�ϵ��1~99
	uint16_t	bps;			//106 ������1~9��4����9600��Ĭ�ϣ�1-1200��2-2400��3-4800��4-9600��5-14400��6-19200��7-38400, 8-57600, 9-115200
	uint16_t 	canal;		//110	ˮ������ ��0���Ρ�1Բ�Ρ�2�������Ρ�3�����Σ�
	float			bcpt;			//112	�ײ�����
	float			angel;		//114	�Ƕ�
	float			wsize;		//116	��ȳߴ�
	float			thd;			//120 ��������
	float			fsk;			//122 �����ʶ�ϵ����Ĭ��1
	uint16_t	flowdtS;	//128 ����˲ʱ������Һλѡ��0������1ѹ��
	uint16_t	flowdtT;	//130 ˲ʱ������λѡ�� ��0 ������/s; 1������/h
	double		flowitSet;//132 �����ۼ�����
	
	float			out4ma;		//200 4ma���
	float  		out20ma;	//202 20ma���
	float			relay1;		//204 �̵���1
	float			relay2;		//206 �̵���2
	uint16_t	dsecsd;		//208 ��¼���
	
	uint32_t  uid;
	
	
	uint8_t		update;		//���ݸ���״̬
	//step1����ַ��ֵ(100-132)ʱд�봫�������� д����ɺ���λFMд��״̬
	//       ������� 201 Э���޸� 202 �������޸� 203������ַ�޸�
	//step2��FMд��״̬(0xFF)ʱ д��FM�洢�� д����ɺ����״̬�������
	
}ConfigTypdef;



#endif /*__DATATYPEDEF_H*/
