/**
  ******************************************************************************
  * File Name          : Task01.c
  * Description        : Code for freeRTOS Task01 Application
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-1
  * @brief   SD��Ϣ��¼
	******************************************************************************
  * @attention
  * <h2><center>&copy; Copyright (c) kmakise
  * All rights reserved.
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "Task01.h"

#include "golbaldata.h"
#include "SDcard.h"
#include "fatfs.h"
#include "stdio.h"

/*Golbal data space ----------------------------------------------------------*/

char 							SDPath[4];                  			 	/* SD���߼��豸·�� */
FATFS 						fs;																	/* FatFs�ļ�ϵͳ���� */
FIL 							file;																/* �ļ����� */
FRESULT 					f_res;                    					/* �ļ�������� */
UINT 							fnum;            					  				/* �ļ��ɹ���д���� */
BYTE 							ReadBuffer[1024]={0};        				/* �������� */
uint8_t 					SDMountState;												/* sd������״̬ 0 δ���� 1�ѹ���*/
extern 						Diskio_drvTypeDef  USER_Driver; 		/* ������������*/
BYTE 							WriteBuffer[1024];									/* д������*/ 
uint32_t 					memsize = 0;												/* ���̴�С*/


//sd������
uint8_t SDmount(void)
{
	//��ʼ����
	if(FATFS_LinkDriver(&USER_Driver, SDPath) == 0)
  {
    //��SD�������ļ�ϵͳ���ļ�ϵͳ����ʱ���SD����ʼ��
    f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
    /*----------------------- ��ʽ������ ---------------------------*/  
    /* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
    if(f_res == FR_NO_FILESYSTEM)
    {
      /* ��ʽ�� */
      f_res = f_mkfs((TCHAR const*)SDPath,0,0);							
      
      if(f_res == FR_OK)
      {
        /* ��ʽ������ȡ������ */
        f_res = f_mount(NULL,(TCHAR const*)SDPath,1);			
        /* ���¹���	*/			
        f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
      }
      else
      {
        //��ʽ��ʧ��
        return 0;
      }
    }
    else if(f_res!=FR_OK)
    {
			//����ʧ��
      return 0;
    }
    else
    {
			//���سɹ�
			return 1;
    }
	}
	return 0;
}
//�Ƴ����
void diskRemoveCheck(void)
{
	//�Ƴ�
	if(SDCD_CHECK != 0)
	{
		SDMountState = 0;
	}
}


//���̹���
void diskMountMaster(void)
{
	//����״̬
	uint8_t mountstate = 0;
	//�Ƴ����
	diskRemoveCheck();
	
	//δ����
	if(SDMountState == 0)
	{
		//�ȴ�����
		while(mountstate == 0)
		{
			//�ȴ�����
			while(SDCD_CHECK != 0);
			//����
			mountstate = SDmount();
		}
		SDMountState = 1;
	}
}


//�����ļ�д�����
void createCsvFile(void)
{
	f_res = f_open(&file, "0:/��������.csv",FA_CREATE_ALWAYS | FA_WRITE );
	if ( f_res == FR_OK )
	{
		f_printf (&file,"ʱ������,����,˲ʱ����,�ۻ�����,�¶�,��ѹҺλ\n");
		f_close(&file);
	}
}

//��¼���
void recDelay(int sec)
{
	//����ʱ����
	sec = (sec > 0) ? sec : 60;
	for(int i = 0;i < sec;i++)
	{
		osDelay(1000);
	}
}


//���ݼ�¼
void dataRecording(void)
{
	//�ѹ��� 
	if(SDMountState == 1)
	{
		//���ļ�
		f_res = f_open(&file, "0:/��������.csv",FA_OPEN_EXISTING | FA_WRITE );
		
		//�ļ�����
		if ( f_res == FR_OK )
		{
			//�����ļ����д�С�ƶ�ָ��
			f_lseek(&file,file.fptr + file.fsize);
			
			//���ļ�д������
			//								ʱ�����ڣ�				���٣�˲ʱ�������ۻ��������¶ȣ�ѹ��
			sprintf((char *)WriteBuffer,"%d-%d-%d %d:%d:%d ,%f,%f,%f,%f,%f\n",
				Time.year,
				Time.mon,
				Time.day,
				Time.hour,
				Time.min,
				Time.sec,
				Sdata.flows,
				Sdata.flowdt,
				Sdata.flowit,
				Sdata.temp,
				Sdata.levelsp						
			);
			
			f_printf(&file,(const char *)WriteBuffer);
			f_close(&file);
		
		}
		//�ļ�������
		else
		{
			createCsvFile();
		}
	}
}

/* SD������д
 *
 * ������
 *  ���� 
 *   ����
 *
 * ʱ��������ѭ��
 *
 * ���������Ч
 * 	1.�ļ��������ڣ���ȡ�ļ� �򴴽��ļ�
 * 	2.д������
 * 	3.�ر��ļ�
 *
 **/
void Task01Main(void)
{
	osDelay(5000);
	for(;;)
	{
		
#ifndef DEBUGGING
		
		//���̹���
		diskMountMaster();
		//���ݼ�¼
		dataRecording();
		//��¼���
		recDelay(Cfg.dsecsd);
		
#else 
		osDelay(100);
#endif
	}
}



