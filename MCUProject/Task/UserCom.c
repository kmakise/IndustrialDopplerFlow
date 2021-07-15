/**
  ******************************************************************************
  * File Name          : UserCom.c
  * Description        : Code for usart communication it txrx func
  ******************************************************************************
  * @author  kmakise
  * @version V1.0.0
  * @date    2021-3-2
  * @brief   串口通信
	******************************************************************************
  * @attention
  * <h2><center>&copy; Copyright (c) kmakise
  * All rights reserved.
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "UserCom.h"
#include "golbaldata.h"
#include "usart.h"

//通信端口
const USART_TypeDef * UARTCOM[5] = {
	USART1,USART2,USART3,UART4,UART5
};

//通信缓冲区
ComBufTypedef ComTx[5];
ComBufTypedef ComRx[5];

//中断数据接收
void RecToBuf(int p)
{
	if(UARTCOM[p]->SR&(1<<5)) 
	{
		if(p != COMDW)
		{
			ComRx[p].buf[ComRx[p].cnt++] = (UARTCOM[p]->DR&(uint8_t)0x00FF);
		}
		else
		{
			ComRx[p].buf[ComRx[p].cnt++] = (UARTCOM[p]->DR&(uint8_t)0x00FF);
			//应答消息清除
			if( (ComRx[p].cnt > 4) && 
					(ComRx[p].buf[ComRx[p].cnt - 1] == 0x4b) && 
					(ComRx[p].buf[ComRx[p].cnt - 2] == 0x4f) && 
					(ComRx[p].buf[ComRx[p].cnt - 4] == 0x03) &&
					(ComRx[p].buf[ComRx[p].cnt - 5] == 0xA5) &&
					(ComRx[p].buf[ComRx[p].cnt - 6] == 0x5A) )
			{
				ComRx[p].cnt -= 6;
			}	
		}
	}
}

//发送接收使能
void TRenSet(int p,int s)
{
	if(s != 0)
	{
		switch(p)
		{
			case 0:TXDEN1_GPIO_Port->BSRR = TXDEN1_Pin;break;
			case 1:TXDEN2_GPIO_Port->BSRR = TXDEN2_Pin;break;
			case 2:TXDEN3_GPIO_Port->BSRR = TXDEN3_Pin;break;
			default:break;
		}
	}
	else
	{
		switch(p)
		{
			case 0:TXDEN1_GPIO_Port->BRR = TXDEN1_Pin;break;
			case 1:TXDEN2_GPIO_Port->BRR = TXDEN2_Pin;break;
			case 2:TXDEN3_GPIO_Port->BRR = TXDEN3_Pin;break;
			default:break;
		}
	}
}

//数据发送
void TxData(int p)
{
	//485发送使能
	TRenSet(p,TXEN);
	//发送数据
	switch(p)
	{
		case 0:HAL_UART_Transmit(&huart1,ComTx[p].buf,ComTx[p].cnt,1000);break;
		case 1:HAL_UART_Transmit(&huart2,ComTx[p].buf,ComTx[p].cnt,1000);break;
		case 2:HAL_UART_Transmit(&huart3,ComTx[p].buf,ComTx[p].cnt,1000);break;
		case 3:HAL_UART_Transmit(&huart4,ComTx[p].buf,ComTx[p].cnt,1000);break;
		case 4:HAL_UART_Transmit(&huart5,ComTx[p].buf,ComTx[p].cnt,1000);break;
		default:break;
	}
	//485发送失能
	TRenSet(p,RXEN);

}


