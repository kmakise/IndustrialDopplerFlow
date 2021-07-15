/**
  ******************************************************************************
  * File Name          : Task06.c
  * Description        : Code for freeRTOS Task05 Application
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-1
  * @brief   电流输出控制 继电器控制
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
#include "Task06.h"
#include "Task02.h"
#include "tim.h"

#include "golbaldata.h"



//继电器控制
void relayCtrl(float m,float r1,float r2)
{
	//1对应高流量，高于报警
	if(m > r1 && r1 != 0)
	{
		RELAY1(GPIO_PIN_SET);
	}
	else
	{
		RELAY1(GPIO_PIN_RESET);
	}
	//2对应低流量
	if(m < r2 && r2 != 0)
	{
		RELAY2(GPIO_PIN_SET);
	}
	else
	{
		RELAY2(GPIO_PIN_RESET);
	}
}

void relayTFmaster(void)
{
	//m^3/h
	if(Cfg.flowdtT != 0)
	{
		relayCtrl(Sdata.flowdt				,Cfg.relay1,Cfg.relay2);
	}
	//m^3/s
	else
	{
		relayCtrl(Sdata.flowdt * 3600	,Cfg.relay1,Cfg.relay2);
	}
}

//电流输出控制
void currentCtrl(float c20,float c4,float fl)
{
	//4 16600
	//24 100
	float dec = 16500;		//分辨率
	float dur = 0;				//电流区间
	float det = 0;				//增量
	uint16_t ccr = 0;

	if (c20 > c4)		//正向输出模式
	{
		dur = c20 - c4;
		if (c4 < fl && c20 > fl)
		{
			det = fl - c4;
			ccr = 16600 - (dec * (det / dur));
		}
		else if (c4 > fl)
		{
			ccr = 100;
		}
		else 
		{
			ccr = 16600;
		}
	}
	else if (c20 < c4)	//反向输出模式
	{
		dur = c4 - c20;
		if (c20 < fl && c4 > fl)
		{
			det = fl - c20;
			ccr = 100 + (dec * (det / dur));
		}
		else if (c20 > fl)
		{
			ccr = 16600;
		}
		else
		{
			ccr = 100;
		}
	}
	TIM3->CCR3 = ccr;
}


void currentTFmaster(void)
{
	//m^3/h
	if(Cfg.flowdtT != 0)
	{
		currentCtrl(Cfg.out20ma,Cfg.out4ma,Sdata.flowdt);
	}
	//m^3/s
	else
	{
		currentCtrl(Cfg.out20ma,Cfg.out4ma,Sdata.flowdt * 3600);
	}
}

/*Golbal data space ----------------------------------------------------------*/

void Task06Main(void)
{
	//等待FM参数读取
	waitCfgInitState();
	
	//PWMout
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	
	for(;;)
	{
		relayTFmaster();
		currentTFmaster();

		osDelay(100);
	}
}



