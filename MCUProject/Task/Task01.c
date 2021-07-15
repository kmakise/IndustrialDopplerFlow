/**
  ******************************************************************************
  * File Name          : Task01.c
  * Description        : Code for freeRTOS Task01 Application
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-1
  * @brief   SD信息记录
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

char 							SDPath[4];                  			 	/* SD卡逻辑设备路径 */
FATFS 						fs;																	/* FatFs文件系统对象 */
FIL 							file;																/* 文件对象 */
FRESULT 					f_res;                    					/* 文件操作结果 */
UINT 							fnum;            					  				/* 文件成功读写数量 */
BYTE 							ReadBuffer[1024]={0};        				/* 读缓冲区 */
uint8_t 					SDMountState;												/* sd卡挂载状态 0 未挂载 1已挂载*/
extern 						Diskio_drvTypeDef  USER_Driver; 		/* 驱动函数描述*/
BYTE 							WriteBuffer[1024];									/* 写缓冲区*/ 
uint32_t 					memsize = 0;												/* 磁盘大小*/


//sd卡挂载
uint8_t SDmount(void)
{
	//开始挂载
	if(FATFS_LinkDriver(&USER_Driver, SDPath) == 0)
  {
    //在SD卡挂载文件系统，文件系统挂载时会对SD卡初始化
    f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
    /*----------------------- 格式化测试 ---------------------------*/  
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if(f_res == FR_NO_FILESYSTEM)
    {
      /* 格式化 */
      f_res = f_mkfs((TCHAR const*)SDPath,0,0);							
      
      if(f_res == FR_OK)
      {
        /* 格式化后，先取消挂载 */
        f_res = f_mount(NULL,(TCHAR const*)SDPath,1);			
        /* 重新挂载	*/			
        f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
      }
      else
      {
        //格式化失败
        return 0;
      }
    }
    else if(f_res!=FR_OK)
    {
			//挂载失败
      return 0;
    }
    else
    {
			//挂载成功
			return 1;
    }
	}
	return 0;
}
//移除检查
void diskRemoveCheck(void)
{
	//移除
	if(SDCD_CHECK != 0)
	{
		SDMountState = 0;
	}
}


//磁盘挂载
void diskMountMaster(void)
{
	//挂载状态
	uint8_t mountstate = 0;
	//移除检查
	diskRemoveCheck();
	
	//未挂载
	if(SDMountState == 0)
	{
		//等待挂载
		while(mountstate == 0)
		{
			//等待接入
			while(SDCD_CHECK != 0);
			//挂载
			mountstate = SDmount();
		}
		SDMountState = 1;
	}
}


//创建文件写入标题
void createCsvFile(void)
{
	f_res = f_open(&file, "0:/流量数据.csv",FA_CREATE_ALWAYS | FA_WRITE );
	if ( f_res == FR_OK )
	{
		f_printf (&file,"时间日期,流速,瞬时流量,累积流量,温度,静压液位\n");
		f_close(&file);
	}
}

//记录间隔
void recDelay(int sec)
{
	//阻塞时间间隔
	sec = (sec > 0) ? sec : 60;
	for(int i = 0;i < sec;i++)
	{
		osDelay(1000);
	}
}


//数据记录
void dataRecording(void)
{
	//已挂载 
	if(SDMountState == 1)
	{
		//打开文件
		f_res = f_open(&file, "0:/流量数据.csv",FA_OPEN_EXISTING | FA_WRITE );
		
		//文件存在
		if ( f_res == FR_OK )
		{
			//按照文件现有大小移动指针
			f_lseek(&file,file.fptr + file.fsize);
			
			//向文件写入数据
			//								时间日期，				流速，瞬时流量，累积流量，温度，压力
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
		//文件不存在
		else
		{
			createCsvFile();
		}
	}
}

/* SD卡表格读写
 *
 * 接入检查
 *  接入 
 *   挂载
 *
 * 时间间隔控制循环
 *
 * 接入挂载有效
 * 	1.文件名（日期）读取文件 或创建文件
 * 	2.写入内容
 * 	3.关闭文件
 *
 **/
void Task01Main(void)
{
	osDelay(5000);
	for(;;)
	{
		
#ifndef DEBUGGING
		
		//磁盘挂载
		diskMountMaster();
		//数据记录
		dataRecording();
		//记录间隔
		recDelay(Cfg.dsecsd);
		
#else 
		osDelay(100);
#endif
	}
}



