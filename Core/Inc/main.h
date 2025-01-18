/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32l4xx_hal.h"

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
#define I2C_C1_SCL_Pin GPIO_PIN_0
#define I2C_C1_SCL_GPIO_Port GPIOC
#define I2C_C1_SDA_Pin GPIO_PIN_1
#define I2C_C1_SDA_GPIO_Port GPIOC
#define C3_NH3_Pin GPIO_PIN_1
#define C3_NH3_GPIO_Port GPIOA
#define C3_NO2_Pin GPIO_PIN_2
#define C3_NO2_GPIO_Port GPIOA
#define C3_CO_Pin GPIO_PIN_3
#define C3_CO_GPIO_Port GPIOA
#define KP_NSS_Pin GPIO_PIN_4
#define KP_NSS_GPIO_Port GPIOA
#define KP_SCK_Pin GPIO_PIN_5
#define KP_SCK_GPIO_Port GPIOA
#define KP_MISO_Pin GPIO_PIN_6
#define KP_MISO_GPIO_Port GPIOA
#define KP_MOSI_Pin GPIO_PIN_7
#define KP_MOSI_GPIO_Port GPIOA
#define C2_Fotodioda_Pin GPIO_PIN_4
#define C2_Fotodioda_GPIO_Port GPIOC
#define C2_PIN1_Pin GPIO_PIN_5
#define C2_PIN1_GPIO_Port GPIOC
#define C2_PIN2_Pin GPIO_PIN_0
#define C2_PIN2_GPIO_Port GPIOB
#define C2_PIN3_Pin GPIO_PIN_1
#define C2_PIN3_GPIO_Port GPIOB
#define I2C_C4_SCL_Pin GPIO_PIN_10
#define I2C_C4_SCL_GPIO_Port GPIOB
#define I2C_C4_SDA_Pin GPIO_PIN_11
#define I2C_C4_SDA_GPIO_Port GPIOB
#define CAN_RX_Pin GPIO_PIN_11
#define CAN_RX_GPIO_Port GPIOA
#define CAN_TX_Pin GPIO_PIN_12
#define CAN_TX_GPIO_Port GPIOA
#define L2_Pin GPIO_PIN_10
#define L2_GPIO_Port GPIOC
#define L3_Pin GPIO_PIN_11
#define L3_GPIO_Port GPIOC
#define L1_Pin GPIO_PIN_12
#define L1_GPIO_Port GPIOC
#define L4_Pin GPIO_PIN_2
#define L4_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
