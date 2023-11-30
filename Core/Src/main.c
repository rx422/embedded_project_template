/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUTTON_READ_RECURENCE	200
#define LED_GREEN_GPIO_Port		MOTOR_A_GPIO_Port
#define LED_GREEN_Pin			MOTOR_A_Pin
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

int _write(int file, char *ptr, int len)
{
	(void) file;

	HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, HAL_MAX_DELAY);

	return len;
}

int _read(int file, char *ptr, int len)
{
	(void) file;

	__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_ORE);
	HAL_UART_Receive(&huart2, (uint8_t*) ptr, len, HAL_MAX_DELAY);

	return len;
}

// Another method of redirecting input-output flows is presented below.
/*
 int __io_putchar(int ch)
 {
 HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

 return HAL_OK;
 }

 int __io_getchar(void)
 {
 int ch;

 //	__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_ORE);
 __HAL_UART_CLEAR_OREFLAG(&huart2);

 HAL_UART_Receive(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

 return ch;
 }
 */

void changeOnceIfButtonPressed(void);
void changeIfButtonPressed(void);
void motorControl(motor_t *motor, const char *command);
uint8_t needReadUsartInputBuff(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	// No buffers will be used for input and output streams.
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	char input_data_buff[20];

	motor_t motor;



	const uint8_t FIRST_MOTOR_ID = 1;

	motor_init(&motor, FIRST_MOTOR_ID, 0,
	MOTOR_A_Pin,
	MOTOR_B_Pin,
	MOTOR_A_GPIO_Port);

	printf("%s\n", "Hello, kitty!");

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if (needReadUsartInputBuff())
		{
			scanf("%20s", input_data_buff);
			printf("\a"); // Bip sound
			printf("\f%s\n", input_data_buff); // Refresh display and print input_data_buff
			motorControl(&motor, input_data_buff);
		}
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void changeOnceIfButtonPressed(void)
{
	static uint32_t nextCheckTime = 0;

	if (nextCheckTime < HAL_GetTick())
	{
		uint8_t buttonState = !HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);

		if (buttonState)
		{
			HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

			nextCheckTime = HAL_GetTick() + BUTTON_READ_RECURENCE;
		}
	}
}

void changeIfButtonPressed(void)
{
	static uint32_t nextCheckTime = 0;

	uint8_t buttonState = !HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);

	if (buttonState)
	{
		if (nextCheckTime < HAL_GetTick())
		{
			HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
		}
		nextCheckTime = HAL_GetTick() + BUTTON_READ_RECURENCE;
	}
}

void motorControl(motor_t *motor, const char *command)
{
	if (!strcmp(command, "motorRight"))
	{
		motor_set_direction(motor, RIGHT);
		motor_start(motor);
	}

	if (!strcmp(command, "motorLeft"))
	{
		motor_set_direction(motor, LEFT);
		motor_start(motor);
	}

	if (!strcmp(command, "motorStart"))
	{
		motor_start(motor);
	}

	if (!strcmp(command, "motorStop"))
	{
		motor_stop(motor);
	}
}

inline uint8_t needReadUsartInputBuff(void)
{
	return (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
