/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOC
#define TXDEN2_Pin GPIO_PIN_1
#define TXDEN2_GPIO_Port GPIOA
#define FLASH_SPI_CS_Pin GPIO_PIN_4
#define FLASH_SPI_CS_GPIO_Port GPIOA
#define TXDEN3_Pin GPIO_PIN_2
#define TXDEN3_GPIO_Port GPIOB
#define FMCS_Pin GPIO_PIN_12
#define FMCS_GPIO_Port GPIOB
#define FMCK_Pin GPIO_PIN_13
#define FMCK_GPIO_Port GPIOB
#define FMSO_Pin GPIO_PIN_14
#define FMSO_GPIO_Port GPIOB
#define FMSI_Pin GPIO_PIN_15
#define FMSI_GPIO_Port GPIOB
#define TXDEN1_Pin GPIO_PIN_11
#define TXDEN1_GPIO_Port GPIOA
#define RTCSCL_Pin GPIO_PIN_6
#define RTCSCL_GPIO_Port GPIOB
#define RTCSDA_Pin GPIO_PIN_7
#define RTCSDA_GPIO_Port GPIOB
#define WPG1_Pin GPIO_PIN_8
#define WPG1_GPIO_Port GPIOB
#define WPG2_Pin GPIO_PIN_9
#define WPG2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
